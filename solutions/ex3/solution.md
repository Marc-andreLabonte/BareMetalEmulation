# Qemu memory overlap error

```bash
/home/user/src/qemu/build/qemu-bundle/usr/local/share/qemu/opensbi-riscv64-generic-fw_dynamic.bin (addresses 0x0000000080000000 - 0x0000000080042878)
build/bin/rv64imac/qemu-sifive_u/hello ELF program header segment 1 (addresses 0x0000000080000000 - 0x0000000080000040)
```

---

## Disabling opensbi

Qemu tries to load opensbi at the same memory location our `kernel.elf` file is to be loaded, hence the overlap error.

One may add `-bios none` in qemu command line options to disable loading of opensbi

```bash
QEMUBIN=$HOME/src/qemu/build/qemu-system-riscv64

${QEMUBIN} -nographic -machine sifive_u \
  -bios none \
  -kernel build/bin/rv64imac/qemu-sifive_u/hello 
```

---

## What is opensbi

Opensbi runs with the highest level of CPU privilege, M-Mode.  Responsible for authenticating and loading kernel, which may run at a lesser level of privileges (S-Mode)

## References

- Riscv boot process: [https://popovicu.com/posts/risc-v-sbi-and-full-boot-process/](https://popovicu.com/posts/risc-v-sbi-and-full-boot-process/)
- Riscv privilege levels (WD slide deck): [https://riscv.org/wp-content/uploads/2024/12/13.30-RISCV_OpenSBI_Deep_Dive_v5.pdf](https://riscv.org/wp-content/uploads/2024/12/13.30-RISCV_OpenSBI_Deep_Dive_v5.pdf)

---

# PC = 0x1000 at boot instead of 0x80000000

The Zeroth Stage Bootloader (ZSBL)

Once you power on this virtual machine, QEMU fills the memory at 0x1000 with a few instructions and sets the program counter right to that address. 

**0x1000** is referenced as the default reset vector, DEFAULT_RSTVEC = **0x00001000**

See `target/riscv/cpu.c` in qemu source code

```c
// around line 1070
    env->pc = env->resetvec;
```

---

For the riscv `sifive_u` machine, the Zeroth stage boot loader is define in `hw/riscv/sifive_u.c` at line 620:

```c
    /* reset vector */
    uint32_t reset_vec[12] = {
        s->msel,                       /* MSEL pin state */
        0x00000297,                    /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
        0x02c28613,                    /*     addi   a2, t0, %pcrel_lo(1b) */
        0xf1402573,                    /*     csrr   a0, mhartid  */
        0,
        0,
        0x00028067,                    /*     jr     t0 */
        start_addr,                    /* start: .dword */
        start_addr_hi32,
        fdt_load_addr,                 /* fdt_laddr: .dword */
        fdt_load_addr_hi32,
        0x00000000,
```

---

Below is a screen capture from a gdb session. Address 0x1000 contains the zero stage boot loader.  The program counter will start at **0x1004**.

```c
 ► 0x1004    auipc  t0, 0            T0 => 0x1004
   0x1008    addi   a2, t0, 0x2c     A2 => 0x1030 (0x1004 + 0x2c)
   0x100c    csrr   a0, mhartid
   0x1010    ld     a1, 0x20(t0)     A1, [0x1024] => 0x87e00000 ◂— 0xb130000edfe0dd0 // dtb
   0x1014    ld     t0, 0x18(t0)     T0, [0x101c] => 0x80000000 (_start) ◂— 0xe02211410040006f /* address of our code */
   0x1018    jr     t0                          <_start>

```

---

Data stored at **0x1000** is not executable code but the Mode Select (MSEL) which is data.  

It is being used to set where the next stage will be loaded from.  It can take the following values:

- 0x0: Defaut, next stage will be loaded from RAM
- 0x1: Will be loaded from memory mapped SPI flash (MSEL_MEMMAP_QSPI0_FLASH)
- 0x6: Will be loaded from SPI flash (MSEL_L2LIM_QSPI0_FLASH)
- 0x11: Will be loaded from SD card (MSEL_L2LIM_QSPI2_SD)

---

## References

- Qemu source code: [https://github.com/qemu/qemu/blob/master/hw/riscv/sifive_u.c](https://github.com/qemu/qemu/blob/master/hw/riscv/sifive_u.c)
- Qemu headers : [https://github.com/qemu/qemu/blob/master/include/hw/riscv/sifive_u.h](https://github.com/qemu/qemu/blob/master/include/hw/riscv/sifive_u.h)
- Uros Popovic post: [https://popovicu.com/posts/bare-metal-programming-risc-v/](https://popovicu.com/posts/bare-metal-programming-risc-v/)
- slavaim riscv notes: [https://github.com/slavaim/riscv-notes/blob/master/bbl/boot.md](https://github.com/slavaim/riscv-notes/blob/master/bbl/boot.md)

---

# Stack pointer not set 

Stack pointer does not get set by either ZSBL or our code.  

We must set `SP` register to some sensible value.  One possible value is top of RAM as stack will grow downwards.

First, we need to define the __stack_top symbol in the linker script

Our linker script is `env/qemu-sifive_u/default.lds`

Add a PROVIDE statement below the MEMORY STATEMENT to define the `__stack_top` symbol

```c
PROVIDE(__stack_top = ORIGIN(ram) + LENGTH(ram));
```

---

Then, in the `C` runtime file, `env/qemu-sifive_u/crt.s`, add an instruction to load the `__stack_top` symbol into the `SP` register.  That will set the stack pointer to the top of RAM.

```c
_start:
    la      sp, __stack_top
```

---

# Wrong UART address

The string "Hello World!" still does not get printed.  The error message "<Cannot dereference [0x10013000]>" is show in gdb as we try to print a character. 

The address **0x10013000** is supposed to be the location of our memory mapped `UART`, what went wrong?

---

## Memory map reconnaissance: monitor method

Uart address is visible from memory tree in monitor

To access the monitor, one needs to enable it on the `Qemu` command line:

**Note**: It is recommanded to also enable the `gdb` stub so execution won't start right away

```bash
QEMUBIN=$HOME/src/qemu/build/qemu-system-riscv64

${QEMUBIN} -nographic -machine sifive_u \
  -bios none \
  -kernel build/bin/rv64imac/qemu-sifive_u/hello \
  -s -S \
  -monitor telnet:127.0.0.1:55555,server,nowait
```

---

Then, use netcat or similar tool to access it once `Qemu` is running

```bash
nc localhost 55555
```
---

Use `info mtree` command to display the memory map and locate the `UART` address.

```c
(qemu) info mtree 
...
    0000000000000000-ffffffffffffffff (prio 0, i/o): system
    0000000000001000-000000000000ffff (prio 0, rom): riscv.sifive.u.mrom
    0000000002000000-0000000002003fff (prio 0, i/o): riscv.aclint.swi
    0000000002004000-000000000200bfff (prio 0, i/o): riscv.aclint.mtimer
    0000000002010000-0000000002010fff (prio -1000, i/o): riscv.sifive.u.l2cc
    0000000003000000-00000000030fffff (prio 0, i/o): sifive.pdma
    0000000008000000-0000000009ffffff (prio 0, ram): riscv.sifive.u.l2lim
    000000000c000000-000000000fffffff (prio 0, i/o): riscv.sifive.plic
    0000000010000000-0000000010000fff (prio 0, i/o): riscv.sifive.u.prci
    0000000010010000-000000001001001f (prio 0, i/o): riscv.sifive.uart <=== Our UART!!
    0000000010011000-000000001001101f (prio 0, i/o): riscv.sifive.uart
...
    0000000010090000-00000000100907ff (prio 0, i/o): enet
    00000000100a0000-00000000100a0fff (prio -1000, i/o): riscv.sifive.u.gem-mgmt
    00000000100b0000-00000000100bffff (prio -1000, i/o): riscv.sifive.u.dmc
    0000000020000000-000000002fffffff (prio 0, ram): riscv.sifive.u.flash0
    0000000080000000-0000000087ffffff (prio 0, ram): riscv.sifive.u.ram
```

---

## Memory map reconnaissance: flattened device tree method

One may dump the device tree blob (dtb) using `Qemu` and convert it to source form (dts)

```bash
~/src/qemu/build/qemu-system-riscv64 -machine sifive_u,dumpdtb=sifive_u.dtb -nographic
dtc -I dtb -O dts sifive_u.dtb >> sifive_u.dts
```

---

From the device tree in source form (dts), our `UART` is easy to find

```c

    chosen {
        stdout-path = "/soc/serial@10010000";  <=== Our UART!!
    };

    aliases {
        serial0 = "/soc/serial@10010000";
        serial1 = "/soc/serial@10011000";
        ethernet0 = "/soc/ethernet@10090000";
    };

```
---

Once you get the `UART` location using either method, update the file `libfemto/std/putchar.c`. 

Set `UART` address as follows:

```c
static volatile int *uart = (int *)(void *)0x10010000;
```


