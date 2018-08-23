/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_sytare Sytare
 * @ingroup     sys
 * @brief       SYsTème embArqué faible consommation à mémoiRE persistante
 * @{
 *
 * @file
 * @brief       Common interface to Sytare
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 */

#ifndef SYTARE_H
#define SYTARE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Save all the system and shutdown
 */
void syt_save_and_shutdown(void);

/**
 * @brief   Reset the reboot system to do a complete startup next time
 */
void syt_reset_reboot(void);

#ifdef __cplusplus
}
#endif

#endif /* SYTARE_H */
/** @} */
