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
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <exception>
#include <unistd.h>
#include <sys/signalfd.h>

#include "JSONReader.h"
#include "SCD4x.h"
#include "DataCleaner.h"
#include "SDCardWriter.h"
#include "MQTTPublisher.h"
//==============================================================================
int main()
    {
    // Create all system modules
    JSONReader jsonreader("/var/lib/ThisDevice/config.json");
    SCD4x scd4x;
    DataCleaner datacleaner;
    SDCardWriter sdcardwriter;
    MQTTPublisher mqttpublisher;

    // Load configuration from JSON file
    try
        {
        jsonreader.ParseConfig();
        }
    catch (const std::exception& e)
        {
        fprintf(stderr, "Failed to parse config: %s\n", e.what());
        return 1;
        }

    // Retrieve parsed configuration
    const Config& cfg = jsonreader.GetConfig();

    // Connect SCD4x sensor output to DataCleaner
    scd4x.registerCallback([&](SCD4xReading reading)
        {
        datacleaner.HasNewData(reading);
        });

    // Connect DataCleaner output to SDCardWriter and MQTTPublisher
    datacleaner.registerCallback([&](SCD4xReading reading)
        {
        sdcardwriter.HasNewData(reading);
        mqttpublisher.HasNewData(reading);
        });

    // Start output modules
    sdcardwriter.start(cfg);
    mqttpublisher.start(cfg);

    // Start sensor with configured interval (minutes)
    scd4x.start(cfg.device.interval_minutes);

    // Prepare signal set to handle shutdown via signalfd
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);   // Ctrl+C
    sigaddset(&mask, SIGTERM);  // Service stop
    sigaddset(&mask, SIGHUP);   // Reload/terminate signal

    // Block signals so they are handled via signalfd instead of default handlers
    if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1)
        {
        perror("sigprocmask");
        scd4x.stop();
        mqttpublisher.stop();
        sdcardwriter.stop();
        return 1;
        }

    // Create file descriptor to receive signals
    int sfd = signalfd(-1, &mask, 0);
    if (sfd == -1)
        {
        perror("signalfd");
        scd4x.stop();
        mqttpublisher.stop();
        sdcardwriter.stop();
        return 1;
        }

    // Main loop: blocks here until a signal is received
    bool isRunning = true;

    while (isRunning)
        {
        struct signalfd_siginfo fdsi;

        // Sleeps until a signal arrives
        ssize_t s = read(sfd, &fdsi, sizeof(fdsi));

        if (s != sizeof(fdsi))
            {
            perror("read");
            break;
            }

        // Handle received signals
        if (fdsi.ssi_signo == SIGINT)
            {
            printf("Got SIGINT, shutting down\n");
            isRunning = false;
            }
        else if (fdsi.ssi_signo == SIGTERM)
            {
            printf("Got SIGTERM, shutting down\n");
            isRunning = false;
            }
        else if (fdsi.ssi_signo == SIGHUP)
            {
            printf("Got SIGHUP, shutting down\n");
            isRunning = false;
            }
        else
            {
            printf("Unexpected signal received\n");
            }
        }

    // Close signal file descriptor
    close(sfd);

    // Stop modules
    scd4x.stop();
    mqttpublisher.stop();
    sdcardwriter.stop();

    return 0;
    }
//==============================================================================