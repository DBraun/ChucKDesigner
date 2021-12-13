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
#include "ChucKDesignerShared.h"
#include "chuck_globals.h"


std::string
ChucKDesignerPlugin::getNodeFullPath() const {

    std::string pluginFullPath(myNodeInfo->opPath);

    return pluginFullPath;
}

ChucKDesignerPlugin::ChucKDesignerPlugin(const OP_NodeInfo* info) : myNodeInfo(info)
{
    ChucKDesignerShared::addChuckPluginInstance(this);

	myExecuteCount = 0;
	myOffset = 0.0;
}

ChucKDesignerPlugin::~ChucKDesignerPlugin()
{
    ChucKDesignerShared::removeChuckPluginInstance(this);

    int chuckID = 0;
    //clearChuckInstance(chuckID);
    //clearGlobals(chuckID);
    //bool chuckManualAudioCallback(unsigned int chuckID, float* inBuffer, float* outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels);
    //cleanRegisteredChucks();
    //cleanupChuckInstance(chuckID);
	//ChucK::globalCleanup();
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
    if (chuck_instances.size() == 0) {
        info->numChannels = 0;
        return false;
    }

    auto chuck_inst = chuck_instances[0];
    auto vm = chuck_inst->vm();
    info->numChannels = (int) vm->m_num_dac_channels;
    info->sampleRate = std::max(1.f, (float) vm->srate());

    // Since we are outputting a timeslice, the system will dictate
    // the numSamples and startIndex of the CHOP data
    //info->numSamples = 1;
    info->startIndex = 0;
   
	return true;
}

void
ChucKDesignerPlugin::getChannelName(int32_t index, OP_String *name, const OP_Inputs* inputs, void* reserved1)
{
    std::stringstream ss;
    ss << "chan" << (index + 1);
    name->setString(ss.str().c_str());
}


bool
ChucKDesignerPlugin::runChuckCode(unsigned int chuckID, const char* code)
{
    if (chuck_instances.count(chuckID) == 0) {
        std::cout << "tried to run chuck code but no instance was found." << std::endl;
        return false;
    }

    // don't want to replace dac
    // (a safeguard in case compiler got interrupted while replacing dac)
    auto chuck = chuck_instances[chuckID];

    chuck->compiler()->setReplaceDac(FALSE, "");

    // compile it!
    const std::string argsTogether = string("");
    bool result = chuck_instances[chuckID]->compileCode(
        std::string(code), argsTogether);

    if (result) {
        // clear any existing error
        myError.str("");
    }
    else {
        myError.str("ChucK code did not compile correctly.");
    }
    
    return result;
}


bool
ChucKDesignerPlugin::runChuckCodeWithReplacementDac(unsigned int chuckID, const char* code, const char* replacement_dac)
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


bool
ChucKDesignerPlugin::runChuckFile(unsigned int chuckID, const char* filename)
{
    // run with empty args
    return runChuckFileWithArgs(chuckID, filename, "");
}


bool
ChucKDesignerPlugin::runChuckFileWithArgs(unsigned int chuckID, const char* filename, const char* args)
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


bool
ChucKDesignerPlugin::runChuckFileWithReplacementDac(unsigned int chuckID, const char* filename, const char* replacement_dac)
{
    // run with empty args
    return runChuckFileWithArgsWithReplacementDac(
        chuckID, filename, "", replacement_dac
    );
}


bool
ChucKDesignerPlugin::runChuckFileWithArgsWithReplacementDac(unsigned int chuckID, const char* filename, const char* args, const char* replacement_dac)
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


bool
ChucKDesignerPlugin::setChuckInt(unsigned int chuckID, const char* name, t_CKINT val)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->setGlobalInt(name, val);
}


bool
ChucKDesignerPlugin::getChuckInt(unsigned int chuckID, const char* name, void (*callback)(const char*, t_CKINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->getGlobalInt(name, callback);
}


bool
ChucKDesignerPlugin::setChuckFloat(unsigned int chuckID, const char* name, t_CKFLOAT val)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->setGlobalFloat(name, val);
}


bool
ChucKDesignerPlugin::getChuckFloat(unsigned int chuckID, const char* name, void (*callback)(const char*, t_CKFLOAT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->getGlobalFloat(name, callback);
}


bool
ChucKDesignerPlugin::setChuckString(unsigned int chuckID, const char* name, const char* val)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->setGlobalString(name, val);
}


