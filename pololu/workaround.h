/* This is a workaround for Bug 13697 of GNU binutils,
   which is part of the AVR toolchain in Atmel Studio 6.0.1843.
   We force the compiler to store a variable in the .data section
   so it does not get garbage collected by the linker. */

#ifndef _POLOLU_ATMEL_STUDIO_6_WORKAROUND
#define _POLOLU_ATMEL_STUDIO_6_WORKAROUND

static void __attribute__((used)) _pololu_atmel_studio_6_workaround(void)
{
    static volatile unsigned char w = 1;
    w++;
}

#endif
