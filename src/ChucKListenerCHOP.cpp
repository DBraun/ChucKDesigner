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
#include "ChucKDesignerShared.h"

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <assert.h>

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

#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdio.h>

// for string delimiter
vector<string> split(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}


ChucKListenerCHOP::ChucKListenerCHOP(const OP_NodeInfo* info) : myNodeInfo(info)
{
	myExecuteCount = 0;
	myOffset = 0.0;
}

ChucKListenerCHOP::~ChucKListenerCHOP()
{
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

	auto Floatvarstrings = split(Floatvars, " ");

	myFloatVarNames.clear();

	if (Floatvarstrings.size() == 0) {

		info->numChannels = 0;
		return true;	
	}

	for (auto& str : Floatvarstrings) {
		myFloatVarNames.push_back(str);
	}

	info->numChannels = Floatvarstrings.size();

	//info->numSamples = 1;
    info->startIndex = 0;
	//info->sampleRate = 44100.;
	return true;
}

void
ChucKListenerCHOP::getChannelName(int32_t index, OP_String *name, const OP_Inputs* inputs, void* reserved1)
{

	if (index < myFloatVarNames.size()) {
		name->setString(myFloatVarNames.at(index).c_str());
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

	if (myFloatVarNames.size() == 0) {
		return;
	}

    const OP_CHOPInput* pluginCHOP = inputs->getParCHOP("Chuck");

	if (!pluginCHOP) {
		myStatus = false;
		myError << "Please select a chuck instance.";
		return;
	}

	std::string pluginFullPath(pluginCHOP->opPath);

	myChucKDesignerPlugin = ChucKDesignerShared::getChuckPluginInstance(pluginFullPath);

    if (!myChucKDesignerPlugin)
    {
        myStatus = false;
        myError << "Unable to get instance.";
        return;
    }

	int i = 0;

	for (const std::string varName : myFloatVarNames) {

		i += 1;

		myChucKDesignerPlugin->getChuckFloat(varName.c_str());
	}

	i = 0;
	for (const std::string varName : myFloatVarNames)
	{
		output->channels[i][0] = ChucKDesignerShared::getFloat(varName.c_str());
		i += 1;
	}
}

void ChucKListenerCHOP::getErrorString(OP_String* error, void* reserved1) {

    if (!myStatus) {
        error->setString(myError.str().c_str());
    }
}

int32_t
ChucKListenerCHOP::getNumInfoCHOPChans(void * reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 2 + myNumOutfloat;
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

	else if (index == 1)
	{
		chan->name->setString("offset");
		chan->value = (float)myOffset;
	}

    else {

        index -= 2;

        if (index < myNumOutfloat) {
            chan->name->setString(myOutfloatNameBuffer[index]);
            chan->value = myOutfloatValBuffer[index];
        }
    }
}

bool		
ChucKListenerCHOP::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 2;
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

	if (index == 1)
	{
		// Set the value for the first column
		entries->values[0]->setString("offset");

		// Set the value for the second column
#ifdef _WIN32
        sprintf_s(tempBuffer, "%g", myOffset);
#else // macOS
        snprintf(tempBuffer, sizeof(tempBuffer), "%g", myOffset);
#endif
		entries->values[1]->setString( tempBuffer);
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
}

void 
ChucKListenerCHOP::pulsePressed(const char* name, void* reserved1)
{
}

