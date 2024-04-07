/* Shared Use License: This file is owned by Derivative Inc. (Derivative)
* and can only be used, and/or modified for use, in conjunction with
* Derivative's TouchDesigner software, and only if you are a licensee who has
* accepted Derivative's TouchDesigner license or assignment agreement
* (which also govern the use of this file). You may share or redistribute
* a modified version of this file provided the following conditions are met:
*
* 1. The shared file or redistribution must retain the information set out
* above and this list of conditions.
* 2. Derivative's name (Derivative Inc.) or its trademarks may not be used
* to endorse or promote products derived from this file without specific
* prior written permission from Derivative.
*/

#include "ChucKListenerCHOP.h"
#include "Plugin_ChucK.h"

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <assert.h>

//#include <vector>

#include <Python.h>
#include <structmember.h>

const char* PythonCallbacksDATStubs =
"# This is an example callbacks DAT for a ChucK Listener Operator.\n"
"# In all callback methods, \"listener\" is the ChucK Listener operator.\n"
"\n\n"
"def getFloat(listener, name, val):\n"
"    # print(f'getFloat(name=\"{name}\", val={val})')\n"
"    pass\n"
"\n\n"
"def getInt(listener, name, val):\n"
"    # print(f'getInt(name=\"{name}\", val={val})')\n"
"    pass\n"
"\n\n"
"def getString(listener, name, val):\n"
"    # print(f'getString(name=\"{name}\", val={val})')\n"
"    pass\n"
"\n\n"
"def getEvent(listener, name):\n"
"    # print(f'getEvent(name=\"{name}\")')\n"
"    pass\n"
"\n\n"
"def getFloatArray(listener, name, vals):\n"
"    # print(f'getFloatArray(name=\"{name}\", vals={vals})')\n"
"    pass\n"
"\n\n"
"def getIntArray(listener, name, vals):\n"
"    # print(f'getIntArray(name=\"{name}\", vals={vals})')\n"
"    pass\n"
;

// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C"
{

DLLEXPORT
void
FillCHOPPluginInfo(CHOP_PluginInfo *info)
{
	// Always set this to CHOPCPlusPlusAPIVersion.
	info->apiVersion = CHOPCPlusPlusAPIVersion;

	// The opType is the unique name for this CHOP. It must start with a 
	// capital A-Z character, and all the following characters must lower case
	// or numbers (a-z, 0-9)
	info->customOPInfo.opType->setString("Chucklistener");

	// The opLabel is the text that will show up in the OP Create Dialog
	info->customOPInfo.opLabel->setString("ChucK Listener");
	info->customOPInfo.opIcon->setString("CKL");

	// Information about the author of this OP
	info->customOPInfo.authorName->setString("David Braun");
	info->customOPInfo.authorEmail->setString("github.com/DBraun");

	info->customOPInfo.minInputs = 0;
	info->customOPInfo.maxInputs = 0;
    
    info->customOPInfo.pythonVersion->setString(PY_VERSION);
    //info->customOPInfo.pythonMethods = methods;
    //info->customOPInfo.pythonGetSets = getSets;
    info->customOPInfo.pythonCallbacksDAT = PythonCallbacksDATStubs;
}

DLLEXPORT
CHOP_CPlusPlusBase*
CreateCHOPInstance(const OP_NodeInfo* info)
{
	// Return a new instance of your class every time this is called.
	// It will be called once per CHOP that is using the .dll
	return new ChucKListenerCHOP(info);
}

DLLEXPORT
void
DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
{
	// Delete the instance here, this will be called when
	// Touch is shutting down, when the CHOP using that instance is deleted, or
	// if the CHOP loads a different DLL
	delete (ChucKListenerCHOP*)instance;
}

};

vector<string> split(const string& s, char delim) {
	vector<string> result;
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim)) {
		if (item.compare("") != 0) {
			result.push_back(item);
		}
	}

	return result;
}


ChucKListenerCHOP::ChucKListenerCHOP(const OP_NodeInfo* info) : myNodeInfo(info)
{
	myExecuteCount = 0;
}

