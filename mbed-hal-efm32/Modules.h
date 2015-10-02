/***************************************************************************//**
 * @file Modules.h
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014-2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/
#ifndef MBED_MODULES_H
#define MBED_MODULES_H

#define MODULES_SIZE_ANALOGIN  YOTTA_CFG_MODULES_ANALOGIN_SIZE
#define MODULES_SIZE_ANALOGOUT YOTTA_CFG_MODULES_ANALOGOUT_SIZE
#define MODULES_SIZE_GPIO      YOTTA_CFG_MODULES_GPIO_SIZE
#define MODULES_SIZE_SPI       YOTTA_CFG_MODULES_SPI_SIZE
#define MODULES_SIZE_I2C       YOTTA_CFG_MODULES_I2C_SIZE
#define MODULES_SIZE_PWMOUT    YOTTA_CFG_MODULES_PWMOUT_SIZE
#define MODULES_SIZE_SERIAL    YOTTA_CFG_MODULES_SERIAL_SIZE
#define TRANSACTION_QUEUE_SIZE_SPI   YOTTA_CFG_MODULES_SPI_TRANSACTION_QUEUE_SIZE

#endif
