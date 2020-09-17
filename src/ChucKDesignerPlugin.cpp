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

#include "ChucKDesignerPlugin.h"

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
	info->customOPInfo.opType->setString("Chuckaudio");

	// The opLabel is the text that will show up in the OP Create Dialog
	info->customOPInfo.opLabel->setString("ChucK Audio");

	// Information about the author of this OP
	info->customOPInfo.authorName->setString("David Braun");
	info->customOPInfo.authorEmail->setString("github.com/DBraun");

	// This CHOP can work with 0 inputs
	info->customOPInfo.minInputs = 0;

	// It can accept up to 1 input though, which changes it's behavior
	info->customOPInfo.maxInputs = 1;
}

DLLEXPORT
CHOP_CPlusPlusBase*
CreateCHOPInstance(const OP_NodeInfo* info)
{
	// Return a new instance of your class every time this is called.
	// It will be called once per CHOP that is using the .dll
	return new ChucKDesignerPlugin(info);
}

DLLEXPORT
void
DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
{
	// Delete the instance here, this will be called when
	// Touch is shutting down, when the CHOP using that instance is deleted, or
	// if the CHOP loads a different DLL
	delete (ChucKDesignerPlugin*)instance;
}

};




ChucKDesignerPlugin::ChucKDesignerPlugin(const OP_NodeInfo* info) : myNodeInfo(info)
{
	myExecuteCount = 0;
	myOffset = 0.0;

    int sampleRate = 44100;

    inbuffer = new float[sampleRate / 60. * 600.];
    outbuffer = new float[sampleRate / 60. * 60.];

    int chuckID = 0;

    initChuckInstance(0, sampleRate);

  //  //const char* code =
  //  //    "SinOsc foo = > dac; \
  //  //    while (true) \
  //  //    { \
  //  //        Math.random2f(300, 1000) = > foo.freq; \
  //  //        100::ms = > now; \
  //  //    }";

    const char* code = R"V0G0N(

		SinOsc foo => dac;
		while( true )
		{
			Math.random2f( 300, 1000 ) => foo.freq;
			200::ms => now;
		}
    )V0G0N";
    myStatus = runChuckCode(chuckID, code);

    if (myStatus) {
        std::cout << "running code" << std::endl;
    }
    else {
        std::cout << "not running code" << std::endl;
    }


}

ChucKDesignerPlugin::~ChucKDesignerPlugin()
{
    int chuckID = 0;
    //clearChuckInstance(chuckID);
    //clearGlobals(chuckID);
    //bool chuckManualAudioCallback(unsigned int chuckID, float* inBuffer, float* outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels);
    //cleanRegisteredChucks();
    //cleanupChuckInstance(chuckID);
	ChucK::globalCleanup();
}

void
ChucKDesignerPlugin::getGeneralInfo(CHOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = true;

	// Note: To disable timeslicing you'll need to turn this off, as well as ensure that
	// getOutputInfo() returns true, and likely also set the info->numSamples to how many
	// samples you want to generate for this CHOP. Otherwise it'll take on length of the
	// input CHOP, which may be timesliced.
	ginfo->timeslice = true;

	ginfo->inputMatchIndex = 0;
}

bool
ChucKDesignerPlugin::getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs* inputs, void* reserved1)
{
	// If there is an input connected, we are going to match it's channel names etc
	// otherwise we'll specify our own.
	//if (inputs->getNumInputs() > 0)
	//{
	//	return false;
	//}
	//else
	{
		info->numChannels = 2;

		// Since we are outputting a timeslice, the system will dictate
		// the numSamples and startIndex of the CHOP data
		//info->numSamples = 1;
        info->startIndex = 0;

		// For illustration we are going to output 120hz data
		info->sampleRate = 44100.;
		return true;
	}
}

void
ChucKDesignerPlugin::getChannelName(int32_t index, OP_String *name, const OP_Inputs* inputs, void* reserved1)
{
    if (index == 0) {
        name->setString("chan1");
    }
    else {
        name->setString("chan2");
    }
	
}


// C# "string" corresponds to passing char *
bool ChucKDesignerPlugin::runChuckCode(unsigned int chuckID, const char* code)
{
    if (chuck_instances.count(chuckID) == 0) {
        std::cout << "tried to run chuck code but no instance was found." << std::endl;
        return false;
    }

    // don't want to replace dac
    // (a safeguard in case compiler got interrupted while replacing dac)
    chuck_instances[chuckID]->compiler()->setReplaceDac(FALSE, "");

    // compile it!
    return chuck_instances[chuckID]->compileCode(
        std::string(code), std::string(""));
}