bool
ChucKDesignerPlugin::getChuckString(unsigned int chuckID, const char* name, void (*callback)(const char*, const char*))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->getGlobalString(name, callback);
}


bool
ChucKDesignerPlugin::signalChuckEvent(unsigned int chuckID, const char* name)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->signalGlobalEvent(name);
}


bool
ChucKDesignerPlugin::broadcastChuckEvent(unsigned int chuckID, const char* name)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->broadcastGlobalEvent(name);
}


bool
ChucKDesignerPlugin::listenForChuckEventOnce(unsigned int chuckID, const char* name, void (*callback)(const char*))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->listenForGlobalEvent(
        name, callback, FALSE);
}


bool
ChucKDesignerPlugin::startListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(const char*))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->listenForGlobalEvent(
        name, callback, TRUE);
}


bool
ChucKDesignerPlugin::stopListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(const char*))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->stopListeningForGlobalEvent(
        name, callback);
}


bool
ChucKDesignerPlugin::getGlobalUGenSamples(unsigned int chuckID,
    const char* name, SAMPLE* buffer, int numSamples)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    if (!chuck_instances[chuckID]->globals()->getGlobalUGenSamples(
        name, buffer, numSamples))
    {
        // failed. fill with zeroes.
        memset(buffer, 0, sizeof(SAMPLE) * numSamples);
        return false;
    }

    return true;
}


// int array methods
bool
ChucKDesignerPlugin::setGlobalIntArray(unsigned int chuckID,
    const char* name, t_CKINT arrayValues[], unsigned int numValues)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->setGlobalIntArray(
        name, arrayValues, numValues);
}


bool
ChucKDesignerPlugin::getGlobalIntArray(unsigned int chuckID,
    const char* name, void (*callback)(const char*, t_CKINT[], t_CKUINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->getGlobalIntArray(
        name, callback);
}


bool
ChucKDesignerPlugin::setGlobalIntArrayValue(unsigned int chuckID,
    const char* name, unsigned int index, t_CKINT value)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->setGlobalIntArrayValue(
        name, index, value);
}


bool
ChucKDesignerPlugin::getGlobalIntArrayValue(unsigned int chuckID,
    const char* name, unsigned int index, void (*callback)(const char*, t_CKINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->getGlobalIntArrayValue(
        name, index, callback);
}


bool
ChucKDesignerPlugin::setGlobalAssociativeIntArrayValue(
    unsigned int chuckID, const char* name, char* key, t_CKINT value)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->setGlobalAssociativeIntArrayValue(
        name, key, value);
}


bool
ChucKDesignerPlugin::getGlobalAssociativeIntArrayValue(
    unsigned int chuckID, const char* name, char* key,
    void (*callback)(const char*, t_CKINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->getGlobalAssociativeIntArrayValue(
        name, key, callback);
}


// float array methods
bool
ChucKDesignerPlugin::setGlobalFloatArray(unsigned int chuckID,
    const char* name, t_CKFLOAT arrayValues[], unsigned int numValues)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->setGlobalFloatArray(
        name, arrayValues, numValues);
}


bool
ChucKDesignerPlugin::getGlobalFloatArray(unsigned int chuckID,
    const char* name, void (*callback)(const char*, t_CKFLOAT[], t_CKUINT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->getGlobalFloatArray(
        name, callback);
}


bool
ChucKDesignerPlugin::setGlobalFloatArrayValue(unsigned int chuckID,
    const char* name, unsigned int index, t_CKFLOAT value)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->setGlobalFloatArrayValue(
        name, index, value);
}


bool
ChucKDesignerPlugin::getGlobalFloatArrayValue(unsigned int chuckID,
    const char* name, unsigned int index, void (*callback)(const char*, t_CKFLOAT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->getGlobalFloatArrayValue(
        name, index, callback);
}


bool
ChucKDesignerPlugin::setGlobalAssociativeFloatArrayValue(
    unsigned int chuckID, const char* name, char* key, t_CKFLOAT value)
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->setGlobalAssociativeFloatArrayValue(
        name, key, value);
}


bool
ChucKDesignerPlugin::getGlobalAssociativeFloatArrayValue(
    unsigned int chuckID, const char* name, char* key,
    void (*callback)(const char*, t_CKFLOAT))
{
    if (chuck_instances.count(chuckID) == 0) { return false; }

    return chuck_instances[chuckID]->globals()->getGlobalAssociativeFloatArrayValue(
        name, key, callback);
}


