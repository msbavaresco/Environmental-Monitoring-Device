//==============================================================================
/*
Copyright (c) 2026, Mariana de Sene Bavaresco

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
#include <ctime>
#include "SCD4x.h"
#include "./Sensirion/sensirion_config.h"
#include "./Sensirion/sensirion_i2c.h"
#include "./Sensirion/scd4x_i2c.h"
#include "./Sensirion/sensirion_common.h"
#include "./Sensirion/sensirion_i2c_hal.h"
//==============================================================================
void SCD4x::start(int intervalMinutes)
    {
    sensirion_i2c_hal_init();
    scd4x_init(SCD41_I2C_ADDR_62);

    startms(intervalMinutes * 60 * 1000);
    }
//==============================================================================
void SCD4x::stop()
    {
    CppTimer::stop();
    }
//==============================================================================
void SCD4x::registerCallback( std::function<void(SCD4xReading)> cb )
    {
    readingCallback = cb;
    }
//==============================================================================
void SCD4x::timerEvent()
{
    if (!readingCallback)
        return;

    SCD4xReading reading{};
    int error = SingleShot(reading);

    if (error == -1)
        {
        fprintf(stderr, "SCD4x: failed to wake up\n");
        return;
        }

    if (error == -2)
        {
        fprintf(stderr, "SCD4x: failed to measure and read\n");
        return;
        }

    readingCallback(reading);
}
//==============================================================================
int SCD4x::SingleShot(SCD4xReading& reading)
    {
    int i;

    if (scd4x_wake_up() != NO_ERROR)
        return -1;

    scd4x_measure_single_shot();

    for (i = 0;
         i < 10 &&
         scd4x_measure_and_read_single_shot(&reading.CO2,
                                            &reading.temperature,
                                            &reading.humidity) != NO_ERROR;
         i++)
        {
        }

    scd4x_power_down();

    if (i < 10)
        {
        reading.timestamp = std::time(nullptr);
        return 1;
        }

    return -2;
    }
//==============================================================================