bool ChucKDesignerPlugin::runChuckCodeWithReplacementDac(unsigned int chuckID, const char* code, const char* replacement_dac)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    // replace dac
    chuck_instances[chuckID]->compiler()->setReplaceDac(TRUE,
        std::string(replacement_dac));

    // compile it!
    bool ret = chuck_instances[chuckID]->compileCode(
        std::string(code), std::string(""));

    // don't replace dac for future compilations
    chuck_instances[chuckID]->compiler()->setReplaceDac(FALSE, "");

    return ret;
}



bool ChucKDesignerPlugin::runChuckFile(unsigned int chuckID, const char* filename)
{
    // run with empty args
    return runChuckFileWithArgs(chuckID, filename, "");
}



bool ChucKDesignerPlugin::runChuckFileWithArgs(unsigned int chuckID, const char* filename, const char* args)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    // don't want to replace dac
    // (a safeguard in case compiler got interrupted while replacing dac)
    chuck_instances[chuckID]->compiler()->setReplaceDac(FALSE, "");

    // compile it!
    return chuck_instances[chuckID]->compileFile(
        std::string(filename), std::string(args)
    );
}



bool ChucKDesignerPlugin::runChuckFileWithReplacementDac(unsigned int chuckID, const char* filename, const char* replacement_dac)
{
    // run with empty args
    return runChuckFileWithArgsWithReplacementDac(
        chuckID, filename, "", replacement_dac
    );
}



bool ChucKDesignerPlugin::runChuckFileWithArgsWithReplacementDac(unsigned int chuckID, const char* filename, const char* args, const char* replacement_dac)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    // replace dac
    chuck_instances[chuckID]->compiler()->setReplaceDac(TRUE,
        std::string(replacement_dac));

    // compile it!
    bool ret = chuck_instances[chuckID]->compileFile(
        std::string(filename), std::string(args)
    );

    // don't replace dac for future compilations
    chuck_instances[chuckID]->compiler()->setReplaceDac(FALSE, "");

    return ret;
}



bool ChucKDesignerPlugin::setChuckInt(unsigned int chuckID, const char* name, t_CKINT val)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->setGlobalInt(name, val);
}



bool ChucKDesignerPlugin::getChuckInt(unsigned int chuckID, const char* name, void (*callback)(t_CKINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->getGlobalInt(name, callback);
}



bool ChucKDesignerPlugin::setChuckFloat(unsigned int chuckID, const char* name, t_CKFLOAT val)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->setGlobalFloat(name, val);
}



bool ChucKDesignerPlugin::getChuckFloat(unsigned int chuckID, const char* name, void (*callback)(t_CKFLOAT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->getGlobalFloat(name, callback);
}



bool ChucKDesignerPlugin::setChuckString(unsigned int chuckID, const char* name, const char* val)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->setGlobalString(name, val);
}



bool ChucKDesignerPlugin::getChuckString(unsigned int chuckID, const char* name, void (*callback)(const char*))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->getGlobalString(name, callback);
}



bool ChucKDesignerPlugin::signalChuckEvent(unsigned int chuckID, const char* name)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->signalGlobalEvent(name);
}



bool ChucKDesignerPlugin::broadcastChuckEvent(unsigned int chuckID, const char* name)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->broadcastGlobalEvent(name);
}



bool ChucKDesignerPlugin::listenForChuckEventOnce(unsigned int chuckID, const char* name, void (*callback)(void))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->listenForGlobalEvent(
        name, callback, FALSE);
}



bool ChucKDesignerPlugin::startListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(void))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->listenForGlobalEvent(
        name, callback, TRUE);
}



bool ChucKDesignerPlugin::stopListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(void))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->stopListeningForGlobalEvent(
        name, callback);
}


bool ChucKDesignerPlugin::getGlobalUGenSamples(unsigned int chuckID,
    const char* name, SAMPLE* buffer, int numSamples)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    if (!chuck_instances[chuckID]->getGlobalUGenSamples(
        name, buffer, numSamples))
    {
        // failed. fill with zeroes.
        memset(buffer, 0, sizeof(SAMPLE) * numSamples);
        return false;
    }

    return true;
}



// int array methods
bool ChucKDesignerPlugin::setGlobalIntArray(unsigned int chuckID,
    const char* name, t_CKINT arrayValues[], unsigned int numValues)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->setGlobalIntArray(
        name, arrayValues, numValues);
}