bool
ChucKDesignerPlugin::setChoutCallback(unsigned int chuckID, void (*callback)(const char*))
{
    return chuck_instances[chuckID]->setChoutCallback(callback);
}


bool
ChucKDesignerPlugin::setCherrCallback(unsigned int chuckID, void (*callback)(const char*))
{
    return chuck_instances[chuckID]->setCherrCallback(callback);
}


bool
ChucKDesignerPlugin::setStdoutCallback(void (*callback)(const char*))
{
    return ChucK::setStdoutCallback(callback);
}


bool
ChucKDesignerPlugin::setStderrCallback(void (*callback)(const char*))
{
    return ChucK::setStderrCallback(callback);
}


bool
ChucKDesignerPlugin::setDataDir(const char* dir)
{
    chuck_global_data_dir = std::string(dir);
    return true;
}


bool
ChucKDesignerPlugin::setLogLevel(unsigned int level)
{
    EM_setlog(level);
    return true;
}

bool
ChucKDesignerPlugin::initChuckInstance(unsigned int chuckID, unsigned int sampleRate, unsigned int numInChans, unsigned int numOutChans)
{
    if (chuck_instances.count(chuckID) == 0)
    {
        // if we aren't tracking a chuck vm on this ID, create a new one
        ChucK* chuck = new ChucK();

        // set params: sample rate, 2 in channels, 2 out channels,
        // don't halt the vm, and use our data directory
        chuck->setParam(CHUCK_PARAM_SAMPLE_RATE, (t_CKINT)sampleRate);
        chuck->setParam(CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT)numInChans);
        chuck->setParam(CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT)numOutChans);
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


bool
ChucKDesignerPlugin::clearChuckInstance(unsigned int chuckID)
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
        chuck->vm()->globals_manager()->execute_chuck_msg_with_globals(msg);

        return true;
    }

    return false;
}


bool
ChucKDesignerPlugin::clearGlobals(unsigned int chuckID)
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
        chuck->vm()->globals_manager()->execute_chuck_msg_with_globals(msg);

        return true;
    }

    return false;
}


bool
ChucKDesignerPlugin::cleanupChuckInstance(unsigned int chuckID)
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


