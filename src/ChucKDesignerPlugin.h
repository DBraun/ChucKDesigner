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

#include "CHOP_CPlusPlusBase.h"
#include "chuck.h"

/*

This example file implements a class that does 2 different things depending on
if a CHOP is connected to the CPlusPlus CHOPs input or not.
The example is timesliced, which is the more complex way of working.

If an input is connected the node will output the same number of channels as the
input and divide the first 'N' samples in the input channel by 2. 'N' being the current
timeslice size. This is noteworthy because if the input isn't changing then the output
will look wierd since depending on the timeslice size some number of the first samples
of the input will get used.

If no input is connected then the node will output a smooth sine wave at 120hz.
*/

enum Param
{
	P_CHUCKID,
	P_NUM
};

struct EffectData
{
	struct Data
	{
		float p[P_NUM];
		t_CKINT myId;
		bool initialized;
	};
	union
	{
		Data data;
		unsigned char pad[(sizeof(Data) + 15) & ~15]; // This entire structure must be a multiple of 16 bytes (and and instance 16 byte aligned) for PS3 SPU DMA requirements
	};
};


// To get more help about these functions, look at CHOP_CPlusPlusBase.h
class ChucKDesignerPlugin : public CHOP_CPlusPlusBase
{
public:
	ChucKDesignerPlugin(const OP_NodeInfo* info);
	virtual ~ChucKDesignerPlugin();

	virtual void		getGeneralInfo(CHOP_GeneralInfo*, const OP_Inputs*, void* ) override;
	virtual bool		getOutputInfo(CHOP_OutputInfo*, const OP_Inputs*, void*) override;
	virtual void		getChannelName(int32_t index, OP_String *name, const OP_Inputs*, void* reserved) override;

	virtual void		execute(CHOP_Output*,
								const OP_Inputs*,
								void* reserved) override;


	virtual int32_t		getNumInfoCHOPChans(void* reserved1) override;
	virtual void		getInfoCHOPChan(int index,
										OP_InfoCHOPChan* chan,
										void* reserved1) override;

	virtual bool		getInfoDATSize(OP_InfoDATSize* infoSize, void* resereved1) override;
	virtual void		getInfoDATEntries(int32_t index,
											int32_t nEntries,
											OP_InfoDATEntries* entries,
											void* reserved1) override;

	virtual void		setupParameters(OP_ParameterManager* manager, void *reserved1) override;
	virtual void		pulsePressed(const char* name, void* reserved1) override;

private:

	// We don't need to store this pointer, but we do for the example.
	// The OP_NodeInfo class store information about the node that's using
	// this instance of the class (like its name).
	const OP_NodeInfo*	myNodeInfo;

	// In this example this value will be incremented each time the execute()
	// function is called, then passes back to the CHOP 
	int32_t				myExecuteCount;


	double				myOffset;

	std::map< unsigned int, ChucK* > chuck_instances;
	std::map< unsigned int, EffectData::Data* > data_instances;
	std::string chuck_global_data_dir;

    bool runChuckCode(unsigned int chuckID, const char* code);
    bool runChuckCodeWithReplacementDac(unsigned int chuckID, const char* code, const char* replacement_dac);
    bool runChuckFile(unsigned int chuckID, const char* filename);
    bool runChuckFileWithReplacementDac(unsigned int chuckID, const char* filename, const char* replacement_dac);
    bool runChuckFileWithArgs(unsigned int chuckID, const char* filename, const char* args);
    bool runChuckFileWithArgsWithReplacementDac(unsigned int chuckID, const char* filename, const char* args, const char* replacement_dac);

    bool setChuckInt(unsigned int chuckID, const char* name, t_CKINT val);
    bool getChuckInt(unsigned int chuckID, const char* name, void (*callback)(t_CKINT));

    bool setChuckFloat(unsigned int chuckID, const char* name, t_CKFLOAT val);
    bool getChuckFloat(unsigned int chuckID, const char* name, void (*callback)(t_CKFLOAT));

    bool setChuckString(unsigned int chuckID, const char* name, const char* val);
    bool getChuckString(unsigned int chuckID, const char* name, void (*callback)(const char*));

    bool signalChuckEvent(unsigned int chuckID, const char* name);
    bool broadcastChuckEvent(unsigned int chuckID, const char* name);
    bool listenForChuckEventOnce(unsigned int chuckID, const char* name, void (*callback)(void));
    bool startListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(void));
    bool stopListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(void));

    bool getGlobalUGenSamples(unsigned int chuckID, const char* name, SAMPLE* buffer, int numSamples);

    // int array methods
    bool setGlobalIntArray(unsigned int chuckID, const char* name, t_CKINT arrayValues[], unsigned int numValues);
    bool getGlobalIntArray(unsigned int chuckID, const char* name, void (*callback)(t_CKINT[], t_CKUINT));
    bool setGlobalIntArrayValue(unsigned int chuckID, const char* name, unsigned int index, t_CKINT value);
    bool getGlobalIntArrayValue(unsigned int chuckID, const char* name, unsigned int index, void (*callback)(t_CKINT));
    bool setGlobalAssociativeIntArrayValue(unsigned int chuckID, const char* name, char* key, t_CKINT value);
    bool getGlobalAssociativeIntArrayValue(unsigned int chuckID, const char* name, char* key, void (*callback)(t_CKINT));
    // TODO: set entire dict, add to dict in batch; get entire dict

    // float array methods
    bool setGlobalFloatArray(unsigned int chuckID, const char* name, t_CKFLOAT arrayValues[], unsigned int numValues);
    bool getGlobalFloatArray(unsigned int chuckID, const char* name, void (*callback)(t_CKFLOAT[], t_CKUINT));
    bool setGlobalFloatArrayValue(unsigned int chuckID, const char* name, unsigned int index, t_CKFLOAT value);
    bool getGlobalFloatArrayValue(unsigned int chuckID, const char* name, unsigned int index, void (*callback)(t_CKFLOAT));
    bool setGlobalAssociativeFloatArrayValue(unsigned int chuckID, const char* name, char* key, t_CKFLOAT value);
    bool getGlobalAssociativeFloatArrayValue(unsigned int chuckID, const char* name, char* key, void (*callback)(t_CKFLOAT));


    bool initChuckInstance(unsigned int chuckID, unsigned int sampleRate);
    bool clearChuckInstance(unsigned int chuckID);
    bool clearGlobals(unsigned int chuckID);
    bool cleanupChuckInstance(unsigned int chuckID);
    bool chuckManualAudioCallback(unsigned int chuckID, float* inBuffer, float* outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels);
    void cleanRegisteredChucks();

    bool setChoutCallback(unsigned int chuckID, void (*callback)(const char*));
    bool setCherrCallback(unsigned int chuckID, void (*callback)(const char*));
    bool setStdoutCallback(void (*callback)(const char*));
    bool setStderrCallback(void (*callback)(const char*));

    bool setDataDir(const char* dir);

    bool setLogLevel(unsigned int level);

    bool RegisterChuckData(EffectData::Data* data, const unsigned int id);

};
