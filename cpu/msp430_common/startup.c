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

__attribute__((constructor)) static void startup(void)
{
    /* use putchar so the linker links it in: */
    putchar('\n');

    board_init();

    LOG_INFO("RIOT MSP430 hardware initialization complete.\n");

    kernel_init();
}

#include <msp430.h>
#include "periph/dma.h"

void riot_boot(void)
{
    /* disable watchdog timer */
    WDTCTL = WDTPW + WDTHOLD;
    
    extern char LINKER_data_start;
    extern char LINKER_data_size;
    extern char LINKER_data_start_rom;

    extern char LINKER_bss_start;
    extern char LINKER_bss_size;

    dma_memset((void*)((unsigned int)&LINKER_bss_start),
        0,
        ((unsigned int)&LINKER_bss_size));

    dma_memcpy((void*)((unsigned int)&LINKER_data_start),
        (void*)((unsigned int)&LINKER_data_start_rom),
        ((unsigned int)&LINKER_data_size));
    
    startup();
}