bool
ChucKDesignerPlugin::chuckManualAudioCallback(unsigned int chuckID, float* inBuffer, float* outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels)
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
void
ChucKDesignerPlugin::cleanRegisteredChucks() {

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


bool
ChucKDesignerPlugin::RegisterChuckData(EffectData::Data* data, const unsigned int id)
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


void
ChucKDesignerPlugin::execute(CHOP_Output* output,
							  const OP_Inputs* inputs,
							  void* reserved)
{

    mySampleRateRequest = inputs->getParDouble("Samplerate");

    unsigned int numOutChannels = inputs->getParInt("Outchannels");
    unsigned int numInChannels = 2;

	myExecuteCount++;

    if (needCompile) {

        if (chuck_instances.size() == 0) {

            mySampleRate = mySampleRateRequest;

            inbuffer = new float[mySampleRate / 60. * 600.];
            outbuffer = new float[mySampleRate / 60. * 60.];

            double videoFramesPerSec = 60.;
            float typicalSamplesPerFrame = mySampleRate / videoFramesPerSec;
            typicalSamplesPerFrame *= 4.; // quadruple to be safe for dropped frames
            outbuffer = new float[typicalSamplesPerFrame * numOutChannels];

            int chuckID = 0;

            initChuckInstance(0, mySampleRate, numInChannels, numOutChannels);

//            const char* code = R"V0G0N(
//
//SinOsc foo => dac;
//
//300. => global float freq => foo.freq;
//
//global float otherSineGlobal;
//
//SinOsc blackholeSine => blackhole;
//1. => blackholeSine.freq;
//
//while( true )
//{
//	freq => foo.freq;
//
//    blackholeSine.last() => otherSineGlobal;
//
//	10::ms => now;
//}
//    )V0G0N";
//            myStatus = runChuckCode(chuckID, code);
        }

        const OP_DATInput* input = inputs->getParDAT("Code");
        const char* code = *(input->cellData);
        std::cout << "code: " << code << std::endl;

        if (chuck_instances.size()) {

            ChucK* chuck = chuck_instances[0];
            clearChuckInstance(0);
            myStatus = runChuckCode(0, code);
        }

        needCompile = false;
    }

    if (chuck_instances.size())
    {
        ChucK* chuck = chuck_instances[0];

        const OP_CHOPInput* Infloat_CHOPInput = inputs->getParCHOP("Infloat");
        if (Infloat_CHOPInput) {
            for (size_t chanIndex = 0; chanIndex < Infloat_CHOPInput->numChannels; chanIndex++)
            {
                const char* chanName = Infloat_CHOPInput->getChannelName(chanIndex);
                float chanVal = Infloat_CHOPInput->channelData[chanIndex][0];
                auto result = chuck->globals()->setGlobalFloat(chanName, chanVal);
            }
        }

        int numOutChans = chuck->vm()->m_num_dac_channels;
        if (numOutChans != output->numChannels) {
            return;
        }

        // chuck->run(inbuffer, *output->channels, output->numSamples); // this doesn't work because of interleaved samples.
        // Chuck returns LRLRLRLR but for touchdesigner we want LLLLRRRR.
        // Therefore we must use an intermediate buffer

        chuck->run(inbuffer, outbuffer, output->numSamples);

        float* ptr = outbuffer;
        for (int i = 0; i < output->numSamples; i++) {
            for (int chan = 0; chan < numOutChans; chan++) {
                output->channels[chan][i] = *ptr++;
            }
        }
    }
}

void
ChucKDesignerPlugin::getErrorString(OP_String* error, void* reserved1) {

    if (!myStatus) {
        error->setString(myError.str().c_str());
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

	else if (index == 1)
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

    // Sample Rate
    {
    	OP_NumericParameter	np;

    	np.name = "Samplerate";
    	np.label = "Sample Rate";
    	np.defaultValues[0] = 44100;
    	np.minSliders[0] = 0.0;
    	np.maxSliders[0] =  96000.0;
        np.clampMins[0] = true;
    	
    	OP_ParAppendResult res = manager->appendFloat(np);
    	assert(res == OP_ParAppendResult::Success);
    }

    // Num out channels
    {
        OP_NumericParameter	np;

        np.name = "Outchannels";
        np.label = "Out Channels";
        np.defaultValues[0] = 2;
        np.minSliders[0] = 1;
        np.maxSliders[0] = 16;
        np.minValues[0] = 0;
        np.clampMins[0] = true;

        OP_ParAppendResult res = manager->appendInt(np);
        assert(res == OP_ParAppendResult::Success);
    }

	//// speed
	//{
	//	OP_NumericParameter	np;

	//	np.name = "Speed";
	//	np.label = "Speed";
	//	np.defaultValues[0] = 1.0;
	//	np.minSliders[0] = -10.0;
	//	np.maxSliders[0] =  10.0;
	//	
	//	OP_ParAppendResult res = manager->appendFloat(np);
	//	assert(res == OP_ParAppendResult::Success);
	//}

	//// scale
	//{
	//	OP_NumericParameter	np;

	//	np.name = "Scale";
	//	np.label = "Scale";
	//	np.defaultValues[0] = 1.0;
	//	np.minSliders[0] = -10.0;
	//	np.maxSliders[0] =  10.0;
	//	
	//	OP_ParAppendResult res = manager->appendFloat(np);
	//	assert(res == OP_ParAppendResult::Success);
	//}

	//// shape
	//{
	//	OP_StringParameter	sp;

	//	sp.name = "Shape";
	//	sp.label = "Shape";

	//	sp.defaultValue = "Sine";

	//	const char *names[] = { "Sine", "Square", "Ramp" };
	//	const char *labels[] = { "Sine", "Square", "Ramp" };

	//	OP_ParAppendResult res = manager->appendMenu(sp, 3, names, labels);
	//	assert(res == OP_ParAppendResult::Success);
	//}

    // chuck source code DAT
    {
        OP_StringParameter	sp;

        sp.name = "Code";
        sp.label = "Code";

        sp.defaultValue = "";

        OP_ParAppendResult res = manager->appendDAT(sp);
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

    // Global In Float CHOP
    {
        OP_StringParameter	sp;

        sp.name = "Infloat";
        sp.label = "In Float";

        sp.defaultValue = "";

        OP_ParAppendResult res = manager->appendCHOP(sp);
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
