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
#ifndef MQTTPUBLISHER_H
#define MQTTPUBLISHER_H

#include <string>
#include <memory>
#include <mqtt/async_client.h>
#include "config.h"
#include "SCD4xReading.h"
//==============================================================================
class MQTTPublisher
{
    public:
        void start(const Config& cfg);
        void stop();
        void HasNewData(SCD4xReading reading);

    private:
        std::string ReadingToJSON(const SCD4xReading& reading) const;
        std::unique_ptr<mqtt::async_client> client;
        Config config;
};
//==============================================================================
#endif
//==============================================================================