ChucKListenerCHOP::~ChucKListenerCHOP()
{
	for (auto varName : myEventVarNames) {
		ChucK_For_TouchDesigner::removeListenerCHOP(varName.c_str(), this->myNodeInfo->opId);
	}
}

void
ChucKListenerCHOP::getGeneralInfo(CHOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = true;

	// Note: To disable timeslicing you'll need to turn this off, as well as ensure that
	// getOutputInfo() returns true, and likely also set the info->numSamples to how many
	// samples you want to generate for this CHOP. Otherwise it'll take on length of the
	// input CHOP, which may be timesliced.
	ginfo->timeslice = false;

	ginfo->inputMatchIndex = 0;
}

bool
ChucKListenerCHOP::getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs* inputs, void* reserved1)
{
	std::string Floatvars = inputs->getParString("Floatvars");
	std::string Intvars = inputs->getParString("Intvars");
	std::string Stringvars = inputs->getParString("Stringvars");
    std::string Floatarrayvars = inputs->getParString("Floatarrayvars");
	std::string Intarrayvars = inputs->getParString("Intarrayvars");

	auto Floatvarstrings = split(Floatvars, ' ');
	auto Intvarstrings = split(Intvars, ' ');
	auto Stringvarstrings = split(Stringvars, ' ');
    auto Floatarrayvarstrings = split(Floatarrayvars, ' ');
	auto Intarrayvarstrings = split(Intarrayvars, ' ');

	myFloatVarNames.clear();
	myIntVarNames.clear();
	myStringVarNames.clear();
    myFloatArrayVarNames.clear();
	myIntArrayVarNames.clear();

	//std::vector<std::string> myAssociativeFloatArrayVarNames;
	//std::vector<std::string> myAssociativeIntArrayVarNames;
    
	for (auto& str : Floatvarstrings) {
		myFloatVarNames.push_back(str);
	}

	for (auto& str : Intvarstrings) {
		myIntVarNames.push_back(str);
	}

	for (auto& str : Stringvarstrings) {
		myStringVarNames.push_back(str);
	}

	for (auto& str : Floatarrayvarstrings) {
		myFloatArrayVarNames.push_back(str);
	}

	for (auto& str : Intarrayvarstrings) {
		myIntArrayVarNames.push_back(str);
	}

	//info->sampleRate = 44100.;
	info->numSamples = 1;
    info->startIndex = 0;
	info->numChannels = myFloatVarNames.size() + myIntVarNames.size();
	
	return true;
}

void
ChucKListenerCHOP::getChannelName(int32_t index, OP_String *name, const OP_Inputs* inputs, void* reserved1)
{

	if (index < myFloatVarNames.size()) {
		name->setString(myFloatVarNames.at(index).c_str());
		return;
	}

	index -= myFloatVarNames.size();
	if (index < myIntVarNames.size()) {
		name->setString(myIntVarNames.at(index).c_str());
		return;
	}

	name->setString("chan1");
}


