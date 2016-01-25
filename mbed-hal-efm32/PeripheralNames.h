/***************************************************************************//**
 * @file PeripheralNames.h
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
#ifndef MBED_PERIPHERALNAMES_H
#define MBED_PERIPHERALNAMES_H

#include "mbed-hal-efm32/device.h"
#include "em_adc.h"
#include "em_usart.h"
#include "em_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#if DEVICE_ANALOGOUT
typedef enum {
    DAC_0 = DAC0_BASE
} DACName;
#endif

#if DEVICE_ANALOGIN
typedef enum {
    ADC_0 = ADC0_BASE
} ADCName;
#endif

#if DEVICE_I2C
typedef enum {
#if defined(I2C0_BASE)
    I2C_0 = I2C0_BASE,
#endif
#if defined(I2C1_BASE)
    I2C_1 = I2C1_BASE,
#endif
} I2CName;
#endif

#if DEVICE_PWMOUT
typedef enum {
    PWM_CH0 = 0,
    PWM_CH1 = 1,
    PWM_CH2 = 2,
#ifdef TIMER_ROUTEPEN_CC0PEN
    PWM_CH3 = 3,
#endif
} PWMName;
#endif

#if DEVICE_SERIAL
typedef enum {
#if defined(UART0_BASE)
    UART_0 = UART0_BASE,
#endif
#if defined(UART1_BASE)
    UART_1 = UART1_BASE,
#endif
#if defined(USART0_BASE)
    USART_0 = USART0_BASE,
#endif
#if defined(USART1_BASE)
    USART_1 = USART1_BASE,
#endif
#if defined(USART2_BASE)
    USART_2 = USART2_BASE,
#endif
#if defined(LEUART0_BASE)
    LEUART_0 = LEUART0_BASE,
#endif
#if defined(LEUART1_BASE)
    LEUART_1 = LEUART1_BASE,
#endif
} UARTName;
#endif

#define STDIO_UART        YOTTA_CFG_MODULES_SERIAL_STDIO_UART

#if DEVICE_SPI
typedef enum {
#if defined(USART0_BASE)
    SPI_0 = USART0_BASE,
#endif
#if defined(USART1_BASE)
    SPI_1 = USART1_BASE,
#endif
#if defined(USART2_BASE)
    SPI_2 = USART2_BASE,
#endif
} SPIName;
#endif

#ifdef __cplusplus
}
#endif

#endif