bool ChucKDesignerPlugin::getGlobalIntArray(unsigned int chuckID,
    const char* name, void (*callback)(t_CKINT[], t_CKUINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->getGlobalIntArray(
        name, callback);
}



bool ChucKDesignerPlugin::setGlobalIntArrayValue(unsigned int chuckID,
    const char* name, unsigned int index, t_CKINT value)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->setGlobalIntArrayValue(
        name, index, value);
}



bool ChucKDesignerPlugin::getGlobalIntArrayValue(unsigned int chuckID,
    const char* name, unsigned int index, void (*callback)(t_CKINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->getGlobalIntArrayValue(
        name, index, callback);
}



bool ChucKDesignerPlugin::setGlobalAssociativeIntArrayValue(
    unsigned int chuckID, const char* name, char* key, t_CKINT value)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->setGlobalAssociativeIntArrayValue(
        name, key, value);
}



bool ChucKDesignerPlugin::getGlobalAssociativeIntArrayValue(
    unsigned int chuckID, const char* name, char* key,
    void (*callback)(t_CKINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->getGlobalAssociativeIntArrayValue(
        name, key, callback);
}



// float array methods
bool ChucKDesignerPlugin::setGlobalFloatArray(unsigned int chuckID,
    const char* name, t_CKFLOAT arrayValues[], unsigned int numValues)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->setGlobalFloatArray(
        name, arrayValues, numValues);
}



bool ChucKDesignerPlugin::getGlobalFloatArray(unsigned int chuckID,
    const char* name, void (*callback)(t_CKFLOAT[], t_CKUINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->getGlobalFloatArray(
        name, callback);
}



bool ChucKDesignerPlugin::setGlobalFloatArrayValue(unsigned int chuckID,
    const char* name, unsigned int index, t_CKFLOAT value)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->setGlobalFloatArrayValue(
        name, index, value);
}



bool ChucKDesignerPlugin::getGlobalFloatArrayValue(unsigned int chuckID,
    const char* name, unsigned int index, void (*callback)(t_CKFLOAT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->getGlobalFloatArrayValue(
        name, index, callback);
}



bool ChucKDesignerPlugin::setGlobalAssociativeFloatArrayValue(
    unsigned int chuckID, const char* name, char* key, t_CKFLOAT value)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->setGlobalAssociativeFloatArrayValue(
        name, key, value);
}



bool ChucKDesignerPlugin::getGlobalAssociativeFloatArrayValue(
    unsigned int chuckID, const char* name, char* key,
    void (*callback)(t_CKFLOAT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->getGlobalAssociativeFloatArrayValue(
        name, key, callback);
}



bool ChucKDesignerPlugin::setChoutCallback(unsigned int chuckID, void (*callback)(const char*))
{
    return chuck_instances[chuckID]->setChoutCallback(callback);
}



bool ChucKDesignerPlugin::setCherrCallback(unsigned int chuckID, void (*callback)(const char*))
{
    return chuck_instances[chuckID]->setCherrCallback(callback);
}



bool ChucKDesignerPlugin::setStdoutCallback(void (*callback)(const char*))
{
    return ChucK::setStdoutCallback(callback);
}



bool ChucKDesignerPlugin::setStderrCallback(void (*callback)(const char*))
{
    return ChucK::setStderrCallback(callback);
}



bool ChucKDesignerPlugin::setDataDir(const char* dir)
{
    chuck_global_data_dir = std::string(dir);
    return true;
}



bool ChucKDesignerPlugin::setLogLevel(unsigned int level)
{
    EM_setlog(level);
    return true;
}



bool ChucKDesignerPlugin::initChuckInstance(unsigned int chuckID, unsigned int sampleRate)
{
    if (chuck_instances.count(chuckID) == 0)
    {
        // if we aren't tracking a chuck vm on this ID, create a new one
        ChucK* chuck = new ChucK();

        // set params: sample rate, 2 in channels, 2 out channels,
        // don't halt the vm, and use our data directory
        chuck->setParam(CHUCK_PARAM_SAMPLE_RATE, (t_CKINT)sampleRate);
        chuck->setParam(CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT)2);
        chuck->setParam(CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT)2);
        chuck->setParam(CHUCK_PARAM_VM_HALT, (t_CKINT)0);
        chuck->setParam(CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT)0);
        // directory for compiled.code
        chuck->setParam(CHUCK_PARAM_WORKING_DIRECTORY, chuck_global_data_dir);
        // directories to search for chugins and auto-run ck files
        std::list< std::string > chugin_search;
        chugin_search.push_back(chuck_global_data_dir + "/chugins");
        chuck->setParam(CHUCK_PARAM_USER_CHUGIN_DIRECTORIES, chugin_search);

        // initialize and start
        chuck->init();
        chuck->start();

        chuck_instances[chuckID] = chuck;
    }
    return true;
}



