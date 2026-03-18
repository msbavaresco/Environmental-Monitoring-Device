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
#ifndef JSONREADER_H
#define JSONREADER_H

#include <string>
#include "config.h"
//==============================================================================
class JSONReader
    {
    public:
        JSONReader(const std::string& path);
        void ParseConfig();
        const Config& GetConfig() const;

    private:
        std::string configPath;
        Config config;
    };
//==============================================================================
#endif
//==============================================================================