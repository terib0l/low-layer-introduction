__Vectors:
    /* Main stack pointer (MSP) */
    .long __main_stack_start

    /* Reset Handler */
    /* 1. Initialize Memory Section (bss, data) */
    /* 2. Call main() */
    .long Reset_Handler
