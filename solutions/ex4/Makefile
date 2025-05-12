


all:
	arm-none-eabi-as boot.s -o boot.o
	arm-none-eabi-gcc -mcpu=cortex-a7 -ffreestanding -c kernel.c -o kernel.o
	arm-none-eabi-gcc -ffreestanding -c sha1.c -o sha1.o
	arm-none-eabi-gcc -ffreestanding -c sha256.c -o sha256.o
	arm-none-eabi-gcc -ffreestanding -c aes.c -o aes.o
	arm-none-eabi-gcc -ffreestanding -c binascii.c -o binascii.o
	arm-none-eabi-gcc -ffreestanding -c libc.c -o libc.o
	arm-none-eabi-ld  -nostdlib  -Tlinker.ld boot.o sha1.o sha256.o aes.o binascii.o libc.o kernel.o -o kernel.elf

clean:
	rm *.o
	rm kernel.elf
