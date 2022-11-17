#include <stdio.h>
#include <setjmp.h>

void func(jmp_buf env)
{
  printf("func() in\n");
  longjmp(env, 1);
  printf("func() out\n");
}

int main()
{
  jmp_buf env;

  printf("main() in\n");

  if (setjmp(env) == 0) {
    printf("setjmp() return 0\n");
    func(env);
  }

  printf("setjpm() return !0\n");
  printf("main() out\n");

  return 0;
}
