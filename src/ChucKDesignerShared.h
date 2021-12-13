#pragma once
//
//	shared_data.h is part of YouTubeTOP.dll
//
//	Copyright 2016 Regents of the University of California
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU Lesser General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.
// 
//	Author: Peter Gusev, peter@remap.ucla.edu

#pragma once

#include <string>
#include <mutex>

#ifdef CHUCKDESIGNERSHARED_EXPORTS
#define CHUCKDESIGNERSHARED_API __declspec(dllexport)
#else
#define CHUCKDESIGNERSHARED_API __declspec(dllimport)
#endif

class ChucKDesignerPlugin;

// thread-safe class for sharing data between CHOPs and TOPs
class ChucKDesignerShared {
public:
	CHUCKDESIGNERSHARED_API static void addChuckPluginInstance(ChucKDesignerPlugin* top);
	CHUCKDESIGNERSHARED_API static void removeChuckPluginInstance(ChucKDesignerPlugin* top);
	CHUCKDESIGNERSHARED_API static ChucKDesignerPlugin* getChuckPluginInstance(const std::string& chucKNodeName);
};