void
ChucKListenerCHOP::execute(CHOP_Output* output,
							  const OP_Inputs* inputs,
							  void* reserved)
{
    myStatus = true;
    myError.str("");

    const OP_CHOPInput* chuckDesignerCHOP = inputs->getParCHOP("Chuck");

	if (!chuckDesignerCHOP) {
		myStatus = false;
		myError << "Please select a ChucK instance.";
		return;
	}

	int chuck_id = ChucK_For_TouchDesigner::getChucKIDForOpID(chuckDesignerCHOP->opId);

	if (chuck_id != m_chuckID) {
		// stop all listeners
        for (auto varName : myEventVarNames) {
            ChucK_For_TouchDesigner::removeListenerCHOP(varName.c_str(), this->myNodeInfo->opId);
        }
		myEventVarNames.clear();

		m_chuckID = chuck_id;
	}

	if (chuck_id < 0) {
		return;
	}

	for (const std::string varName : myFloatVarNames) {
		ChucK_For_TouchDesigner::getNamedChuckFloat(chuck_id, varName.c_str(), ChucK_For_TouchDesigner::sharedFloatCallback);
	}
	for (const std::string varName : myIntVarNames) {
		ChucK_For_TouchDesigner::getNamedChuckInt(chuck_id, varName.c_str(), ChucK_For_TouchDesigner::sharedIntCallback);
	}
	for (const std::string varName : myStringVarNames) {
		ChucK_For_TouchDesigner::getNamedChuckString(chuck_id, varName.c_str(), ChucK_For_TouchDesigner::sharedStringCallback);
	}
    for (const std::string varName : myFloatArrayVarNames) {
        ChucK_For_TouchDesigner::getNamedGlobalFloatArray(chuck_id, varName.c_str(), ChucK_For_TouchDesigner::sharedFloatArrayCallback);
    }
	for (const std::string varName : myIntArrayVarNames) {
		ChucK_For_TouchDesigner::getNamedGlobalIntArray(chuck_id, varName.c_str(), ChucK_For_TouchDesigner::sharedIntArrayCallback);
	}

	// << event stuff
	std::string Eventvars = inputs->getParString("Eventvars");

	auto Eventvarstrings = split(Eventvars, ' ');

	std::set<std::string> newEventStrings, eventsToStartListening, eventsToStopListening;

	for (auto& str : Eventvarstrings) {
		newEventStrings.insert(str);
	}

	set_difference(newEventStrings.begin(), newEventStrings.end(), myEventVarNames.begin(), myEventVarNames.end(), inserter(eventsToStartListening, eventsToStartListening.end()));
	set_difference(myEventVarNames.begin(), myEventVarNames.end(), newEventStrings.begin(), newEventStrings.end(), inserter(eventsToStopListening, eventsToStopListening.end()));

	for (const std::string varName : eventsToStopListening) {
		bool result = ChucK_For_TouchDesigner::stopListeningForNamedChuckEvent(chuck_id, varName.c_str(), ChucK_For_TouchDesigner::sharedEventNonCallback);
		if (result) {
			ChucK_For_TouchDesigner::removeListenerCHOP(varName.c_str(), this->myNodeInfo->opId);
		}
	}

	for (const std::string varName : eventsToStartListening) {
		bool result = ChucK_For_TouchDesigner::startListeningForNamedChuckEvent(chuck_id, varName.c_str(), ChucK_For_TouchDesigner::sharedEventCallback);
		if (result) {
			ChucK_For_TouchDesigner::addListenerCHOP(varName.c_str(), this->myNodeInfo->opId);

			myEventVarNames.insert(varName);
		}
	}
	// end event stuff >>

	int i = 0;
	for (const std::string varName : myFloatVarNames)
	{
        auto name = varName.c_str();
        t_CKFLOAT val = ChucK_For_TouchDesigner::getFloat(name);
        if (i < output->numChannels) {
            output->channels[i][0] = val;
        }
        
		i += 1;
        
        // We'll only be adding one extra argument
        PyObject* args = myNodeInfo->context->createArgumentsTuple(2, nullptr);
        // The first argument is already set to the 'op' variable, so we set the second argument to our speed value
        PyTuple_SET_ITEM(args, 1, PyUnicode_FromString(name));
        PyTuple_SET_ITEM(args, 2, PyFloat_FromDouble(val));
		

        PyObject *result = myNodeInfo->context->callPythonCallback("getFloat", args, nullptr, nullptr);
        // callPythonCallback doesn't take ownership of the argts
        Py_DECREF(args);

        // We own result now, so we need to Py_DECREF it unless we want to hold onto it
        if (result) { Py_DECREF(result); }
	}

	for (const std::string varName : myIntVarNames)
	{
		auto name = varName.c_str();
		t_CKINT val = ChucK_For_TouchDesigner::getInt(name);
		if (i < output->numChannels) {
			output->channels[i][0] = val;
		}

		i += 1;

		// We'll only be adding one extra argument
		PyObject* args = myNodeInfo->context->createArgumentsTuple(2, nullptr);
		// The first argument is already set to the 'op' variable, so we set the second argument to our speed value
		PyTuple_SET_ITEM(args, 1, PyUnicode_FromString(name));
		PyTuple_SET_ITEM(args, 2, PyLong_FromLongLong(val));

		PyObject* result = myNodeInfo->context->callPythonCallback("getInt", args, nullptr, nullptr);
		// callPythonCallback doesn't take ownership of the argts
		Py_DECREF(args);

		// We own result now, so we need to Py_DECREF it unless we want to hold onto it
		if (result) { Py_DECREF(result); }
	}

	for (const std::string varName : myStringVarNames)
	{
		auto name = varName.c_str();
		auto str = ChucK_For_TouchDesigner::getString(name);

		// We'll only be adding one extra argument
		PyObject* args = myNodeInfo->context->createArgumentsTuple(2, nullptr);
		// The first argument is already set to the 'op' variable, so we set the second argument to our speed value
		PyTuple_SET_ITEM(args, 1, PyUnicode_FromString(name));
		PyTuple_SET_ITEM(args, 2, PyUnicode_FromString(str));

		PyObject* result = myNodeInfo->context->callPythonCallback("getString", args, nullptr, nullptr);
		// callPythonCallback doesn't take ownership of the argts
		Py_DECREF(args);

		// We own result now, so we need to Py_DECREF it unless we want to hold onto it
		if (result) { Py_DECREF(result); }
	}
    
    for (const std::string varName : myFloatArrayVarNames)
    {
        auto name = varName.c_str();
		int numItems = 0;
        auto vec = ChucK_For_TouchDesigner::getFloatArray(name, numItems);
        
        if (!numItems || !vec) {
            continue;
        }

        // We'll only be adding one extra argument
        PyObject* args = myNodeInfo->context->createArgumentsTuple(2, nullptr);
        // The first argument is already set to the 'op' variable, so we set the second argument to our speed value
        PyTuple_SET_ITEM(args, 1, PyUnicode_FromString(name));

        // todo: return a numpy array
        PyObject *lst = PyList_New(numItems);
        for (i = 0; i < numItems; i++) {
            PyList_SET_ITEM(lst, i, PyFloat_FromDouble(*(vec++)));
        }

        PyTuple_SET_ITEM(args, 2, lst);

        PyObject *result = myNodeInfo->context->callPythonCallback("getFloatArray", args, nullptr, nullptr);
        // callPythonCallback doesn't take ownership of the argts
        Py_DECREF(args);
        //Py_DECREF(lst);  // todo?

        // We own result now, so we need to Py_DECREF it unless we want to hold onto it
        if (result) { Py_DECREF(result); }
    }

	for (const std::string varName : myIntArrayVarNames)
	{
		auto name = varName.c_str();
		int numItems = 0;
		auto vec = ChucK_For_TouchDesigner::getIntArray(name, numItems);

        if (!numItems || !vec) {
            continue;
        }
        
		// We'll only be adding one extra argument
		PyObject* args = myNodeInfo->context->createArgumentsTuple(2, nullptr);
		// The first argument is already set to the 'op' variable, so we set the second argument to our speed value
		PyTuple_SET_ITEM(args, 1, PyUnicode_FromString(name));

		// todo: return a numpy array
		PyObject *lst = PyList_New(numItems);
		for (i = 0; i < numItems; i++) {
			PyList_SET_ITEM(lst, i, PyLong_FromLongLong(*(vec++)));
		}

		PyTuple_SET_ITEM(args, 2, lst);

		PyObject* result = myNodeInfo->context->callPythonCallback("getIntArray", args, nullptr, nullptr);
		// callPythonCallback doesn't take ownership of the argts
		Py_DECREF(args);
        //Py_DECREF(lst);  // todo?

		// We own result now, so we need to Py_DECREF it unless we want to hold onto it
		if (result) { Py_DECREF(result); }
	}

	for (const std::string varName : myEventVarNames)
	{
		auto name = varName.c_str();
		int count = ChucK_For_TouchDesigner::queryEvent(name, this->myNodeInfo->opId);
		for (int i = 0; i < count; i++ ) {
			// We'll only be adding one extra argument
			PyObject* args = myNodeInfo->context->createArgumentsTuple(1, nullptr);
			// The first argument is already set to the 'op' variable, so we set the second argument to our speed value
			PyTuple_SET_ITEM(args, 1, PyUnicode_FromString(name));

			PyObject* result = myNodeInfo->context->callPythonCallback("getEvent", args, nullptr, nullptr);
			// callPythonCallback doesn't take ownership of the argts
			Py_DECREF(args);

			// We own result now, so we need to Py_DECREF it unless we want to hold onto it
			if (result) { Py_DECREF(result); }
		}

	}
    
}

