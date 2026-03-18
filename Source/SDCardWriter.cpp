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
#include "SDCardWriter.h"
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>
//==============================================================================
void SDCardWriter::start( const Config& cfg )
    {
    config = cfg;
    }
//==============================================================================
void SDCardWriter::stop()
    {
    // Does nothing for now
    }
//==============================================================================
bool SDCardWriter::FileExists(const char* path) const
    {
    return access(path, F_OK) == 0;
    }
//==============================================================================
void SDCardWriter::HasNewData(SCD4xReading reading)
    {
    FILE* dataLog;

    unsigned currentMonth;
    unsigned currentYear;
    char timestamp[64];
    char dirName[256];
    char fileName[256];
    char oldFileName[256];

    struct tm t{};
    gmtime_r(&reading.timestamp, &t);

    currentMonth = static_cast<unsigned>(t.tm_mon + 1);
    currentYear  = static_cast<unsigned>(t.tm_year + 1900);

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &t);

    // Ensure logging directories exist
    snprintf(dirName, sizeof(dirName),
             "./logs/%s",
             config.device.id.c_str());

    mkdir("./logs", 0755);
    mkdir(dirName, 0755);

    // Monthly CSV file
    snprintf(fileName, sizeof(fileName),
             "%s/%04u-%02u.csv",
             dirName,
             currentYear,
             currentMonth);

    // Open file for append
    dataLog = fopen(fileName, "a+");
    if (!dataLog)
        {
        fprintf(stderr, "Failed to open %s: %s\n", fileName, strerror(errno));
        return;
        }

    // If file is new, write CSV header
    fseek(dataLog, 0, SEEK_END);
    long size = ftell(dataLog);

    if (size == 0)
        {
        fprintf(dataLog,
                "timestamp_utc,device_id,location,co2_ppm,temperature_c,rh_percent\n");
        }

    // Write sensor reading
    fprintf(dataLog, "%s,%s,%s,%u,%.1f,%.1f\n",
            timestamp,
            config.device.id.c_str(),
            config.device.location.c_str(),
            static_cast<unsigned>(reading.CO2),
            reading.temperature,
            reading.humidity);

    fclose(dataLog);

    // Print to screen for debugging
    printf("[%s] Device: %s | Location: %s | CO2: %u ppm | Temp: %.1f C | RH: %.1f %%\n",
           timestamp,
           config.device.id.c_str(),
           config.device.location.c_str(),
           static_cast<unsigned>(reading.CO2),
           reading.temperature,
           reading.humidity);

    // Remove file older than 1 year (same month)
    snprintf(oldFileName, sizeof(oldFileName),
             "%s/%04u-%02u.csv",
             dirName,
             currentYear - 1,
             currentMonth);

    if (FileExists(oldFileName))
        {
        remove(oldFileName);
        }
    }
//==============================================================================
