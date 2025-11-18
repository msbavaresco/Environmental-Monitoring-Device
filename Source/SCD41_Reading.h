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
#ifndef __SCD41_READING_H__
#define __SCD41_READING_H__
//==============================================================================
typedef struct 
    {
    uint16_t    CO2;
    float       temperature;
    float       humidity;
    } SCD41_Reading_t;
//==============================================================================
int SCD41_single_shot( SCD41_Reading_t &reading );
//==============================================================================
#endif
//==============================================================================
