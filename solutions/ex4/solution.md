# General strategy

The objective is to recover the firmware in plaintext.

To succeed, we need either:

- The key
- The address of the memory location where plaintext is stored

# Static analysis

Load `kernel.elf` in `Ghidra`, open it in code browser and analyse it.

Some suggestions:

- Define a `char[16]`  at address **0x48006a58**, we want to figure out the purpose of that data
- Binary is not stripped, try to look up function names.  We could afford to make one attempt assuming than these functions behave like those bearing the same name in known libraries.  It not then it means we need to investigate further.
- If you can't find a function's documentation by looking up the name, try looking into it.  Which arguments are being read?  Which arguments are being written to?

![h:400](../../Ressources/images/draft-static-analysis.png)

# Dynamic analysis

Time to check hypotheses from static analysis

Set breakpoint at the print("Loading system ...") function.

Inspect the memory using arguments passed to aes_decrypt_cbc as pointers

Can you find the plaintext?

Use `Qemu` monitor to dump memory

```bash
# From Qemu monitor's help on memsave command
# memsave addr size file
memsave 0x60000000 0xd23000 plaintext.bin
```

One may also use `gdb`

# Plaintext analysis

Firmware root filesystem type still to be identified

Basic tools:

- file
- binwalk

Once you know which filesystem it is, you may access it and recover the flag.