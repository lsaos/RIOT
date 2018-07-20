/*
 * Copyright (C) 2008, 2009, 2010 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "led.h"
#include "shell.h"
#include "shell_commands.h"
#include "xtimer.h"
#include "periph/gpio.h"

int echo(int argc, char **argv)
{
    int i;
    const char* s;

    for(i = 1; i < argc; i++)
    {
        s = argv[i];
        while(*s)
        {
            putchar(*s);
            s++;
        }

        putchar(' ');
    }

    putchar('\n');
    return 0;
}

int led(int argc, char **argv)
{
    if(argc != 3) {
        puts("Use: led 1|2 on|off|toggle");
        return -1;
    }
    
    if(strcmp(argv[1], "1") == 0) {
        if(strcmp(argv[2], "on") == 0) {
            LED0_ON;
        } else if(strcmp(argv[2], "off") == 0) {
            LED0_OFF;
        } else if(strcmp(argv[2], "toggle") == 0) {
            LED0_TOGGLE;
        } else {
            puts("Use: led 1|2 on|off|toggle");
            return -1;
        }
    } else if(strcmp(argv[1], "2") == 0) {
        if(strcmp(argv[2], "on") == 0) {
            LED1_ON;
        } else if(strcmp(argv[2], "off") == 0) {
            LED1_OFF;
        } else if(strcmp(argv[2], "toggle") == 0) {
            LED1_TOGGLE;
        } else {
            puts("Use: led 1|2 on|off|toggle");
            return -1;
        }
    } else {
        puts("Use: led 1|2 on|off|toggle");
        return -1;
    }

    return 0;
}

int countdown(int argc, char **argv)
{
    if(argc != 2) {
        puts("Use: countdown seconds");
        return -1;
    }

    (void)argv;
    unsigned int seconds = atoi(argv[1]);

    while(seconds > 0)
    {
        printf("%u\r\n", seconds);
        xtimer_sleep(1);
        seconds--;
    }

    printf("0\r\n");

    return 0;
}

static const shell_command_t commands[] = {
    { "echo", "Simple echo", echo },
    { "led", "LED control", led },
    { "countdown", "Countdown in seconds", countdown },
    { NULL, NULL, NULL }
};

int main(void)
{
    (void)puts("Sytare test");

    char lineBuffer[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, lineBuffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
