#pragma once

#include "chuck.h"

#define CHUCKDESIGNERCHOP_BUFFER_SIZE 256

#ifdef WIN32

#ifdef CHUCKDESIGNERSHARED_EXPORTS
#define CHUCKDESIGNERSHARED_API __declspec(dllexport)
#else
#define CHUCKDESIGNERSHARED_API __declspec(dllimport)
#endif

#else
#define CHUCKDESIGNERSHARED_API
#endif


extern "C" {

    namespace ChucK_For_TouchDesigner {

        CHUCKDESIGNERSHARED_API bool runChuckCode(unsigned int chuckID, const char* code);
        CHUCKDESIGNERSHARED_API bool runChuckCodeWithReplacementDac(unsigned int chuckID, const char* code, const char* replacement_dac);
        CHUCKDESIGNERSHARED_API bool runChuckFile(unsigned int chuckID, const char* filename);
        CHUCKDESIGNERSHARED_API bool runChuckFileWithReplacementDac(unsigned int chuckID, const char* filename, const char* replacement_dac);
        CHUCKDESIGNERSHARED_API bool runChuckFileWithArgs(unsigned int chuckID, const char* filename, const char* args);
        CHUCKDESIGNERSHARED_API bool runChuckFileWithArgsWithReplacementDac(unsigned int chuckID, const char* filename, const char* args, const char* replacement_dac);

        CHUCKDESIGNERSHARED_API bool setChuckInt(unsigned int chuckID, const char* name, t_CKINT val);
        CHUCKDESIGNERSHARED_API bool getChuckInt(unsigned int chuckID, const char* name, void (*callback)(t_CKINT));
        CHUCKDESIGNERSHARED_API bool getNamedChuckInt(unsigned int chuckID, const char* name, void (*callback)(const char*, t_CKINT));
        CHUCKDESIGNERSHARED_API bool getChuckIntWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, t_CKINT));


        CHUCKDESIGNERSHARED_API bool setChuckFloat(unsigned int chuckID, const char* name, t_CKFLOAT val);
        CHUCKDESIGNERSHARED_API bool getChuckFloat(unsigned int chuckID, const char* name, void (*callback)(t_CKFLOAT));
        CHUCKDESIGNERSHARED_API bool getNamedChuckFloat(unsigned int chuckID, const char* name, void (*callback)(const char*, t_CKFLOAT));
        CHUCKDESIGNERSHARED_API bool getChuckFloatWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, t_CKFLOAT));

        CHUCKDESIGNERSHARED_API bool setChuckString(unsigned int chuckID, const char* name, const char* val);
        CHUCKDESIGNERSHARED_API bool getChuckString(unsigned int chuckID, const char* name, void (*callback)(const char*));
        CHUCKDESIGNERSHARED_API bool getNamedChuckString(unsigned int chuckID, const char* name, void (*callback)(const char*, const char*));
        CHUCKDESIGNERSHARED_API bool getChuckStringWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, const char*));

        CHUCKDESIGNERSHARED_API bool signalChuckEvent(unsigned int chuckID, const char* name);
        CHUCKDESIGNERSHARED_API bool broadcastChuckEvent(unsigned int chuckID, const char* name);
        CHUCKDESIGNERSHARED_API bool listenForChuckEventOnce(unsigned int chuckID, const char* name, void (*callback)(void));
        CHUCKDESIGNERSHARED_API bool listenForNamedChuckEventOnce(unsigned int chuckID, const char* name, void (*callback)(const char*));
        CHUCKDESIGNERSHARED_API bool listenForChuckEventOnceWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void (*callback)(t_CKINT));
        CHUCKDESIGNERSHARED_API bool startListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(void));
        CHUCKDESIGNERSHARED_API bool startListeningForNamedChuckEvent(unsigned int chuckID, const char* name, void (*callback)(const char*));
        CHUCKDESIGNERSHARED_API bool startListeningForChuckEventWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void (*callback)(t_CKINT));
        CHUCKDESIGNERSHARED_API bool stopListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(void));
        CHUCKDESIGNERSHARED_API bool stopListeningForNamedChuckEvent(unsigned int chuckID, const char* name, void (*callback)(const char*));
        CHUCKDESIGNERSHARED_API bool stopListeningForChuckEventWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void (*callback)(t_CKINT));

        CHUCKDESIGNERSHARED_API bool getGlobalUGenSamples(unsigned int chuckID, const char* name, SAMPLE* buffer, int numSamples);

        // int array methods
        CHUCKDESIGNERSHARED_API bool setGlobalIntArray(unsigned int chuckID, const char* name, t_CKINT arrayValues[], unsigned int numValues);
        CHUCKDESIGNERSHARED_API bool getGlobalIntArray(unsigned int chuckID, const char* name, void (*callback)(t_CKINT[], t_CKUINT));
        CHUCKDESIGNERSHARED_API bool getNamedGlobalIntArray(unsigned int chuckID, const char* name, void (*callback)(const char*, t_CKINT[], t_CKUINT));
        CHUCKDESIGNERSHARED_API bool getGlobalIntArrayWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, t_CKINT[], t_CKUINT));
        CHUCKDESIGNERSHARED_API bool setGlobalIntArrayValue(unsigned int chuckID, const char* name, unsigned int index, t_CKINT value);
        CHUCKDESIGNERSHARED_API bool getGlobalIntArrayValue(unsigned int chuckID, const char* name, unsigned int index, void (*callback)(t_CKINT));
        CHUCKDESIGNERSHARED_API bool getNamedGlobalIntArrayValue(unsigned int chuckID, const char* name, unsigned int index, void (*callback)(const char*, t_CKINT));
        CHUCKDESIGNERSHARED_API bool getGlobalIntArrayValueWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, unsigned int index, void (*callback)(t_CKINT, t_CKINT));
        CHUCKDESIGNERSHARED_API bool setGlobalAssociativeIntArrayValue(unsigned int chuckID, const char* name, char* key, t_CKINT value);
        CHUCKDESIGNERSHARED_API bool getGlobalAssociativeIntArrayValue(unsigned int chuckID, const char* name, char* key, void (*callback)(t_CKINT));
        CHUCKDESIGNERSHARED_API bool getNamedGlobalAssociativeIntArrayValue(unsigned int chuckID, const char* name, char* key, void (*callback)(const char*, t_CKINT));
        CHUCKDESIGNERSHARED_API bool getGlobalAssociativeIntArrayValueWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, char* key, void (*callback)(t_CKINT, t_CKINT));
        // TODO: set entire dict, add to dict in batch; get entire dict

        // float array methods
        CHUCKDESIGNERSHARED_API bool setGlobalFloatArray(unsigned int chuckID, const char* name, t_CKFLOAT arrayValues[], unsigned int numValues);
        CHUCKDESIGNERSHARED_API bool getGlobalFloatArray(unsigned int chuckID, const char* name, void (*callback)(t_CKFLOAT[], t_CKUINT));
        CHUCKDESIGNERSHARED_API bool getNamedGlobalFloatArray(unsigned int chuckID, const char* name, void (*callback)(const char*, t_CKFLOAT[], t_CKUINT));
        CHUCKDESIGNERSHARED_API bool getGlobalFloatArrayWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, t_CKFLOAT[], t_CKUINT));
        CHUCKDESIGNERSHARED_API bool setGlobalFloatArrayValue(unsigned int chuckID, const char* name, unsigned int index, t_CKFLOAT value);
        CHUCKDESIGNERSHARED_API bool getGlobalFloatArrayValue(unsigned int chuckID, const char* name, unsigned int index, void (*callback)(t_CKFLOAT));
        CHUCKDESIGNERSHARED_API bool getNamedGlobalFloatArrayValue(unsigned int chuckID, const char* name, unsigned int index, void (*callback)(const char*, t_CKFLOAT));
        CHUCKDESIGNERSHARED_API bool getGlobalFloatArrayValueWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, unsigned int index, void (*callback)(t_CKINT, t_CKFLOAT));
        CHUCKDESIGNERSHARED_API bool setGlobalAssociativeFloatArrayValue(unsigned int chuckID, const char* name, char* key, t_CKFLOAT value);
        CHUCKDESIGNERSHARED_API bool getGlobalAssociativeFloatArrayValue(unsigned int chuckID, const char* name, char* key, void (*callback)(t_CKFLOAT));
        CHUCKDESIGNERSHARED_API bool getNamedGlobalAssociativeFloatArrayValue(unsigned int chuckID, const char* name, char* key, void (*callback)(const char*, t_CKFLOAT));
        CHUCKDESIGNERSHARED_API bool getGlobalAssociativeFloatArrayValueWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, char* key, void (*callback)(t_CKINT, t_CKFLOAT));


        CHUCKDESIGNERSHARED_API bool initChuckInstance(unsigned int chuckID, unsigned int sampleRate, unsigned int numInChannels, unsigned int numOutChannels, string globalDir);
        CHUCKDESIGNERSHARED_API bool clearChuckInstance(unsigned int chuckID);
        CHUCKDESIGNERSHARED_API bool clearGlobals(unsigned int chuckID);
        CHUCKDESIGNERSHARED_API bool cleanupChuckInstance(unsigned int chuckID, unsigned int opId);
        CHUCKDESIGNERSHARED_API bool chuckManualAudioCallback(unsigned int chuckID, float* inBuffer, float* outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels);
        CHUCKDESIGNERSHARED_API void cleanRegisteredChucks();

        CHUCKDESIGNERSHARED_API bool setChoutCallback(unsigned int chuckID, void (*callback)(const char*));
        CHUCKDESIGNERSHARED_API bool setCherrCallback(unsigned int chuckID, void (*callback)(const char*));
        CHUCKDESIGNERSHARED_API bool setStdoutCallback(void (*callback)(const char*));
        CHUCKDESIGNERSHARED_API bool setStderrCallback(void (*callback)(const char*));

        CHUCKDESIGNERSHARED_API bool setLogLevel(unsigned int level);

        CHUCKDESIGNERSHARED_API unsigned int getNextValidID(uint32_t opID);

        CHUCKDESIGNERSHARED_API unsigned int getChucKIDForOpID(uint32_t opID);

        CHUCKDESIGNERSHARED_API bool getInstanceInfo(unsigned int chuckID, int& numChannels, int& numSamples, float& sampleRate);

        CHUCKDESIGNERSHARED_API bool processBlock(unsigned int chuckID, const float** inBuffer, int inBufferNumChannels, int inBufferNumSamples, float* inChucKBuffer, float* outChucKBuffer, float** outBuffer, int numOutSamples, int numOutChannels);
        
        CHUCKDESIGNERSHARED_API t_CKFLOAT getFloat(const char* varStr);
        CHUCKDESIGNERSHARED_API t_CKINT getInt(const char* varStr);
        CHUCKDESIGNERSHARED_API const char* getString(const char* varStr);
        CHUCKDESIGNERSHARED_API t_CKFLOAT* getFloatArray(const char* varName, int& numItems);
        CHUCKDESIGNERSHARED_API t_CKINT* getIntArray(const char* varName, int& numItems);

        CHUCKDESIGNERSHARED_API void sharedFloatCallback(const char* varName, t_CKFLOAT val);
        CHUCKDESIGNERSHARED_API void sharedIntCallback(const char* varName, t_CKINT val);
        CHUCKDESIGNERSHARED_API void sharedStringCallback(const char* varName, const char* val);

        CHUCKDESIGNERSHARED_API void sharedFloatArrayCallback(const char* varName, t_CKFLOAT vals[], t_CKUINT numItems);
        CHUCKDESIGNERSHARED_API void sharedIntArrayCallback(const char* varName, t_CKINT vals[], t_CKUINT numItems);
    }
};
