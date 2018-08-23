/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @ingroup     sys_sytare
 * @{
 *
 * @file
 * @brief       Sytare implementation
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @}
 */

#include "board.h"
#include "kernel_init.h"
#include "periph_syt.h"
#include "periph/dma.h"

__attribute__((section(".periphs_state")))
volatile char periphs_state[0
#ifdef MODULE_PERIPH_DMA
    + sizeof(dma_syt_state_t)
#endif /* MODULE_PERIPH_DMA */
#ifdef MODULE_PERIPH_GPIO
    + sizeof(gpio_syt_state_t)
#endif /* MODULE_PERIPH_GPIO */
#ifdef MODULE_PERIPH_UART
    + sizeof(uart_syt_state_t)
#endif /* MODULE_PERIPH_UART */
#ifdef MODULE_PERIPH_TIMER
    + sizeof(timer_syt_state_t)
#endif /* MODULE_PERIPH_TIMER */
];

/**
 * @brief   Save all active periphs state
 * 
 * @param[out] state    buffer to save the state
 */
static void save_periphs_state(char* state)
{
#ifdef MODULE_PERIPH_DMA
    dma_save_state((dma_syt_state_t*)state);
    state += sizeof(dma_syt_state_t);
#endif /* MODULE_PERIPH_DMA */
#ifdef MODULE_PERIPH_GPIO
    gpio_save_state((gpio_syt_state_t*)state);
    state += sizeof(gpio_syt_state_t);
#endif /* MODULE_PERIPH_GPIO */
#ifdef MODULE_PERIPH_UART
    uart_save_state((uart_syt_state_t*)state);
    state += sizeof(uart_syt_state_t);
#endif /* MODULE_PERIPH_UART */
#ifdef MODULE_PERIPH_TIMER
    timer_save_state((timer_syt_state_t*)state);
    state += sizeof(timer_syt_state_t);
#endif /* MODULE_PERIPH_TIMER */
}

/**
 * @brief   Restore all active periphs state
 * 
 * @param[in] state    buffer storing the state
 */
static void restore_periphs_state(const char* state)
{
#ifdef MODULE_PERIPH_DMA
    dma_restore_state((const dma_syt_state_t*)state);
    state += sizeof(dma_syt_state_t);
#endif /* MODULE_PERIPH_DMA */
#ifdef MODULE_PERIPH_GPIO
    gpio_restore_state((const gpio_syt_state_t*)state);
    state += sizeof(gpio_syt_state_t);
#endif /* MODULE_PERIPH_GPIO */
#ifdef MODULE_PERIPH_UART
    uart_restore_state((const uart_syt_state_t*)state);
    state += sizeof(uart_syt_state_t);
#endif /* MODULE_PERIPH_UART */
#ifdef MODULE_PERIPH_TIMER
    timer_restore_state((const timer_syt_state_t*)state);
    state += sizeof(timer_syt_state_t);
#endif /* MODULE_PERIPH_TIMER */
}

extern void board_init_dco(void);
extern void board_init(void);

extern char LINKER_data_start;
extern char LINKER_data_size;
extern char LINKER_data_start_rom;

extern char LINKER_bss_start;
extern char LINKER_bss_size;

extern char LINKER_checkpoint_data_start;
extern char LINKER_checkpoint_bss_start;
extern char LINKER_checkpoint_periphs_start;

extern char LINKER_checkpoint0_start;
extern char LINKER_checkpoint1_start;

#define LINKER_DATA         ((void*)((unsigned int)&LINKER_data_start))
#define LINKER_DATA_ROM     ((void*)((unsigned int)&LINKER_data_start_rom))
#define LINKER_DATA_SIZE    ((unsigned int)&LINKER_data_size)

#define LINKER_BSS          ((void*)((unsigned int)&LINKER_bss_start))
#define LINKER_BSS_SIZE     ((unsigned int)&LINKER_bss_size)

