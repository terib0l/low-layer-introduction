# play-with-assembly

## Environment

- Linux Mint
- x86_64

## Execute

### x86_64

```
$ gcc *.c
$ ./a.out
```

### aarch32

```
$ arm-linux-gnueabihf-gcc -static *.c
$ ./a.out
```

### aarch64

```
$ aarch64-linux-gnu-gcc -static *.c
$ ./a.out
```