bool ChucKDesignerPlugin::clearChuckInstance(unsigned int chuckID)
{
    if (chuck_instances.count(chuckID) > 0)
    {
        // the chuck to clear
        ChucK* chuck = chuck_instances[chuckID];

        // create a msg asking to clear the VM
        Chuck_Msg* msg = new Chuck_Msg;
        msg->type = MSG_CLEARVM;

        // null reply so that VM will delete for us when it's done
        msg->reply = (ck_msg_func)NULL;

        // tell the VM to clear
        chuck->vm()->execute_chuck_msg_with_globals(msg);

        return true;
    }

    return false;
}



bool ChucKDesignerPlugin::clearGlobals(unsigned int chuckID)
{
    if (chuck_instances.count(chuckID) > 0)
    {
        // the chuck to clear
        ChucK* chuck = chuck_instances[chuckID];

        // create a msg asking to clear the globals
        Chuck_Msg* msg = new Chuck_Msg;
        msg->type = MSG_CLEARGLOBALS;

        // null reply so that VM will delete for us when it's done
        msg->reply = (ck_msg_func)NULL;

        // tell the VM to clear
        chuck->vm()->execute_chuck_msg_with_globals(msg);

        return true;
    }

    return false;
}


bool ChucKDesignerPlugin::cleanupChuckInstance(unsigned int chuckID)
{
    if (chuck_instances.count(chuckID) > 0)
    {
        ChucK* chuck = chuck_instances[chuckID];

        // don't track it anymore
        chuck_instances.erase(chuckID);

        if (data_instances.count(chuckID) > 0)
        {
            data_instances[chuckID]->myId = -1;
            data_instances.erase(chuckID);
        }

        // wait a bit
        usleep(30000);

        // cleanup this chuck early
        delete chuck;

    }

    return true;
}



bool ChucKDesignerPlugin::chuckManualAudioCallback(unsigned int chuckID, float* inBuffer, float* outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels)
{
    if (chuck_instances.count(chuckID) > 0)
    {
        // zero out the output buffer, in case chuck isn't running
        for (unsigned int n = 0; n < numFrames * outChannels; n++)
        {
            outBuffer[n] = 0;
        }

        // call callback
        // TODO: check inChannels, outChannels
        //chuck_instances[chuckID]->run(inBuffer, outBuffer, numFrames);

    }

    return true;
}



// on launch, reset all ids (necessary when relaunching a lot in unity editor)
void ChucKDesignerPlugin::cleanRegisteredChucks() {

    // first, invalidate all callbacks' references to chucks
    for (std::map< unsigned int, EffectData::Data* >::iterator it =
        data_instances.begin(); it != data_instances.end(); it++)
    {
        EffectData::Data* data = it->second;
        data->myId = -1;
    }

    // wait for callbacks to finish their current run
    usleep(30000);

    // next, delete chucks
    for (std::map< unsigned int, ChucK* >::iterator it =
        chuck_instances.begin(); it != chuck_instances.end(); it++)
    {
        ChucK* chuck = it->second;
        delete chuck;
    }

    // delete stored chuck pointers
    chuck_instances.clear();
    // delete data instances
    data_instances.clear();

    // clear out callbacks also
    setStdoutCallback(NULL);
    setStderrCallback(NULL);
}


bool ChucKDesignerPlugin::RegisterChuckData(EffectData::Data* data, const unsigned int id)
{
    // only store if id has been used / a chuck is already initialized
    if (chuck_instances.count(id) == 0)
    {
        return false;
    }

    // store id on data; note we might be replacing a non-zero id
    //  in the case when unity is reusing an audio callback the next time
    //  the scene is entered.
    data->myId = id;

    // store the data pointer, for validation later.
    // the chuck associated with id should only work with *this* data.
    data_instances[id] = data;

    return true;
}


#if !UNITY_SPU

