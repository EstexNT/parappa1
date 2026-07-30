// TODO: Unhardcode this address. It should point to main_VRAM_END
__asm__(
".include \"macro.inc\"\n"
"\n"
".section .rodata, \"a\"\n"
"\n"
"dlabel D_80010000\n"
"    /* 800 80010000 1CC21C80 */ .word 0x801CC21C\n"
".size D_80010000, . - D_80010000\n"
);