void
ChucKListenerCHOP::getErrorString(OP_String* error, void* reserved1) {

    if (!myStatus) {
        error->setString(myError.str().c_str());
    }
}

int32_t
ChucKListenerCHOP::getNumInfoCHOPChans(void * reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 1;
}

void
ChucKListenerCHOP::getInfoCHOPChan(int32_t index,
										OP_InfoCHOPChan* chan,
										void* reserved1)
{
	// This function will be called once for each channel we said we'd want to return
	// In this example it'll only be called once.

	if (index == 0)
	{
		chan->name->setString("executeCount");
		chan->value = (float)myExecuteCount;
	}

	// todo: show number of shreds here
}

bool		
ChucKListenerCHOP::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 1;
	infoSize->cols = 2;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void
ChucKListenerCHOP::getInfoDATEntries(int32_t index,
										int32_t nEntries,
										OP_InfoDATEntries* entries, 
										void* reserved1)
{
	char tempBuffer[4096];

	if (index == 0)
	{
		// Set the value for the first column
		entries->values[0]->setString("executeCount");

		// Set the value for the second column
#ifdef _WIN32
		sprintf_s(tempBuffer, "%d", myExecuteCount);
#else // macOS
        snprintf(tempBuffer, sizeof(tempBuffer), "%d", myExecuteCount);
#endif
		entries->values[1]->setString(tempBuffer);
	}
}

