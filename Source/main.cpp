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
#include "SCD41_Reading.h"
#include "./CppTimer/CppTimer.h"
//==============================================================================
class Timer1 : public CppTimer
    {
    void timerEvent() override
        {
        SCD41_Reading_t reading;
        int error;
        
        error = SCD41_single_shot( reading );

        if(error == -1)
            fprintf( stderr, "Failed to wake up\n");

        else if( error == -2 )
            fprintf( stderr, "Failed to measure and read\n");

        else
        fprintf(stdout,
                "CO2: %d\r\n"
                "Temperature: %f\r\n"
                "Relative Humidity: %f\r\n",
                reading.CO2,
                reading.temperature,
                reading.humidity);
        };
    };
//==============================================================================
int main()
    {
    Timer1 timer1;

    SCD41_Init();

    timer1.startms(90000);

    while (true)
        {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
//==============================================================================
