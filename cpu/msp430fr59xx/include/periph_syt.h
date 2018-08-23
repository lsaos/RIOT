/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_msp430fr5969
 * @{
 *
 * @file
 * @brief           CPU specific definitions for Sytare
 *
 * @author          Loïc Saos <Loic.Saos@insa-lyon.fr>
 */

#ifndef PERIPH_SYT_H
#define PERIPH_SYT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Structure storing GPIO periph hardware state
 */
typedef struct {
    uint8_t OD[5];
    uint8_t DIR[5];
    uint8_t REN[5];
    uint8_t SEL0[5];
    uint8_t SEL1[5];
    uint8_t SELC[5];
    uint8_t IES[4];
    uint8_t IE[4];
} gpio_syt_state_t;

/**
 * @brief   Save the hardware state of the GPIO periph
 *
 * @param[out] state    structure to save the state
 */
void gpio_save_state(gpio_syt_state_t* state);

/**
 * @brief   Restore the hardware state of the GPIO periph
 *
 * @param[in] state    structure storing the state
 */
void gpio_restore_state(const gpio_syt_state_t* state);

/**
 * @brief   Structure storing UART periph hardware state
 */
typedef struct {
    uint8_t BR0;
    uint8_t BR1;
    uint16_t MCTLW;
} uart_syt_state_t;

/**
 * @brief   Save the hardware state of the UART periph
 *
 * @param[out] state    structure to save the state
 */
void uart_save_state(uart_syt_state_t* state);

/**
 * @brief   Restore the hardware state of the UART periph
 *
 * @param[in] state    structure storing the state
 */
void uart_restore_state(const uart_syt_state_t* state);

/**
 * @brief   Structure storing timer periph hardware state
 */
typedef struct {
    uint16_t CTL;
    uint16_t R;
    uint16_t CCTL[3];
    uint16_t CCR[3];
} timer_syt_state_t;

/**
 * @brief   Save the hardware state of the timer periph
 *
 * @param[out] state    structure to save the state
 */
void timer_save_state(timer_syt_state_t* state);

/**
 * @brief   Restore the hardware state of the timer periph
 *
 * @param[in] state    structure storing the state
 */
void timer_restore_state(const timer_syt_state_t* state);

/**
 * @brief   Structure storing DMA periph hardware state
 */
typedef struct {
    uint32_t SA;
    uint32_t DA;
    uint16_t SZ;
} dma_syt_state_t;

/**
 * @brief   Save the hardware state of the DMA periph
 *
 * @param[out] state    structure to save the state
 */
void dma_save_state(dma_syt_state_t* state);

/**
 * @brief   Restore the hardware state of the DMA periph
 *
 * @param[in] state    structure storing the state
 */
void dma_restore_state(const dma_syt_state_t* state);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_SYT_H */
/** @} */
