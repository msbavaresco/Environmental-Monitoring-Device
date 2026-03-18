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
#include "MQTTPublisher.h"
#include <cstdio>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <mqtt/async_client.h>
//==============================================================================
using json = nlohmann::json;
//==============================================================================
void MQTTPublisher::start(const Config& cfg)
    {
    config = cfg;
    }
//==============================================================================
void MQTTPublisher::stop()
    {
    // Does nothing for now
    }
//==============================================================================
// Format reading to be published
std::string MQTTPublisher::ReadingToJSON(const SCD4xReading& reading) const
    {
    json j;

    // Convert timestamp ISO 8601 UTC format
    char timestamp[64];
    struct tm t{};
    gmtime_r(&reading.timestamp, &t);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &t);

    j["time_and_date"] = timestamp;
    j["device_id"]     = config.device.id;
    j["location"]      = config.device.location;
    j["co2"]           = reading.CO2;
    // Round temperature and humidity to 2 decimal places
    j["temp"]          = std::round(reading.temperature * 100.0f) / 100.0f;
    j["rh"]            = std::round(reading.humidity * 100.0f) / 100.0f;

    return j.dump();
    }
//==============================================================================
void MQTTPublisher::HasNewData(SCD4xReading reading)
    {
    if (config.mqtt.host.empty())
        {
        fprintf(stderr, "MQTT host not configured, skipping publish\n");
        return;
        }

    if (config.mqtt.base_topic.empty())
        {
        fprintf(stderr, "MQTT base_topic not configured, skipping publish\n");
        return;
        }

    // Construct broker address
    const std::string serverAddress =
        "tcp://" + config.mqtt.host + ":" + std::to_string(config.mqtt.port);

    // Client identifier
    const std::string clientId =
        config.device.id + "_publisher";

    // Topic name
    const std::string topic =
        config.mqtt.base_topic + "/" + config.device.id;

    // JSON payload
    const std::string payload = ReadingToJSON(reading);

    try
        {
        // Create MQTT client
        mqtt::async_client client(serverAddress, clientId);

        mqtt::connect_options connOpts;
        connOpts.set_clean_session(true);

        // Configure authentication if provided
        if (!config.mqtt.username.empty())
            {
            connOpts.set_user_name(config.mqtt.username);

            if (!config.mqtt.password.empty())
                connOpts.set_password(config.mqtt.password);
            }

        // Connect to broker
        client.connect(connOpts)->wait();

        // Create message
        auto msg = mqtt::make_message(topic, payload);
        msg->set_qos(1);
        msg->set_retained(false);

        // Publish
        client.publish(msg)->wait();

        // Disconnect
        client.disconnect()->wait();

        printf("MQTT publish OK - %s\n", topic.c_str());
        }

    catch (const mqtt::exception& e)
        {
        fprintf(stderr, "MQTT publish failed: %s\n", e.what());
        }
    }
//==============================================================================