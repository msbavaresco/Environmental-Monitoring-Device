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
#include "scd4x_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "SCD41_Reading.h"
//==============================================================================
int SCD41_single_shot( SCD41_Reading_t &reading )
    {
    if( scd4x_wake_up() != NO_ERROR )
        return -1;

    sensirion_sleep_msec( 30 );

    if( scd4x_measure_and_read_single_shot(&reading.CO2, &reading.temperature, &reading.humidity) != NO_ERROR )
        return -1;

    scd4x_power_down();

    return 1;
    }
//==============================================================================
