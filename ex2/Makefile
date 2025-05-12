

# Hint: emulated board uses cortex-a9 which ARM 32 bits CPU
# Therefore, we use ARM 32 bits toolchain
all:
	arm-none-eabi-as boot.s -o boot.o
	arm-none-eabi-gcc -ffreestanding -c kernel.c -o kernel.o
	arm-none-eabi-ld -nostdlib -Tlinker.ld boot.o kernel.o -o kernel.elf
clean:
	rm *.o




