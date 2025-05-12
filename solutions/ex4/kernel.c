#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "sha256.h"
#include "aes.h"
#include "binascii.h"

volatile uint8_t *uart = (uint8_t *) 0x10009000;
const BYTE *mmiobase = (BYTE *)0x10004000;

void putchar(char c) {
    *uart = c;
}

void print(const char *s) {
    while(*s != '\0') {
        putchar(*s);
        s++;
    }
}

void kmain(void) {

    //BYTE hash3[SHA1_BLOCK_SIZE] = {0x34,0xaa,0x97,0x3c,0xd4,0xc4,0xda,0xa4,0xf6,0x1e,0xeb,0x2b,0xdb,0xad,0x27,0x31,0x65,0x34,0x01,0x6f};
    
    BYTE buf[SHA256_BLOCK_SIZE];
    BYTE outbuf[SHA256_BLOCK_SIZE*2+1];
    int idx;
    SHA256_CTX ctx;
    BYTE text2[] = "Hello world!\n";
    int pass = 1;

    WORD key_schedule[60];                                                                                                                      
    BYTE *flash = (BYTE *)0x40000000;
    BYTE *highmem = (BYTE *)0x60000000;
    BYTE iv[16] = "NSECWorkshop2025";
    size_t filesystem_size = 13774848;

    print("Workshop 2025 Really Secure Boot Enabled Firmware!!\n");

    print("Checking machine integrity\n ");
    sha256_init(&ctx);
    sha256_update(&ctx, mmiobase, 0x4000);
    sha256_final(&ctx, buf);
    // I wish i had a real printf 
    //print("Binascii this: ");
    hexlify(buf, SHA256_BLOCK_SIZE, outbuf, SHA256_BLOCK_SIZE * 2 + 1);
    //print(outbuf);
    //print("\n");
    //pass = pass && !memcmp(hash2, buf, SHA1_BLOCK_SIZE);

    aes_key_setup(buf, key_schedule, 256);


    //pass = pass && !memcmp(enc_buf, ciphertext[0], 32);

    print("Reading flash...\n ");
    aes_decrypt_cbc(flash, filesystem_size, highmem, key_schedule, 256, iv);

    print("Loading system ...\n ");
    print("License check failed...\n ");
    print("Shutting down...\n ");
    memset(highmem, 0, filesystem_size);

    // try to shut machine down, FIXME does not work on vexpress, maybe find another board
    __asm__ (   "ldr r0, =0x84000008\n\t" //  ; Load PSCI_SYSTEM_OFF function ID into r0
                "smc #0" ); //               ; Secure Monitor Call to invoke PSCI

    
}
