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

#include <functional>
#include <map>
#include <mutex>
#include <set>

// To get more help about these functions, look at CHOP_CPlusPlusBase.h
class ChucKListenerCHOP : public CHOP_CPlusPlusBase
{
public:
    ChucKListenerCHOP(const OP_NodeInfo* info);
    virtual ~ChucKListenerCHOP();

    virtual void		getGeneralInfo(CHOP_GeneralInfo*, const OP_Inputs*, void*) override;
    virtual bool		getOutputInfo(CHOP_OutputInfo*, const OP_Inputs*, void*) override;
    virtual void		getChannelName(int32_t index, OP_String* name, const OP_Inputs*, void* reserved) override;

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

    virtual void		setupParameters(OP_ParameterManager* manager, void* reserved1) override;
    virtual void		pulsePressed(const char* name, void* reserved1) override;

    virtual void getErrorString(OP_String* error, void* reserved1) override;

private:

    // We don't need to store this pointer, but we do for the example.
    // The OP_NodeInfo class store information about the node that's using
    // this instance of the class (like its name).
    const OP_NodeInfo* myNodeInfo;

    // In this example this value will be incremented each time the execute()
    // function is called, then passes back to the CHOP 
    int32_t				myExecuteCount;

    bool myStatus = false;

    std::stringstream myError;
    std::vector<std::string> myFloatVarNames;
    std::vector<std::string> myIntVarNames;
    std::vector<std::string> myStringVarNames;
    std::vector<std::string> myFloatArrayVarNames;
    std::vector<std::string> myIntArrayVarNames;
    std::set<std::string> myEventVarNames;

    std::vector<std::string> myAssociativeFloatArrayVarNames;
    std::vector<std::string> myAssociativeIntArrayVarNames;

    unsigned int m_chuckID = -1;
};
