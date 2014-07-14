/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_OBJECTS_H
#define MBED_OBJECTS_H

#include "PinNames.h"
#include "PeripheralNames.h"

#ifdef __cplusplus
extern "C" {
#endif

struct analogin_s {
    ADCName adc_input;
};

struct dac_s {
    DACName dac;
};

struct pwmout_s {
    //The period of the pulse in clock cycles
    uint32_t period_cycles;
    //The width of the pulse in clock cycles
    uint32_t width_cycles;
    //Channel on TIMER2
    uint32_t channel;
};
#include "gpio_object.h"

#ifdef __cplusplus
}
#endif

#endif