//int InternalRegisterEffectDefinition(UnityAudioEffectDefinition& definition)
//{
//    int numparams = P_NUM;
//    definition.paramdefs = new UnityAudioParameterDefinition[numparams];
//    // float vals are: min, max, default, scale (?), scale (?)
//    RegisterParameter(definition, "ChucK ID", "#", -1.0f, 256.0f, -1.0f, 1.0f, 1.0f, P_CHUCKID, "Internal ID number used to run ChucK scripts programmatically. Leave set to -1 manually.");
//    return numparams;
//}


// NOTE: CreateCallback and ReleaseCallback are called at odd times, e.g.
// - When unity launches for the first time and when unity exits
// - When a new effect is added
// - When a parameter is exposed
// - NOT when play mode is activated or deactivated
// instantiation
//UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK CreateCallback(UnityAudioEffectState* state)
//{
//    EffectData* effectdata = new EffectData;
//    memset(effectdata, 0, sizeof(EffectData));

//    // don't hook into any particular chuck; id not yet set
//    effectdata->data.myId = -1;

//    state->effectdata = effectdata;
//    InitParametersFromDefinitions(InternalRegisterEffectDefinition, effectdata->data.p);

//    return UNITY_AUDIODSP_OK;
//}


// deletion
//UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ReleaseCallback(UnityAudioEffectState* state)
//{
//    EffectData::Data* data = &state->GetEffectData<EffectData>()->data;

//    delete data;

//    return UNITY_AUDIODSP_OK;
//}


// set param (piggy-backed to store chucks by id)
//UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK SetFloatParameterCallback(UnityAudioEffectState* state, int index, float value)
//{
//    EffectData::Data* data = &state->GetEffectData<EffectData>()->data;
//    if (index >= P_NUM)
//        return UNITY_AUDIODSP_ERR_UNSUPPORTED;

//    // setting ID, time to cache the pointer to effect data
//    if (index == P_CHUCKID && value >= 0.0f)
//    {
//        // if false( ie already registered data or id ),
//        // an error will be outputted to the Unity console
//        if (RegisterChuckData(data, (unsigned int)value) == false)
//        {
//            return UNITY_AUDIODSP_ERR_UNSUPPORTED;
//        }
//    }

//    data->p[index] = value;
//    return UNITY_AUDIODSP_OK;
//}


// get param (not useful for anything for ChucK)
//UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK GetFloatParameterCallback(UnityAudioEffectState* state, int index, float* value, char* valuestr)
//{
//    EffectData::Data* data = &state->GetEffectData<EffectData>()->data;
//    if (index >= P_NUM)
//        return UNITY_AUDIODSP_ERR_UNSUPPORTED;
//    if (value != NULL) {
//        *value = data->p[index];
//    }
//    if (valuestr != NULL)
//        valuestr[0] = 0;
//    return UNITY_AUDIODSP_OK;
//}


// get float buffer (not useful for anything for ChucK)
//int UNITY_AUDIODSP_CALLBACK GetFloatBufferCallback(UnityAudioEffectState* state, const char* name, float* buffer, int numsamples)
//{
//    return UNITY_AUDIODSP_OK;
//}

#endif


