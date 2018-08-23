/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_msp430_common
 * @{
 *
 * @file
 * @brief       stdio.h wrapper for MSP430
 * @see http://www.cplusplus.com/reference/cstdio/
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se
 */

#ifndef MSP430_STDIO_H
#define MSP430_STDIO_H

#include_next <stdio.h>

/*
 * Newlib already provides getchar in stdio.h.
 */
#ifndef MODULE_NEWLIB_SYSCALLS_DEFAULT

/*
 * The MSP430 toolchain does not provide getchar in stdio.h.
 * As a workaround, we include the system stdio.h here and then add getchar.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

int getchar(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULE_NEWLIB_SYSCALLS_DEFAULT */

#endif /* MSP430_STDIO_H */
/** @} */
