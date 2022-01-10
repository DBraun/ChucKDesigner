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

#pragma once

#include "CHOP_CPlusPlusBase.h"
using namespace TD;

#include "chuck.h"
#include "Plugin_ChucK.h"

#include <functional>
#include <map>

// To get more help about these functions, look at CHOP_CPlusPlusBase.h
class ChucKDesignerCHOP : public CHOP_CPlusPlusBase
{
public:
    ChucKDesignerCHOP(const OP_NodeInfo* info);
	virtual ~ChucKDesignerCHOP();

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

    virtual void getErrorString(OP_String* error, void* reserved1);
    
    void
    setGlobalFloat(const char* name, double val)
    {
        ChucK_For_TouchDesigner::setChuckFloat(m_chuckID, name, val);
    }
    
    void
    setGlobalInt(const char* name, int val)
    {
        ChucK_For_TouchDesigner::setChuckInt(m_chuckID, name, val);
    }
    
    void
    setGlobalString(const char* name, const char* val)
    {
        ChucK_For_TouchDesigner::setChuckString(m_chuckID, name, val);
    }
    
    void
    setGlobalIntArray(const char* name, t_CKINT arrayValues[], unsigned int numValues)
    {
        ChucK_For_TouchDesigner::setGlobalIntArray(m_chuckID, name, arrayValues, numValues);
    }
    
    void
    setGlobalFloatArray(const char* name, t_CKFLOAT arrayValues[], unsigned int numValues)
    {
        ChucK_For_TouchDesigner::setGlobalFloatArray(m_chuckID, name, arrayValues, numValues);
    }

private:

	// We don't need to store this pointer, but we do for the example.
	// The OP_NodeInfo class store information about the node that's using
	// this instance of the class (like its name).
	const OP_NodeInfo*	myNodeInfo;

	// In this example this value will be incremented each time the execute()
	// function is called, then passes back to the CHOP 
	int32_t				myExecuteCount;

	void reset();

    bool myStatus = false;
    bool needCompile = false;

    float* inChucKBuffer = new float[1];
    float* outChucKBuffer = new float[1];

    std::stringstream myError;

    int m_chuckID = 0;
	int m_inChannels = 0;
	int m_outChannels = 0;
};
