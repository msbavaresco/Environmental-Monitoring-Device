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
#include <cmath>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <string>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <ctime>
#include <stdexcept>
#include "SCD41_Reading.h"
#include <nlohmann/json.hpp>
#include <mqtt/async_client.h>

#define INTERVAL_MINUTES 20

using json = nlohmann::json;
//==============================================================================
// Structure containing the device configuration loaded from config.json
typedef struct
    {
    struct
        {
        bool configured;
        std::string ssid;
        } wifi;

    struct
        {
        std::string host;
        int port;
        std::string username;
        std::string password;
        std::string base_topic;
        } mqtt;

    struct
        {
        std::string location;
        } device;
    } Config_t;

//==============================================================================
// Helper function to check if a file exists
static bool fileExists(const char* path) {
    return access(path, F_OK) == 0;
}
//==============================================================================
// Reads configuration from JSON file and fills Config_t structure
Config_t Parse_Config( void )
    {
    // Expected location of config file
    std::filesystem::path config_file_path = "/var/lib/ThisDevice/config.json";

    // Ensure config file exists
    if (!std::filesystem::exists(config_file_path))
        throw std::runtime_error("Config file missing");

    // Open config file
    std::ifstream file( config_file_path );

    if (!file.is_open())
        throw std::runtime_error("Cannot open config file");

    // Parse config.json
    json j;
    file >> j;

    Config_t cfg;

    cfg.wifi.configured     = j["wifi"].value("configured", false);
    cfg.wifi.ssid           = j["wifi"].value("ssid", "");

    cfg.mqtt.host           = j["mqtt"].value("host", "");
    cfg.mqtt.port           = j["mqtt"].value("port", 1883);
    cfg.mqtt.username       = j["mqtt"].value("username", "");
    cfg.mqtt.password       = j["mqtt"].value("password", "");
    cfg.mqtt.base_topic     = j["mqtt"].value("base_topic", "env");

    cfg.device.location     = j["device"].value("location", "");

    return cfg;
    }
//==============================================================================
// Convert sensor reading into a JSON string for MQTT publishing
std::string Reading_to_JSON(const SCD41_Reading_t &reading,
                            const std::string &location,
                            const std::string &device_id)
    {
    json j;

    // Convert timestamp into ISO 8601 UTC format
    char time_and_date[64];
    struct tm t{};
    gmtime_r(&reading.time, &t);
    strftime(time_and_date, sizeof(time_and_date), "%Y-%m-%dT%H:%M:%SZ", &t);

    j["time_and_date"] = time_and_date;
    j["device_id"]     = device_id;
    j["location"]      = location;
    j["co2"]           = reading.CO2;

    // Round temperature and humidity to 2 decimal places
    j["temp"]          = std::round(reading.temperature * 100.0) / 100.0;
    j["rh"]            = std::round(reading.humidity * 100.0) / 100.0;

    return j.dump();
    }
//==============================================================================
// Calculates how long the program should sleep until the next reading
// aligned to the configured interval.
std::chrono::seconds Time_Until_Next_Reading(int interval_minutes)
    {
    using namespace std::chrono;

	// Current system time
    auto now = system_clock::now();
    std::time_t now_c = system_clock::to_time_t(now);

    struct tm tm_now{};
    gmtime_r(&now_c, &tm_now);

	// Determine how far we are into the interval
    int remainder = tm_now.tm_min % interval_minutes;
    int minutes_to_add = interval_minutes - remainder;

    // If exactly on boundary, wait for next interval
    if (remainder == 0 && tm_now.tm_sec == 0) 
        minutes_to_add = interval_minutes;

	// Reset seconds and add required minutes
    tm_now.tm_sec = 0;
    tm_now.tm_min += minutes_to_add;

	// Convert back to time point
    std::time_t next_time = timegm(&tm_now);
    auto next_tp = system_clock::from_time_t(next_time);

	// Return duration until next reading
    return duration_cast<seconds>(next_tp - now);
    }
