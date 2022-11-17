run x86:
	gcc *_x86.c -o x86

run arm32:
	arm-linux-gnueabihf-gcc -static *_aarch32.c -o aarch32

run arm64:
	aarch64-linux-gnu-gcc -static *_aarch64.c -o aarch64

clean:
	rm -f x86 aarch32 aarch64
