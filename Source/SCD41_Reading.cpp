//==============================================================================
/*
Copyright (c) 2025, Mariana de Sene Bavaresco

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>
*/
//==============================================================================
#include "./Sensirion/sensirion_config.h"
#include "./Sensirion/sensirion_i2c.h"
#include "./Sensirion/scd4x_i2c.h"
#include "./Sensirion/sensirion_common.h"
#include "./Sensirion/sensirion_i2c_hal.h"
#include "SCD41_Reading.h"
//==============================================================================
int SCD41_single_shot( SCD41_Reading_t &reading )
    {
    int i;

    if( scd4x_wake_up() != NO_ERROR )
        return -1;

    scd4x_measure_single_shot();

    for(i = 0; i < 10 && scd4x_measure_and_read_single_shot(&reading.CO2, &reading.temperature, &reading.humidity) != NO_ERROR; i++)
        {}
    
    scd4x_power_down();

    return i < 10 ? 1 : -2;
    }
//==============================================================================
void SCD41_Init()
    {
    sensirion_i2c_hal_init();
    scd4x_init(SCD41_I2C_ADDR_62);
    }
//==============================================================================
