/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       Calls startup functions on MSP430-based platforms
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include "kernel_init.h"
#include "irq.h"
#include "log.h"

extern void board_init(void);

#ifdef __MSP430_NO_LIBC_CRT0__

#ifndef MODULE_SYTARE

#include "periph/dma.h"

extern void board_init_dco(void);

extern char LINKER_data_start;
extern char LINKER_data_size;
extern char LINKER_data_start_rom;

extern char LINKER_bss_start;
extern char LINKER_bss_size;

#define LINKER_DATA         ((void*)((unsigned int)&LINKER_data_start))
#define LINKER_DATA_ROM     ((void*)((unsigned int)&LINKER_data_start_rom))
#define LINKER_DATA_SIZE    ((unsigned int)&LINKER_data_size)

#define LINKER_BSS          ((void*)((unsigned int)&LINKER_bss_start))
#define LINKER_BSS_SIZE     ((unsigned int)&LINKER_bss_size)

void riot_boot(void)
{
    board_init_dco();

    dma_memset(LINKER_BSS, 0, LINKER_BSS_SIZE);
    dma_memcpy(LINKER_DATA, LINKER_DATA_ROM, LINKER_DATA_SIZE);

    board_init();
    kernel_init();
}

#endif /* MODULE_SYTARE */

#else /* __MSP430_NO_LIBC_CRT0__ */

__attribute__((constructor)) static void startup(void)
{
    /* use putchar so the linker links it in: */
    putchar('\n');

    board_init();

    LOG_INFO("RIOT MSP430 hardware initialization complete.\n");

    kernel_init();
}

#endif /* __MSP430_NO_LIBC_CRT0__ */