void
ChucKListenerCHOP::setupParameters(OP_ParameterManager* manager, void *reserved1)
{
    // main instance
    {
        OP_StringParameter	sp;

        sp.name = "Chuck";
        sp.label = "Chuck Instance";

        sp.defaultValue = "";

        OP_ParAppendResult res = manager->appendCHOP(sp);
        assert(res == OP_ParAppendResult::Success);
    }

    {
        OP_StringParameter	sp;

        sp.name = "Floatvars";
        sp.label = "Float Variables";

        sp.defaultValue = "";

        OP_ParAppendResult res = manager->appendString(sp);
        assert(res == OP_ParAppendResult::Success);
    }

	{
		OP_StringParameter	sp;

		sp.name = "Intvars";
		sp.label = "Int Variables";

		sp.defaultValue = "";

		OP_ParAppendResult res = manager->appendString(sp);
		assert(res == OP_ParAppendResult::Success);
	}

	{
		OP_StringParameter	sp;

		sp.name = "Stringvars";
		sp.label = "String Variables";

		sp.defaultValue = "";

		OP_ParAppendResult res = manager->appendString(sp);
		assert(res == OP_ParAppendResult::Success);
	}
    
    {
        OP_StringParameter    sp;

        sp.name = "Floatarrayvars";
        sp.label = "Float Array Variables";

        sp.defaultValue = "";

        OP_ParAppendResult res = manager->appendString(sp);
        assert(res == OP_ParAppendResult::Success);
    }

	{
		OP_StringParameter    sp;

		sp.name = "Intarrayvars";
		sp.label = "Int Array Variables";

		sp.defaultValue = "";

		OP_ParAppendResult res = manager->appendString(sp);
		assert(res == OP_ParAppendResult::Success);
	}

	{
		OP_StringParameter    sp;

		sp.name = "Eventvars";
		sp.label = "Event Variables";

		sp.defaultValue = "";

		OP_ParAppendResult res = manager->appendString(sp);
		assert(res == OP_ParAppendResult::Success);
	}
}

void 
ChucKListenerCHOP::pulsePressed(const char* name, void* reserved1)
{

}
