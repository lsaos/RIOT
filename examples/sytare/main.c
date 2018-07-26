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
#include "boostir.h"
#include "boostir_keypad.h"

#define _STACKSIZE      256
#define MSG_TYPE_ISR    0x3456
#define MSG_TYPE_KEY    0x3457

static char stack[_STACKSIZE];
static kernel_pid_t _recv_pid;

static boostir_keypad_t keypad;
static boostir_t boostir;

static void _key_pressed(uint8_t key, void* arg)
{
    (void)arg;

    msg_t msg;
    msg.type = MSG_TYPE_KEY;
    msg.content.value = key;
    msg_send(&msg, _recv_pid);
}

static void _event_cb(netdev_t* dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR)
    {
        msg_t msg;
        msg.type = MSG_TYPE_ISR;
        msg.content.ptr = dev;
        msg_send(&msg, _recv_pid);
    }
    else
    {
    }
}

void* _recv_thread(void* arg)
{
    (void)arg;

    while (1)
    {
        msg_t msg;
        msg_receive(&msg);

        if (msg.type == MSG_TYPE_ISR)
        {
            netdev_t* const dev = msg.content.ptr;
            dev->driver->isr(dev);
        }
        else if(msg.type == MSG_TYPE_KEY)
        {
            const uint8_t key = (uint8_t)msg.content.value;

            printf("Sending key '%s' down\r\n", boostir_keypad_key_name(&keypad, key));

            uint8_t data[] = { 0x55, ~0x55, key, ~key };

            const iolist_t list = {
                .iol_next = NULL,
                .iol_base = data,
                .iol_len = 4
            };

            netdev_t* const dev = (netdev_t*)&boostir;
            dev->driver->send(dev, &list);
        }
        else
        {
            puts("Unexpected message type");
        }
    }
}

static int _echo(int argc, char **argv)
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

static int _led(int argc, char **argv)
{
    if(argc != 3) {
        puts("Use: led 1|2 on|off|toggle");
        return -1;
    }
    
    if(strcmp(argv[1], "1") == 0) {
        if(strcmp(argv[2], "on") == 0) {
            LED0_ON();
        } else if(strcmp(argv[2], "off") == 0) {
            LED0_OFF();
        } else if(strcmp(argv[2], "toggle") == 0) {
            LED0_TOGGLE();
        } else {
            puts("Use: led 1|2 on|off|toggle");
            return -1;
        }
    } else if(strcmp(argv[1], "2") == 0) {
        if(strcmp(argv[2], "on") == 0) {
            LED1_ON();
        } else if(strcmp(argv[2], "off") == 0) {
            LED1_OFF();
        } else if(strcmp(argv[2], "toggle") == 0) {
            LED1_TOGGLE();
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

static int _countdown(int argc, char **argv)
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

static const shell_command_t _commands[] = {
    { "echo", "Simple echo", _echo },
    { "led", "LED control", _led },
    { "countdown", "Countdown in seconds", _countdown },
    { NULL, NULL, NULL }
};

static int _init(void)
{
    boostir_keypad_params_t keypadParams = BOOSTIR_KEYPAD_PARAMS;
    keypadParams.cb = _key_pressed;
    boostir_keypad_init(&keypad, &keypadParams);

    boostir_setup(&boostir, boostir_params);

    netdev_t* const dev = (netdev_t*)(&boostir);
    dev->event_callback = _event_cb;
    dev->driver->init(dev);

    _recv_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                              THREAD_CREATE_STACKTEST, _recv_thread, NULL,
                              "recv_thread");

    if (_recv_pid <= KERNEL_PID_UNDEF) {
        puts("Creation of receiver thread failed");
        return 1;
    }

    return 0;
}

int main(void)
{
    puts("Sytare example");

    if(_init()) {
        return 1;
    }

    puts("Initialization successful - starting the shell now");

    char lineBuffer[SHELL_DEFAULT_BUFSIZE];
    shell_run(_commands, lineBuffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
