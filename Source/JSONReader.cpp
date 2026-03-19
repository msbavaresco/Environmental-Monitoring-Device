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
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include "JSONReader.h"
#include <nlohmann/json.hpp>
//==============================================================================
using json = nlohmann::json;
//==============================================================================
JSONReader::JSONReader( const std::string& path ) : configPath(path)
    {
    }
//==============================================================================
void JSONReader::ParseConfig()
    {
    // Ensure config file exists
    if (!std::filesystem::exists( configPath ))
        throw std::runtime_error("Config file missing");

    // Open config file
    std::ifstream file( configPath );

    if (!file.is_open())
        throw std::runtime_error("Cannot open config file");

    json j;
    file >> j;

    config.mqtt.host                = j["mqtt"].value("host", "");
    config.mqtt.port                = j["mqtt"].value("port", 1883);
    config.mqtt.username            = j["mqtt"].value("username", "");
    config.mqtt.password            = j["mqtt"].value("password", "");
    config.mqtt.base_topic          = j["mqtt"].value("base_topic", "env");

    config.device.location          = j["device"].value("location", "");
    config.device.id                = j["device"].value("id", "");
    config.device.interval_minutes  = j["device"].value("interval_minutes", 15);
    }
//==============================================================================
const Config& JSONReader::GetConfig() const
    {
    return config;
    }
//==============================================================================