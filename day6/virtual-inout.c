#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#if defined(__i386__)
#include <asm/sigcontext.h>
#endif

unsigned char inb(int port)
{
  int v;
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("inb %dx, %al");
  asm volatile ("mov %%eax, %0" : "=r"(v) :);
  return v & 0xff;
}

unsigned short inw(int port)
{
  int v;
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("inw %dx, %ax");
  asm volatile ("mov %%eax, %0" : "=r"(v) :);
  return  (v >> 16) & 0xffff;
}

unsigned int inl(int port)
{
  int v;
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("inl %dx, %eax");
  asm volatile ("mov %%eax, %0" : "=r"(v) :);
  return v;
}

void outb(int port, int v)
{
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("mov %0, %%eax" :: "r"(v));
  asm volatile ("outb %al, %dx");
}

void outw(int port, int v)
{
  port <<= 16;
  v <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("mov %0, %%eax" :: "r"(v));
  asm volatile ("outw %ax, %dx");
}

void outl(int port, int v)
{
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("mov %0, %%eax" :: "r"(v));
  asm volatile ("outl %eax, %dx");
}

void inout(void)
{
  int i;
  for (i = 0;i < 256;i++){
    printf("PORT[%d]:", i);
    outb(i, i); printf(" (byte)%02x", inb(i));
    outw(i, i); printf(" (word)%04x", inw(i));
    outl(i, i); printf(" (long)%08x", inl(i));
    printf("\n");
  }
}

struct sigframe {
#if defined(__i386__)
  int pretcode;
  int sig;
#endif
#if defined(__amd64__)
  long dummy[6];
#endif
  struct sigcontext sc;
};

void sigsegv(int val)
{
  long *sp;
  unsigned char *pc;
  struct sigframe *frame;
  static unsigned int port[0x10000];

  sp = __builtin_frame_address(0);
  frame = (struct sigframe *)(sp + 1);

#if defined(__i386__)
#define xDX edx
#define xAX eax
#define xIP eip
#endif
#if defined(__amd64__)
#define xDX rdx
#define xAX rax
#define xIP rip
#endif

#if 0 // <- 変更箇所
  printf("SP\t: %16lx\n", (long)sp);
  printf("FRAME\t: %16lx\n", (long)frame);
  printf("DX\t: %16lx\n", (long)frame->sc.xDX);
  printf("AX\t: %16lx\n", (long)frame->sc.xAX);
  printf("IP\t: %16lx\n", (long)frame->sc.xIP);
  exit(0);
#endif

  pc = (unsigned char *)frame->sc.xIP;

  switch (*pc) {
    case 0xec: /* inb (%dx), %al */
      frame->sc.xAX &= ~0xffffL;
      frame->sc.xAX |= port[(frame->sc.xDX >> 16) & 0xffff] & 0xff;
      pc++;
      goto ret;
    case 0xed: /* inl (%dx), %eax */
      frame->sc.xAX &= ~0xffffffffL;
      frame->sc.xAX |= port[(frame->sc.xDX >> 16) & 0xffff];
      pc++;
      goto ret;
    case 0xee: /* outb %al, (%dx) */
      port[(frame->sc.xDX >> 16) & 0xffff] &= ~0xff;
      port[(frame->sc.xDX >> 16) & 0xffff] |= frame->sc.xAX & 0xff;
      pc++;
      goto ret;
    case 0xef: /* outl %eax, (%dx) */
      port[(frame->sc.xDX >> 16) & 0xffff] &= ~0xffffffff;
      port[(frame->sc.xDX >> 16) & 0xffff] |= frame->sc.xAX;
      pc++;
      goto ret;
    case 0x66:
      switch (*(pc + 1)) {
        case 0xed: /* inw (%dx), %ax */
          frame->sc.xAX &= ~0xffff0000L;
          frame->sc.xAX |= (port[(frame->sc.xDX >> 16) & 0xffff] & 0xffff) << 16;
          pc += 2;
          goto ret;
        case 0xef: /* outw %ax, (%dx) */
          port[(frame->sc.xDX >> 16) & 0xffff] &= ~0xffff;
          port[(frame->sc.xDX >> 16) & 0xffff] |= (frame->sc.xAX >> 16) & 0xffff;
          pc += 2;
          goto ret;
        default:
          break;
      }
    default:
      break;
  }
  fprintf(stderr, "signal error\n");
  exit(0);

ret:
  frame->sc.xIP = (long)pc;
}

void setsig(void)
{
  signal(SIGSEGV, sigsegv);
}

int main(void)
{
  setsig();
  inout();
  return 0;
}
