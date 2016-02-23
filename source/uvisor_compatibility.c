/***************************************************************************//**
 * @file uvisor_compatibility.c
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
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

#include "em_device.h"
#include "em_chip.h"

uint32_t g_romtable_pid[4];

void pre_uvisor_init(void) {
    /* Initialize chip; apply known errata workarounds */
    CHIP_Init();

    /* Store ROMTABLE into .bss because ROMTABLE access from unprivileged mode
     * causes bus fault */
    g_romtable_pid[0] = ROMTABLE->PID0;
    g_romtable_pid[1] = ROMTABLE->PID1;
    g_romtable_pid[2] = ROMTABLE->PID2;
    g_romtable_pid[3] = ROMTABLE->PID3;
}