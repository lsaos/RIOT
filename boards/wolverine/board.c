/*
 * board.c - Board initialization for the MSP430FR5969 "Wolverine"
 * Copyright (C) 2018 INRIA
 *
 * Author : Loïc Saos <Loic.Saos@insa-lyon.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_wolverine
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Wolverine
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */


#include "cpu.h"
#include "board.h"
#include "uart_stdio.h"
#include "periph/gpio.h"

static void wolverine_ports_init(void)
{
    P2DIR |= BIT0;
    P2SEL0 |= BIT0; // Output ACLK
    P2SEL1 |= BIT0;

    P3DIR |= BIT4;
    P3SEL1 |= BIT4; // Output SMCLK

    PJSEL0 = BIT4 | BIT5; // For XT1

    // LEDs init
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);

    /*P1SEL0 &= ~BIT0;
    P1SEL1 &= ~BIT0;
    P1SELC &= ~BIT0;

    P1DIR &= ~BIT0;
    P1OUT |= BIT0;
    P1REN |= BIT0;
    P1IES |= BIT0;
    P1IFG &= ~BIT0;
    P1IE |= BIT0;*/
}

void msp430_init_dco(void)
{
    // Clock System Setup
    CSCTL0_H = CSKEY >> 8;  // Unlock CS registers

    CSCTL1 = DCOFSEL_6; // Set DCO to 8MHz
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // Set all dividers

    CSCTL4 &= ~LFXTOFF;
    do
    {
        CSCTL5 &= ~LFXTOFFG; // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1&OFIFG); // Test oscillator fault flag

    CSCTL0_H = 0; // Lock CS registers
}

/* "public" specific initialization function for the Wolverine hardware */

void board_init(void)
{
    /* init CPU core */
    msp430_cpu_init();

    /* disable watchdog timer */
    WDTCTL     =  WDTPW + WDTHOLD;

    /* init MCU pins as adequate for wolverine hardware */
    wolverine_ports_init();

    /* initializes DCO */
    msp430_init_dco();

    /* initialize STDIO over UART */
    uart_stdio_init();

    /* Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings */
    PM5CTL0 &= ~LOCKLPM5;

    /* enable interrupts */
    __bis_SR_register(GIE);
}
