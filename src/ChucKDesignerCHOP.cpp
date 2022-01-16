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

#include "ChucKDesignerCHOP.h"

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <assert.h>
#include "chuck_globals.h"

#ifdef _WIN32
    #include <Python.h>
    #include <structmember.h>
    #include <unicodeobject.h>
#else
    #include <Python/Python.h>
    #include <Python/structmember.h>
    #include <Python/unicodeobject.h>
#endif

#define FAIL_IN_CUSTOM_OPERATOR_METHOD Py_INCREF(Py_None);return Py_None;

static PyObject*
pySetGlobalFloat(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;
        PyObject* val;

        if (!PyArg_UnpackTuple(args, "ref", 2, 2, &name, &val)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        PyObject * ascii_mystring=PyUnicode_AsASCIIString(name);
        
        const char* castName = PyBytes_AsString(ascii_mystring);
        t_CKFLOAT castVal = PyFloat_AsDouble(val);
        
        inst->setGlobalFloat(castName, castVal);
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pySetGlobalInt(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;
        PyObject* val;

        if (!PyArg_UnpackTuple(args, "ref", 2, 2, &name, &val)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        PyObject* ascii_mystring = PyUnicode_AsASCIIString(name);

        const char* castName = PyBytes_AsString(ascii_mystring);
        t_CKINT castVal = _PyLong_AsInt(val);

        inst->setGlobalInt(castName, castVal);
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pySetGlobalString(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;
        PyObject* val;

        if (!PyArg_UnpackTuple(args, "ref", 2, 2, &name, &val)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        const char* castName = PyBytes_AsString(PyUnicode_AsASCIIString(name));
        const char* castVal = PyBytes_AsString(PyUnicode_AsASCIIString(val));

        inst->setGlobalString(castName, castVal);
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pySetGlobalFloatArray(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;
        PyObject* vals;

        if (!PyArg_UnpackTuple(args, "ref", 2, 2, &name, &vals)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        PyObject* ascii_mystring = PyUnicode_AsASCIIString(name);

        const char* castName = PyBytes_AsString(ascii_mystring);

        Py_ssize_t numValues = PyList_Size(vals);

        auto nums = new t_CKFLOAT[numValues];
        for (int i = 0; i < numValues; i++) {
            nums[i] = PyFloat_AsDouble(PyList_GetItem(vals, i));;
        }

        inst->setGlobalFloatArray(castName, nums, numValues);
        delete[] nums;
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pySetGlobalIntArray(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;
        PyObject* vals;

        if (!PyArg_UnpackTuple(args, "ref", 2, 2, &name, &vals)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        PyObject* ascii_mystring = PyUnicode_AsASCIIString(name);

        const char* castName = PyBytes_AsString(ascii_mystring);

        Py_ssize_t numValues = PyList_Size(vals);

        auto nums = new t_CKINT[numValues];
        for (int i = 0; i < numValues; i++) {
            nums[i] = PyLong_AsLongLong(PyList_GetItem(vals, i));;
        }

        inst->setGlobalIntArray(castName, nums, numValues);
        delete[] nums;
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pySetGlobalFloatArrayValue(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;
        PyObject* index;
        PyObject* value;

        if (!PyArg_UnpackTuple(args, "ref", 3, 3, &name, &index, &value)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        const char* castName = PyBytes_AsString(PyUnicode_AsASCIIString(name));

        int castIndex = _PyLong_AsInt(index);
        t_CKFLOAT castValue = PyFloat_AsDouble(value);

        inst->setGlobalFloatArrayValue(castName, castIndex, castValue);
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pySetGlobalIntArrayValue(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;
        PyObject* index;
        PyObject* value;

        if (!PyArg_UnpackTuple(args, "ref", 3, 3, &name, &index, &value)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        const char* castName = PyBytes_AsString(PyUnicode_AsASCIIString(name));

        int castIndex = _PyLong_AsInt(index);
        t_CKINT castValue = PyLong_AsLongLong(value);

        inst->setGlobalIntArrayValue(castName, castIndex, castValue);
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pySetGlobalAssociativeFloatArrayValue(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;
        PyObject* key;
        PyObject* value;

        if (!PyArg_UnpackTuple(args, "ref", 3, 3, &name, &key, &value)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        const char* castName = PyBytes_AsString(PyUnicode_AsASCIIString(name));

        char* castKey = PyBytes_AsString(PyUnicode_AsASCIIString(key));

        t_CKFLOAT castValue = PyFloat_AsDouble(value);

        inst->setGlobalAssociativeFloatArrayValue(castName, castKey, castValue);
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pySetGlobalAssociativeIntArrayValue(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;
        PyObject* key;
        PyObject* value;

        if (!PyArg_UnpackTuple(args, "ref", 3, 3, &name, &key, &value)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        const char* castName = PyBytes_AsString(PyUnicode_AsASCIIString(name));

        char* castKey = PyBytes_AsString(PyUnicode_AsASCIIString(key));

        t_CKINT castValue = PyLong_AsLongLong(value);

        inst->setGlobalAssociativeIntArrayValue(castName, castKey, castValue);
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pyBroadcastChuckEvent(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* name;

        if (!PyArg_UnpackTuple(args, "ref", 1, 1, &name)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        const char* castName = PyBytes_AsString(PyUnicode_AsASCIIString(name));

        inst->broadcastChuckEvent(castName);
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyObject*
pySetLogLevel(PyObject* self, PyObject* args, void*)
{
    PY_Struct* me = (PY_Struct*)self;

    PY_GetInfo info;
    // We don't want to cook the node before we set this, since it doesn't depend on its current state
    info.autoCook = false; // todo: which value to use?
    ChucKDesignerCHOP* inst = (ChucKDesignerCHOP*)me->context->getNodeInstance(info);
    // It's possible the instance will be nullptr, such as if the node has been deleted
    // while the Python class is still being held on and used elsewhere.
    if (inst)
    {
        PyObject* level;

        if (!PyArg_UnpackTuple(args, "ref", 1, 1, &level)) {
            // error
            FAIL_IN_CUSTOM_OPERATOR_METHOD
        }

        inst->setLogLevel(_PyLong_AsInt(level));
        // Make the node dirty so it will cook an output a newly reset filter when asked next
        me->context->makeNodeDirty();
    }

    // We need to inc-ref the None object if we are going to return it.
    FAIL_IN_CUSTOM_OPERATOR_METHOD
}

static PyMethodDef methods[] =
{
    {"set_float", (PyCFunction)pySetGlobalFloat, METH_VARARGS, "Set a ChucK global float variable."},
    {"set_int", (PyCFunction)pySetGlobalInt, METH_VARARGS, "Set a ChucK global float variable."},
    {"set_string", (PyCFunction)pySetGlobalString, METH_VARARGS, "Set a ChucK global string variable."},
    
    {"set_float_array", (PyCFunction)pySetGlobalFloatArray, METH_VARARGS, "Set a ChucK global float array variable."},
    {"set_int_array", (PyCFunction)pySetGlobalIntArray, METH_VARARGS, "Set a ChucK global int array variable."},
    
    {"set_float_array_value", (PyCFunction)pySetGlobalFloatArrayValue, METH_VARARGS, "Set a single value in a ChucK global float array variable."},
    {"set_int_array_value", (PyCFunction)pySetGlobalIntArrayValue, METH_VARARGS, "Set a single value in a ChucK global int array variable."},

    {"set_associative_float_array_value", (PyCFunction)pySetGlobalAssociativeFloatArrayValue, METH_VARARGS, "Set a single value in an associative ChucK global float array variable."},
    {"set_associative_float_int_value", (PyCFunction)pySetGlobalAssociativeIntArrayValue, METH_VARARGS, "Set a single value in an associative ChucK global int array variable."},

    {"broadcast_event", (PyCFunction)pyBroadcastChuckEvent, METH_VARARGS, "Broadcast an event to ChucK."},

    {"set_log_level", (PyCFunction)pySetLogLevel, METH_VARARGS, "Set ChucK's log level."},

    {0}
};


extern "C"
{

    DLLEXPORT
        void
        FillCHOPPluginInfo(CHOP_PluginInfo* info)
    {
        // Always set this to CHOPCPlusPlusAPIVersion.
        info->apiVersion = CHOPCPlusPlusAPIVersion;

        // The opType is the unique name for this CHOP. It must start with a 
        // capital A-Z character, and all the following characters must lower case
        // or numbers (a-z, 0-9)
        info->customOPInfo.opType->setString("Chuckaudio");

        // The opLabel is the text that will show up in the OP Create Dialog
        info->customOPInfo.opLabel->setString("ChucK Audio");
        info->customOPInfo.opIcon->setString("CKA");

        // Information about the author of this OP
        info->customOPInfo.authorName->setString("David Braun");
        info->customOPInfo.authorEmail->setString("github.com/DBraun");

        info->customOPInfo.minInputs = 0;
        info->customOPInfo.maxInputs = 1;
            
        info->customOPInfo.pythonVersion->setString(PY_VERSION);
        info->customOPInfo.pythonMethods = methods;
        //info->customOPInfo.pythonGetSets = getSets; // todo:
            
        ChucK_For_TouchDesigner::setStderrCallback(
            [](const char* text) {
                std::cerr << text << std::endl;
            }
        );

        ChucK_For_TouchDesigner::setStdoutCallback(
            [](const char* text) {
                std::cout << text << std::endl;
            }
        );
    }

    DLLEXPORT
        CHOP_CPlusPlusBase*
        CreateCHOPInstance(const OP_NodeInfo* info)
    {
        // Return a new instance of your class every time this is called.
        // It will be called once per CHOP that is using the .dll
        return new ChucKDesignerCHOP(info);
    }

    DLLEXPORT
        void
        DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
    {
        // Delete the instance here, this will be called when
        // Touch is shutting down, when the CHOP using that instance is deleted, or
        // if the CHOP loads a different DLL
        delete (ChucKDesignerCHOP*)instance;
    }

};


ChucKDesignerCHOP::ChucKDesignerCHOP(const OP_NodeInfo* info) : myNodeInfo(info)
{
    m_chuckID = ChucK_For_TouchDesigner::getNextValidID(myNodeInfo->opId);

	myExecuteCount = 0;
}


ChucKDesignerCHOP::~ChucKDesignerCHOP()
{
    reset();
}


void
ChucKDesignerCHOP::getGeneralInfo(CHOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = true;
    ginfo->timeslice = true;
}


bool
ChucKDesignerCHOP::getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs* inputs, void* reserved1)
{	
    ChucK_For_TouchDesigner::getInstanceInfo(m_chuckID, info->numChannels, info->numSamples, info->sampleRate);

	return true;
}


void
ChucKDesignerCHOP::getChannelName(int32_t index, OP_String *name, const OP_Inputs* inputs, void* reserved1)
{
    std::stringstream ss;
    ss << "chan" << (index + 1);
    name->setString(ss.str().c_str());
}


void
ChucKDesignerCHOP::reset() {
    ChucK_For_TouchDesigner::clearGlobals(m_chuckID);
    ChucK_For_TouchDesigner::clearChuckInstance(m_chuckID);
    ChucK_For_TouchDesigner::cleanupChuckInstance(m_chuckID, myNodeInfo->opId);

    m_chuckID = ChucK_For_TouchDesigner::getNextValidID(myNodeInfo->opId);
}

void
ChucKDesignerCHOP::execute(CHOP_Output* output,
							  const OP_Inputs* inputs,
							  void* reserved)
{
	myExecuteCount++;

    if (needCompile) {

        string globalDir = inputs->getParString("Workingdirectory");

        double sample_rate = inputs->getParDouble("Samplerate");

        delete[] inChucKBuffer;
        delete[] outChucKBuffer;

        m_inChannels = inputs->getParInt("Inchannels");
        m_outChannels = inputs->getParInt("Outchannels");

        inChucKBuffer = new float[CHUCKDESIGNERCHOP_BUFFER_SIZE * m_inChannels];
        outChucKBuffer = new float[CHUCKDESIGNERCHOP_BUFFER_SIZE * m_outChannels];

        memset(inChucKBuffer, 0.f, sizeof(float) * CHUCKDESIGNERCHOP_BUFFER_SIZE * m_inChannels);
        memset(outChucKBuffer, 0.f, sizeof(float) * CHUCKDESIGNERCHOP_BUFFER_SIZE * m_outChannels);

        //m_chuckID = ChucK_For_TouchDesigner::getNextValidID(myNodeInfo->opId);

        ChucK_For_TouchDesigner::initChuckInstance(m_chuckID, sample_rate, m_inChannels, m_outChannels, globalDir);

        const OP_DATInput* input = inputs->getParDAT("Code");
        const char* code = *(input->cellData);

        bool result = ChucK_For_TouchDesigner::runChuckCode(m_chuckID, code);
        if (result) {
            // clear any existing error
            myError.str("");
        }
        else {
            myError.str("ChucK code did not compile correctly.");
        }

        needCompile = false;
    }

    const OP_CHOPInput* Globalfloat_CHOPInput = inputs->getParCHOP("Globalfloat");
    if (Globalfloat_CHOPInput) {
        for (size_t chanIndex = 0; chanIndex < Globalfloat_CHOPInput->numChannels; chanIndex++)
        {
            const char* chanName = Globalfloat_CHOPInput->getChannelName(chanIndex);
            float chanVal = Globalfloat_CHOPInput->channelData[chanIndex][0];
            ChucK_For_TouchDesigner::setChuckFloat(m_chuckID, chanName, chanVal);
        }
    }

    const float** inBuffer = nullptr;
    int inBufferNumChannels = 0;
    int inBufferNumSamples = 0;

    if (auto chopInput = inputs->getInputCHOP(0)) {
        inBuffer = chopInput->channelData;
        inBufferNumChannels = chopInput->numChannels;
        inBufferNumSamples = chopInput->numSamples;
    }
    
    bool result = ChucK_For_TouchDesigner::processBlock(m_chuckID, inBuffer, inBufferNumChannels, inBufferNumSamples, inChucKBuffer, outChucKBuffer, output->channels, output->numSamples, output->numChannels);
    if (!result) {
        // fill zeros
        for (int chan = 0; chan < output->numChannels; chan++) {
            memset(output->channels[chan], 0.f, sizeof(float) * output->numSamples);
        }
    }

}

void
ChucKDesignerCHOP::getErrorString(OP_String* error, void* reserved1) {

    if (!myStatus) {
        error->setString(myError.str().c_str());
    }
}

int32_t
ChucKDesignerCHOP::getNumInfoCHOPChans(void * reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 2;
}

void
ChucKDesignerCHOP::getInfoCHOPChan(int32_t index,
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
        chan->name->setString("chuck_id");
        chan->value = (float)m_chuckID;
    }
}

bool		
ChucKDesignerCHOP::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 1;
	infoSize->cols = 2;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void
ChucKDesignerCHOP::getInfoDATEntries(int32_t index,
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
ChucKDesignerCHOP::setupParameters(OP_ParameterManager* manager, void *reserved1)
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

    // Num in channels
    {
        OP_NumericParameter	np;

        np.name = "Inchannels";
        np.label = "In Channels";
        np.defaultValues[0] = 2;
        np.minSliders[0] = 2;
        np.maxSliders[0] = 2;
        np.minValues[0] = 0;
        np.clampMins[0] = true;

        OP_ParAppendResult res = manager->appendInt(np);
        assert(res == OP_ParAppendResult::Success);
    }

    // Num out channels
    {
        OP_NumericParameter	np;

        np.name = "Outchannels";
        np.label = "Out Channels";
        np.defaultValues[0] = 2;
        np.minSliders[0] = 2;
        np.maxSliders[0] = 2;
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

    // Working directory path
    {
        OP_NumericParameter	np;
        OP_StringParameter sp;

        sp.name = "Workingdirectory";
        sp.label = "Working Directory";

        OP_ParAppendResult res = manager->appendFolder(sp);
        assert(res == OP_ParAppendResult::Success);
    }

    // pulse
    {
        OP_NumericParameter	np;

        np.name = "Addchuckcode";
        np.label = "Add ChucK Code";

        OP_ParAppendResult res = manager->appendPulse(np);
        assert(res == OP_ParAppendResult::Success);
    }

    {
        OP_NumericParameter	np;

        np.name = "Replacechuckcode";
        np.label = "Replace ChucK Code";

        OP_ParAppendResult res = manager->appendPulse(np);
        assert(res == OP_ParAppendResult::Success);
    }

    // Reset
    {
        OP_NumericParameter	np;

        np.name = "Reset";
        np.label = "Reset";

        OP_ParAppendResult res = manager->appendPulse(np);
        assert(res == OP_ParAppendResult::Success);
    }

    // Global In Float CHOP
    {
        OP_StringParameter	sp;

        sp.name = "Globalfloat";
        sp.label = "Global Float";

        sp.defaultValue = "";

        OP_ParAppendResult res = manager->appendCHOP(sp);
        assert(res == OP_ParAppendResult::Success);
    }
}

void 
ChucKDesignerCHOP::pulsePressed(const char* name, void* reserved1)
{
	if (!strcmp(name, "Addchuckcode"))
	{
        needCompile = true;
	}

    if (!strcmp(name, "Replacechuckcode"))
    {
        ChucKDesignerCHOP::reset();
        needCompile = true;
    }

    if (!strcmp(name, "Reset"))
    {
        ChucKDesignerCHOP::reset();
    }
}