void
ChucKDesignerPlugin::execute(CHOP_Output* output,
							  const OP_Inputs* inputs,
							  void* reserved)
{
	myExecuteCount++;

    if (needCompile) {

        const OP_DATInput* input = inputs->getParDAT("Code");
        const char* code = *(input->cellData);
        std::cout << "code: " << code << std::endl;

        if (chuck_instances.size()) {

            ChucK* chuck = chuck_instances[0];
            clearChuckInstance(0);
            myStatus = runChuckCode(0, code);

            if (myStatus) {
                std::cout << "running code" << std::endl;
            }
            else {
                std::cout << "not running code" << std::endl;
            }

        }

        needCompile = false;
    }
	
	double	 scale = inputs->getParDouble("Scale");

	// In this case we'll just take the first input and re-output it scaled.

	if (inputs->getNumInputs() > 0)
	{
		// We know the first CHOP has the same number of channels
		// because we returned false from getOutputInfo. 

		inputs->enablePar("Speed", 0);	// not used
		inputs->enablePar("Shape", 0);	// not used

		int ind = 0;
		const OP_CHOPInput	*cinput = inputs->getInputCHOP(0);

		for (int i = 0 ; i < output->numChannels; i++)
		{
			for (int j = 0; j < output->numSamples; j++)
			{
				output->channels[i][j] = float(cinput->getChannelData(i)[ind] * scale);
				ind++;

				// Make sure we don't read past the end of the CHOP input
				ind = ind % cinput->numSamples;
			}
		}

	}
	else // If not input is connected, lets output a sine wave instead
	{
		inputs->enablePar("Speed", 1);

		double speed = inputs->getParDouble("Speed");
		double step = speed * 0.01f;


		// menu items can be evaluated as either an integer menu position, or a string
		int shape = inputs->getParInt("Shape");
//		const char *shape_str = inputs->getParString("Shape");

		// keep each channel at a different phase
		double phase = 2.0f * 3.14159f / (float)(output->numChannels);

		// Notice that startIndex and the output->numSamples is used to output a smooth
		// wave by ensuring that we are outputting a value for each sample
		// Since we are outputting at 120, for each frame that has passed we'll be
		// outputing 2 samples (assuming the timeline is running at 60hz).


		for (int i = 0; i < output->numChannels; i++)
		{
			double offset = myOffset + phase*i;


			double v = 0.0f;

			switch(shape)
			{
				case 0:		// sine
					v = sin(offset);
					break;

				case 1:		// square
					v = fabs(fmod(offset, 1.0)) > 0.5;
					break;

				case 2:		// ramp	
					v = fabs(fmod(offset, 1.0));
					break;
			}


			v *= scale;

			for (int j = 0; j < output->numSamples; j++)
			{
				output->channels[i][j] = float(v);
				offset += step;
			}
		}

		myOffset += step * output->numSamples; 
	}

    // ChucK stuff

    // UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ProcessCallback(UnityAudioEffectState * state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int outchannels)
    // EffectData::Data* data = &state->GetEffectData<EffectData>()->data;

    //// zero out the output buffer, in case chuck isn't running
    //for (unsigned int n = 0; n < length * outchannels; n++)
    //{
    //    outbuffer[n] = 0;
    //}

    //// if we think we can, call chuck callback
    //if (chuck_instances.count(data->myId) > 0    // do we have a chuck
    //    && data_instances.count(data->myId) > 0  // do we have a data
    //    && data_instances[data->myId] == data)    // && is it still aligned



    if (chuck_instances.size())
    {
        ChucK* chuck = chuck_instances[0];

        if (output->numSamples > 1) {
            chuck->run(inbuffer, *(output->channels), output->numSamples);
        }

        
    }

}

int32_t
ChucKDesignerPlugin::getNumInfoCHOPChans(void * reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 2;
}

void
ChucKDesignerPlugin::getInfoCHOPChan(int32_t index,
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

	if (index == 1)
	{
		chan->name->setString("offset");
		chan->value = (float)myOffset;
	}
}

bool		
ChucKDesignerPlugin::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 2;
	infoSize->cols = 2;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void
ChucKDesignerPlugin::getInfoDATEntries(int32_t index,
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
ChucKDesignerPlugin::setupParameters(OP_ParameterManager* manager, void *reserved1)
{
	// speed
	{
		OP_NumericParameter	np;

		np.name = "Speed";
		np.label = "Speed";
		np.defaultValues[0] = 1.0;
		np.minSliders[0] = -10.0;
		np.maxSliders[0] =  10.0;
		
		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// scale
	{
		OP_NumericParameter	np;

		np.name = "Scale";
		np.label = "Scale";
		np.defaultValues[0] = 1.0;
		np.minSliders[0] = -10.0;
		np.maxSliders[0] =  10.0;
		
		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// shape
	{
		OP_StringParameter	sp;

		sp.name = "Shape";
		sp.label = "Shape";

		sp.defaultValue = "Sine";

		const char *names[] = { "Sine", "Square", "Ramp" };
		const char *labels[] = { "Sine", "Square", "Ramp" };

		OP_ParAppendResult res = manager->appendMenu(sp, 3, names, labels);
		assert(res == OP_ParAppendResult::Success);
	}

	// pulse
	{
		OP_NumericParameter	np;

		np.name = "Compile";
		np.label = "Compile";
		
		OP_ParAppendResult res = manager->appendPulse(np);
		assert(res == OP_ParAppendResult::Success);
	}

    // chuck source code DAT
    {
        OP_StringParameter	sp;

        sp.name = "Code";
        sp.label = "Code";

        sp.defaultValue = "";

        OP_ParAppendResult res = manager->appendDAT(sp);
        assert(res == OP_ParAppendResult::Success);
    }

}

void 
ChucKDesignerPlugin::pulsePressed(const char* name, void* reserved1)
{
	if (!strcmp(name, "Compile"))
	{
        needCompile = true;
	}
}