//==============================================================================
// Saves sensor data to a CSV file organized by device and month
void Save_Data(const SCD41_Reading_t& reading, const char* location, const char* device_id) 
    {
    FILE* data_log;

    unsigned current_month;
    unsigned current_year;
    char time_and_date[64];

    char file_name[256];
    char old_file_name[256];
    char dir_name[256];

	// Convert timestamp to UTC
    struct tm t{};
    gmtime_r(&reading.time, &t);

    current_month = (unsigned)(t.tm_mon + 1);
    current_year  = (unsigned)(t.tm_year + 1900);

    strftime(time_and_date, sizeof(time_and_date), "%Y-%m-%dT%H:%M:%SZ", &t);

	// Ensure logging directories exist
    snprintf(dir_name, sizeof(dir_name),
        "./logs/%s", device_id);
		
    mkdir("./logs", 0755);
    mkdir(dir_name, 0755);

	// Monthly CSV file
    snprintf(file_name, sizeof(file_name),
             "%s/%04u-%02u.csv",
             dir_name,
             current_year,
             current_month);

	// Open file for append
    data_log = fopen(file_name, "a+");
    if (!data_log) 
        {
        fprintf(stderr, "Failed to open %s: %s\n", file_name, strerror(errno));
        return;
        }

	// If file is new, write CSV header
    fseek(data_log, 0, SEEK_END);
    long size = ftell(data_log);
    if (size == 0)
        fprintf(data_log, "timestamp_utc,device_id,location,co2_ppm,temperature_c,rh_percent\n");

	// Write sensor reading
    fprintf(data_log, "%s,%s,%s,%u,%.1f,%.1f\n",
            time_and_date,
            device_id,
            location,
            (unsigned)reading.CO2,
            (double)reading.temperature,
            (double)reading.humidity);

    fclose(data_log);

    // Print to screen for debugging
    printf("[%s] Device: %s | Location: %s | CO2: %u ppm | Temp: %.1f°C | RH: %.1f%%\n",
           time_and_date, device_id, location,
           (unsigned)reading.CO2,
           (double)reading.temperature,
           (double)reading.humidity);

	// Remove file older than 1 year (same month)
    snprintf(old_file_name, sizeof(old_file_name),
             "%s/%04u-%02u.csv",
             dir_name,
             current_year - 1,
             current_month);

    if (fileExists(old_file_name))
        remove(old_file_name);
    }
//==============================================================================
// Publish sensor data to an MQTT broker
bool MQTT_Publish(const SCD41_Reading_t& reading,
                  const Config_t& cfg,
                  const std::string& device_id)
    {
    // If MQTT host not configured, skip publishing
    if (cfg.mqtt.host.empty()) 
        {
        fprintf(stderr, "MQTT host not configured, skipping publish\n");
        return false;
        }

    if (cfg.mqtt.base_topic.empty()) 
        {
        fprintf(stderr, "MQTT base_topic not configured, skipping publish\n");
        return false;
        }

	// Construct broker address
    const std::string server_address =
        "tcp://" + cfg.mqtt.host + ":" + std::to_string(cfg.mqtt.port);

	// Client identifier
    const std::string client_id = device_id + "_publisher";

	// Topic name
    const std::string topic =
        cfg.mqtt.base_topic + "/" + device_id;

	// JSON payload
    const std::string payload =
        Reading_to_JSON(reading, cfg.device.location, device_id);

    try
        {
		// Create MQTT client
        mqtt::async_client client(server_address, client_id);

        mqtt::connect_options connOpts;
        connOpts.set_clean_session(true);

        // Configure authentication if provided
        if (!cfg.mqtt.username.empty()) 
            {
            connOpts.set_user_name(cfg.mqtt.username);

            if (!cfg.mqtt.password.empty())
                connOpts.set_password(cfg.mqtt.password);
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

        return true;
        }
    catch (const mqtt::exception& e)
        {
        fprintf(stderr, "MQTT publish failed: %s\n", e.what());
        return false;
        }
    }
//==============================================================================
// Main program
int main()
    {
    const std::string device_id = "raspberry1";

	// Disable stdout buffering for real-time logs
    setvbuf(stdout, nullptr, _IONBF, 0);

    Config_t cfg{};
	
	// Load configuration
    try
        {
        cfg = Parse_Config();
        }
    catch (const std::exception& e)
        {
        fprintf(stderr, "Failed to parse config: %s\n", e.what());
        return 1;
        }

	// Ensure device location is configured
    if (cfg.device.location.empty()) 
        {
        fprintf(stderr, "Device location not configured\n");
        return 1;
        }

	// Initialise SCD41 sensor
    SCD41_Init();

	// Main loop
    while (true)
        {
		// Calculate sleep duration until next scheduled reading
        std::chrono::seconds sleep_time = Time_Until_Next_Reading(INTERVAL_MINUTES);

        printf("Sleeping for %lld seconds until next reading\n",
               static_cast<long long>(sleep_time.count()));

        std::this_thread::sleep_for(sleep_time);

		// Take sensor reading
        SCD41_Reading_t reading{};
        int error = SCD41_single_shot(reading);
		
		// Store timestamp
        reading.time = std::time(nullptr);
		
		// Handle sensor errors
        if (error == -1) 
            {
            fprintf(stderr, "Failed to wake up\n");
            continue;
            }

        if (error == -2) 
            {
            fprintf(stderr, "Failed to measure and read\n");
            continue;
            }
	
		// Save reading locally
        Save_Data(reading,
                  cfg.device.location.c_str(),
                  device_id.c_str());

		// Publish reading to MQTT broker
        MQTT_Publish(reading, cfg, device_id);
        }

    return 0;
    }
//==============================================================================
