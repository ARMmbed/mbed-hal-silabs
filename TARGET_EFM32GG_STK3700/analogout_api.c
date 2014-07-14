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
#include "mbed_assert.h"
#include "em_dac.h"
#include "em_cmu.h"
#include "analogout_api.h"

#include "cmsis.h"
#include "pinmap.h"
#include "PeripheralPins.h"


void analogout_init(dac_t *obj, PinName pin) {
    static bool initialized = false;

    obj->dac = (DACName) pinmap_peripheral(pin, PinMap_DAC);
    MBED_ASSERT(obj->dac != (DACName)NC);


    int channel = 0;
    //There are two channels, 0 and 1, mapped to PB11 and PB12, respectively.
    switch(obj->dac){
        case DAC0_CH0:
            channel = 0;
            break;
        case DAC0_CH1:
            channel = 1;
            break;
    }


    /* Enable the DAC clock */
    CMU_ClockEnable(cmuClock_DAC0, true);

    if(!initialized){
        // Initialize the DAC. Will disable both DAC channels, so should only be done once  

        /* Use default settings */
        DAC_Init_TypeDef        init        = DAC_INIT_DEFAULT;

        /* Calculate the DAC clock prescaler value that will result in a DAC clock
         * close to 500kHz. Second parameter is zero, if the HFPERCLK value is 0, the
         * function will check what the current value actually is. */
        init.prescale = DAC_PrescaleCalc(500000, 0);

        /* Set reference voltage to VDD */
        init.reference = dacRefVDD;

        DAC_Init(DAC0, &init);
        initialized = true;
    }

    //Use default channel settings
    DAC_InitChannel_TypeDef initChannel = DAC_INITCHANNEL_DEFAULT;
    DAC_InitChannel(DAC0, &initChannel, channel);

    DAC_Enable(DAC0, channel, true);
}

void analogout_free(dac_t *obj) {/*TODO Should something be done? Diable clocks etc? PS: Make sure it's the last object*/}

static inline void dac_write(dac_t *obj, int value) {
    switch(obj->dac){
        case DAC0_CH0:
            DAC0->CH0DATA = value;
            break;
        case DAC0_CH1:
            DAC0->CH1DATA = value;
            break;
    }
}

static inline int dac_read(dac_t *obj) {
    switch(obj->dac){
        case DAC0_CH0:
            return DAC0->CH0DATA;
            break;
        case DAC0_CH1:
            return DAC0->CH1DATA;
            break;
        default:
            return -1;//TODO: Exception?
    }
}

void analogout_write(dac_t *obj, float value) {
    dac_write(obj, value*0xFFF);
}

void analogout_write_u16(dac_t *obj, uint16_t value) {
    dac_write(obj, value >> 4);
}

float analogout_read(dac_t *obj) {
    return dac_read(obj)/4095.0;
}

uint16_t analogout_read_u16(dac_t *obj) {
    return analogout_read(obj)*0xFFFF;
}