/**
 * @brief   Get the memory address of the specified checkpoint index
 */
#define GET_CHECKPOINT_ADDR(checkpoint) ((char*)(((checkpoint) == 0) ? ((unsigned int)&LINKER_checkpoint0_start) : ((unsigned int)&LINKER_checkpoint1_start)))

/**
 * @brief   Get the memory offset in checkpoint of the specified variable name
 */
#define GET_CHECKPOINT_VAR_OFFSET(name) (((unsigned int)&LINKER_checkpoint_##name) - ((unsigned int)&LINKER_checkpoint0_start))

/**
 * @brief   Get a pointer to the specified variable in the specified checkpoint index
 */
#define GET_CHECKPOINT_VAR(checkpoint, name) (GET_CHECKPOINT_ADDR(checkpoint) + GET_CHECKPOINT_VAR_OFFSET(name))

/**
 * @brief   True if sytare reboot is enabled
 */
static __attribute__((section(".persistent_bss")))
volatile bool syt_reboot;

/**
 * @brief   Last valid checkpoint index (0 or 1)
 */
static __attribute__((section(".persistent_bss")))
volatile uint8_t syt_checkpoint;

/**
 * @brief   Endless loop to shutdown the platform
 */
static void shutdown_platform(void)
{
    for(;;) {
        __asm__ __volatile__("nop\n\t"::);
    }
}

void syt_reset_reboot(void)
{
    // Disable sytare reboot
    syt_reboot = false;

    // Reset the current checkpoint to zero
    syt_checkpoint = 0;
}

void syt_save_and_shutdown(void)
{
    // Disable interrupts
    __disable_irq();

    // Compute new checkpoint index
    const uint8_t checkpoint = (syt_checkpoint == 0 ? 1 : 0);

    // Save periphs hardware state
    save_periphs_state(GET_CHECKPOINT_VAR(checkpoint, periphs_start));

    // Save bss to checkpoint
    dma_memcpy(GET_CHECKPOINT_VAR(checkpoint, bss_start), LINKER_BSS, LINKER_BSS_SIZE);

    // Save data to checkpoint
    dma_memcpy(GET_CHECKPOINT_VAR(checkpoint, data_start), LINKER_DATA, LINKER_DATA_SIZE);

    // Set current checkpoint index
    syt_checkpoint = checkpoint;

    // Enable sytare reboot
    syt_reboot = true;

    // Notify user with all LEDs on
    LED0_ON();
    LED1_ON();

    // Wait for an hardware reboot
    shutdown_platform();
}

void riot_boot(void)
{
    // Always reset watchdog and init DCO first
    board_init_dco();

    // Check wether first boot or reboot
    if(syt_reboot)
    {
        const uint8_t checkpoint = syt_checkpoint;

        // Restore data from checkpoint
        dma_memcpy(LINKER_DATA, GET_CHECKPOINT_VAR(checkpoint, data_start), LINKER_DATA_SIZE);

        // Restore bss from checkpoint
        dma_memcpy(LINKER_BSS, GET_CHECKPOINT_VAR(checkpoint, bss_start), LINKER_BSS_SIZE);

        // Restore periphs hardware state from checkpoint
        restore_periphs_state(GET_CHECKPOINT_VAR(checkpoint, periphs_start));

        // Enable interrupts
        __enable_irq();

        // Restore all registers, including SP and PC
        __exit_isr();
    }
    else
    {
        // It is the first boot, we do a complete startup

        // Fill bss with zeroes
        dma_memset(LINKER_BSS, 0, LINKER_BSS_SIZE);

        // Fill data from ROM
        dma_memcpy(LINKER_DATA, LINKER_DATA_ROM, LINKER_DATA_SIZE);

        // Initialize the board
        board_init();

        // Initialize RIOT kernel
        // It will run the main too
        kernel_init();
    }

    // If we return from the main, execution is over
    syt_reset_reboot();
    shutdown_platform();
}
