#include <map>
#include <vector>
#include <set>

#include "ChucKDesignerPlugin.h"
#include "ChucKDesignerShared.h"

using namespace std;

typedef set<ChucKDesignerPlugin*> ChucKDesignerPluginSetType;
typedef map<string, ChucKDesignerPlugin*> ChucKDesignerPluginMapType;

static ChucKDesignerPluginSetType global_plugin_set;
static ChucKDesignerPluginMapType global_plugin_map;
static mutex plugin_access_mutex;

typedef std::lock_guard<std::mutex> ScopedLock;

void sharedFloatCallback(const char* varName, t_CKFLOAT val) {
	myFloatVars[varName] = val;
}

float ChucKDesignerShared::getFloat(const char* varName) {
	if (myFloatVars.find(varName) != myFloatVars.end()) {
		return myFloatVars[varName];
	}
	return 0.f;
}

void ChucKDesignerShared::addChuckPluginInstance(ChucKDesignerPlugin* top)
{
	ScopedLock lock(plugin_access_mutex);
	global_plugin_set.insert(top);
	global_plugin_map[top->getNodeFullPath()] = top;
}

void ChucKDesignerShared::removeChuckPluginInstance(ChucKDesignerPlugin* top)
{
	ScopedLock lock(plugin_access_mutex);
	if (global_plugin_set.find(top) != global_plugin_set.end())
	{
		global_plugin_set.erase(top);
		if (global_plugin_map.find(top->getNodeFullPath()) != global_plugin_map.end())
			global_plugin_map.erase(top->getNodeFullPath());
	}
}

ChucKDesignerPlugin* ChucKDesignerShared::getChuckPluginInstance(const std::string& topNodeName)
{
	ScopedLock lock(plugin_access_mutex);
	if (global_plugin_map.find(topNodeName) != global_plugin_map.end()) {
		return global_plugin_map[topNodeName];
	}

	return nullptr;
}