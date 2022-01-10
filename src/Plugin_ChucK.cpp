//
//  Plugin_ChucK.cpp
//  AudioPluginDemo
//
//  Created by Jack Atherton on 4/19/17.
//  Modified by David Braun for TouchDesigner on 12/19/21
//
//


#include "Plugin_ChucK.h"
#include "chuck_globals.h"

#include <iostream>
#include <map>
#ifndef WIN32
#include <unistd.h>
#endif

#include <cmath>


namespace ChucK_For_TouchDesigner
{
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

    std::map< unsigned int, ChucK* > chuck_instances;
    std::map< unsigned int, EffectData::Data* > data_instances;
    unsigned int _nextValidID = 0;
    std::map< unsigned int, unsigned int> op_ids_to_chuck_ids;
    static std::map<std::string, double> myFloatVars;
    static std::map<std::string, float> myFloatArrayVars;
    static std::map<std::string, int> myFloatArrayVarSizes;



    // C# "string" corresponds to passing char *
    CHUCKDESIGNERSHARED_API bool runChuckCode(unsigned int chuckID, const char* code)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac(FALSE, "");

        // compile it!
        return chuck_instances[chuckID]->compileCode(
            std::string(code), std::string(""));
    }


    CHUCKDESIGNERSHARED_API bool runChuckCodeWithReplacementDac(
        unsigned int chuckID, const char* code, const char* replacement_dac)
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


    CHUCKDESIGNERSHARED_API bool runChuckFile(unsigned int chuckID,
        const char* filename)
    {
        // run with empty args
        return runChuckFileWithArgs(chuckID, filename, "");
    }


    CHUCKDESIGNERSHARED_API bool runChuckFileWithArgs(unsigned int chuckID,
        const char* filename, const char* args)
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


    CHUCKDESIGNERSHARED_API bool runChuckFileWithReplacementDac(
        unsigned int chuckID, const char* filename,
        const char* replacement_dac)
    {
        // run with empty args
        return runChuckFileWithArgsWithReplacementDac(
            chuckID, filename, "", replacement_dac
        );
    }


    CHUCKDESIGNERSHARED_API bool runChuckFileWithArgsWithReplacementDac(
        unsigned int chuckID, const char* filename, const char* args,
        const char* replacement_dac)
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


    CHUCKDESIGNERSHARED_API bool setChuckInt(unsigned int chuckID, const char* name, t_CKINT val)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->setGlobalInt(name, val);
    }


    CHUCKDESIGNERSHARED_API bool getChuckInt(unsigned int chuckID, const char* name, void (*callback)(t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalInt(name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getNamedChuckInt(unsigned int chuckID, const char* name, void (*callback)(const char*, t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalInt(name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getChuckIntWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void(*callback)(t_CKINT, t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalInt(name, callbackID, callback);
    }


    CHUCKDESIGNERSHARED_API bool setChuckFloat(unsigned int chuckID, const char* name, t_CKFLOAT val)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->setGlobalFloat(name, val);
    }


    CHUCKDESIGNERSHARED_API bool getChuckFloat(unsigned int chuckID, const char* name, void (*callback)(t_CKFLOAT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalFloat(name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getNamedChuckFloat(unsigned int chuckID, const char* name, void (*callback)(const char*, t_CKFLOAT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalFloat(name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getChuckFloatWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void(*callback)(t_CKINT, t_CKFLOAT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalFloat(name, callbackID, callback);
    }


    CHUCKDESIGNERSHARED_API bool setChuckString(unsigned int chuckID, const char* name, const char* val)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->setGlobalString(name, val);
    }


    CHUCKDESIGNERSHARED_API bool getChuckString(unsigned int chuckID, const char* name, void (*callback)(const char*))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalString(name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getNamedChuckString(unsigned int chuckID, const char* name, void (*callback)(const char*, const char*))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalString(name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getChuckStringWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void(*callback)(t_CKINT, const char*))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalString(name, callbackID, callback);
    }


    CHUCKDESIGNERSHARED_API bool signalChuckEvent(unsigned int chuckID, const char* name)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->signalGlobalEvent(name);
    }


    CHUCKDESIGNERSHARED_API bool broadcastChuckEvent(unsigned int chuckID, const char* name)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->broadcastGlobalEvent(name);
    }


    CHUCKDESIGNERSHARED_API bool listenForChuckEventOnce(unsigned int chuckID, const char* name, void (*callback)(void))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->listenForGlobalEvent(
            name, callback, FALSE);
    }


    CHUCKDESIGNERSHARED_API bool listenForNamedChuckEventOnce(unsigned int chuckID, const char* name, void (*callback)(const char*))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->listenForGlobalEvent(
            name, callback, FALSE);
    }


    CHUCKDESIGNERSHARED_API bool listenForChuckEventOnceWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void(*callback)(t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->listenForGlobalEvent(
            name, callbackID, callback, FALSE);
    }


    CHUCKDESIGNERSHARED_API bool startListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(void))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->listenForGlobalEvent(
            name, callback, TRUE);
    }


    CHUCKDESIGNERSHARED_API bool startListeningForNamedChuckEvent(unsigned int chuckID, const char* name, void (*callback)(const char*))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->listenForGlobalEvent(
            name, callback, TRUE);
    }


    CHUCKDESIGNERSHARED_API bool startListeningForChuckEventWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void(*callback)(t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->listenForGlobalEvent(
            name, callbackID, callback, TRUE);
    }


    CHUCKDESIGNERSHARED_API bool stopListeningForChuckEvent(unsigned int chuckID, const char* name, void (*callback)(void))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->stopListeningForGlobalEvent(
            name, callback);
    }


    CHUCKDESIGNERSHARED_API bool stopListeningForNamedChuckEvent(unsigned int chuckID, const char* name, void (*callback)(const char*))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->stopListeningForGlobalEvent(
            name, callback);
    }


    CHUCKDESIGNERSHARED_API bool stopListeningForChuckEventWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void(*callback)(t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->stopListeningForGlobalEvent(
            name, callbackID, callback);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalUGenSamples(unsigned int chuckID,
        const char* name, SAMPLE* buffer, int numSamples)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        if (!gm->getGlobalUGenSamples(
            name, buffer, numSamples))
        {
            // failed. fill with zeroes.
            memset(buffer, 0, sizeof(SAMPLE) * numSamples);
            return false;
        }

        return true;
    }


    // int array methods
    CHUCKDESIGNERSHARED_API bool setGlobalIntArray(unsigned int chuckID,
        const char* name, t_CKINT arrayValues[], unsigned int numValues)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->setGlobalIntArray(
            name, arrayValues, numValues);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalIntArray(unsigned int chuckID,
        const char* name, void (*callback)(t_CKINT[], t_CKUINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalIntArray(
            name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getNamedGlobalIntArray(unsigned int chuckID,
        const char* name, void (*callback)(const char*, t_CKINT[], t_CKUINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalIntArray(
            name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalIntArrayWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void(*callback)(t_CKINT, t_CKINT[], t_CKUINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalIntArray(
            name, callbackID, callback);
    }


    CHUCKDESIGNERSHARED_API bool setGlobalIntArrayValue(unsigned int chuckID,
        const char* name, unsigned int index, t_CKINT value)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->setGlobalIntArrayValue(
            name, index, value);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalIntArrayValue(unsigned int chuckID,
        const char* name, unsigned int index, void (*callback)(t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalIntArrayValue(
            name, index, callback);
    }


    CHUCKDESIGNERSHARED_API bool getNamedGlobalIntArrayValue(unsigned int chuckID,
        const char* name, unsigned int index, void (*callback)(const char*, t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalIntArrayValue(
            name, index, callback);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalIntArrayValueWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, unsigned int index, void(*callback)(t_CKINT, t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalIntArrayValue(
            name, callbackID, index, callback);
    }


    CHUCKDESIGNERSHARED_API bool setGlobalAssociativeIntArrayValue(
        unsigned int chuckID, const char* name, char* key, t_CKINT value)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->setGlobalAssociativeIntArrayValue(
            name, key, value);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalAssociativeIntArrayValue(
        unsigned int chuckID, const char* name, char* key,
        void (*callback)(t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalAssociativeIntArrayValue(
            name, key, callback);
    }


    CHUCKDESIGNERSHARED_API bool getNamedGlobalAssociativeIntArrayValue(
        unsigned int chuckID, const char* name, char* key,
        void (*callback)(const char*, t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalAssociativeIntArrayValue(
            name, key, callback);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalAssociativeIntArrayValueWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, char* key, void(*callback)(t_CKINT, t_CKINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalAssociativeIntArrayValue(
            name, callbackID, key, callback);
    }


    // float array methods
    CHUCKDESIGNERSHARED_API bool setGlobalFloatArray(unsigned int chuckID,
        const char* name, t_CKFLOAT arrayValues[], unsigned int numValues)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->setGlobalFloatArray(
            name, arrayValues, numValues);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalFloatArray(unsigned int chuckID,
        const char* name, void (*callback)(t_CKFLOAT[], t_CKUINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalFloatArray(
            name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getNamedGlobalFloatArray(unsigned int chuckID,
        const char* name, void (*callback)(const char*, t_CKFLOAT[], t_CKUINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalFloatArray(
            name, callback);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalFloatArrayWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, void(*callback)(t_CKINT, t_CKFLOAT[], t_CKUINT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalFloatArray(
            name, callbackID, callback);
    }


    CHUCKDESIGNERSHARED_API bool setGlobalFloatArrayValue(unsigned int chuckID,
        const char* name, unsigned int index, t_CKFLOAT value)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->setGlobalFloatArrayValue(
            name, index, value);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalFloatArrayValue(unsigned int chuckID,
        const char* name, unsigned int index, void (*callback)(t_CKFLOAT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalFloatArrayValue(
            name, index, callback);
    }


    CHUCKDESIGNERSHARED_API bool getNamedGlobalFloatArrayValue(unsigned int chuckID,
        const char* name, unsigned int index, void (*callback)(const char*, t_CKFLOAT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalFloatArrayValue(
            name, index, callback);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalFloatArrayValueWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, unsigned int index, void(*callback)(t_CKINT, t_CKFLOAT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalFloatArrayValue(
            name, callbackID, index, callback);
    }


    CHUCKDESIGNERSHARED_API bool setGlobalAssociativeFloatArrayValue(
        unsigned int chuckID, const char* name, char* key, t_CKFLOAT value)
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->setGlobalAssociativeFloatArrayValue(
            name, key, value);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalAssociativeFloatArrayValue(
        unsigned int chuckID, const char* name, char* key,
        void (*callback)(t_CKFLOAT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalAssociativeFloatArrayValue(
            name, key, callback);
    }


    CHUCKDESIGNERSHARED_API bool getNamedGlobalAssociativeFloatArrayValue(
        unsigned int chuckID, const char* name, char* key,
        void (*callback)(const char*, t_CKFLOAT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalAssociativeFloatArrayValue(
            name, key, callback);
    }


    CHUCKDESIGNERSHARED_API bool getGlobalAssociativeFloatArrayValueWithID(unsigned int chuckID, t_CKINT callbackID, const char* name, char* key, void(*callback)(t_CKINT, t_CKFLOAT))
    {
        if (chuck_instances.count(chuckID) == 0) { return false; }
        Chuck_Globals_Manager* gm = chuck_instances[chuckID]->globals();
        if (gm == NULL) { return false; }

        return gm->getGlobalAssociativeFloatArrayValue(
            name, callbackID, key, callback);
    }


    CHUCKDESIGNERSHARED_API bool setChoutCallback(unsigned int chuckID, void (*callback)(const char*))
    {
        return chuck_instances[chuckID]->setChoutCallback(callback);
    }


    CHUCKDESIGNERSHARED_API bool setCherrCallback(unsigned int chuckID, void (*callback)(const char*))
    {
        return chuck_instances[chuckID]->setCherrCallback(callback);
    }


    CHUCKDESIGNERSHARED_API bool setStdoutCallback(void (*callback)(const char*))
    {
        return ChucK::setStdoutCallback(callback);
    }


    CHUCKDESIGNERSHARED_API bool setStderrCallback(void (*callback)(const char*))
    {
        return ChucK::setStderrCallback(callback);
    }

    CHUCKDESIGNERSHARED_API bool setLogLevel(unsigned int level)
    {
        EM_setlog(level);
        return true;
    }

    CHUCKDESIGNERSHARED_API unsigned int getNextValidID(uint32_t opID)
    {
        int nextID = ++_nextValidID;

        op_ids_to_chuck_ids[opID] = nextID;

        return nextID;
    }

    CHUCKDESIGNERSHARED_API unsigned int getChucKIDForOpID(uint32_t opID)
    {

        if (op_ids_to_chuck_ids.count(opID)) {
            return op_ids_to_chuck_ids[opID];
        }

        return -1;
    }

    CHUCKDESIGNERSHARED_API bool getInstanceInfo(unsigned int chuckID, int& numChannels, int& numSamples, float& sampleRate) {

        if (chuck_instances.count(chuckID) == 0) {
            numChannels = 0;
            numSamples = 0;
        }
        else {
            auto chuck_inst = chuck_instances[chuckID];
            auto vm = chuck_inst->vm();
            numChannels = (int)vm->m_num_dac_channels;
            sampleRate = std::fmax(1.f, vm->srate());
        }

        return true;
    }


    CHUCKDESIGNERSHARED_API bool processBlock(unsigned int chuckID, const float** inBuffer, int inBufferNumChannels, int inBufferNumSamples, float* inChucKBuffer, float* outChucKBuffer, float** outBuffer, int numOutSamples, int numOutChannels) {

        if (chuck_instances.count(chuckID) == 0) {
            return false;
        }
        ChucK* chuck = chuck_instances[chuckID];

        int numOutChans = chuck->vm()->m_num_dac_channels;
        if (numOutChans != numOutChannels) {
            return false;
        }

        int numSamples;
        int numInChannels = std::min<int>(inBufferNumChannels, (int)chuck->vm()->m_num_adc_channels);

        for (int i = 0; i < numOutSamples; i += CHUCKDESIGNERCHOP_BUFFER_SIZE) {

            // chuck->run(inbuffer, *output->channels, output->numSamples); // this doesn't work because of interleaved samples.
            // Chuck returns LRLRLRLR but for touchdesigner we want LLLLRRRR.
            // Therefore we must use an intermediate buffer
            float* inPtr = inChucKBuffer;

            numSamples = min(CHUCKDESIGNERCHOP_BUFFER_SIZE, numOutSamples - i);

            if (inBuffer) {
                for (int samp = i; samp < std::min<int>(inBufferNumSamples, i + CHUCKDESIGNERCHOP_BUFFER_SIZE); samp++) {
                    for (int chan = 0; chan < numInChannels; chan++) {
                        *(inPtr++) = inBuffer[chan][samp];
                    }
                }
            }
            float* outPtr = outChucKBuffer;

            chuck->run(inChucKBuffer, outChucKBuffer, numSamples);

            for (int samp = 0; samp < numSamples; samp++) {
                for (int chan = 0; chan < numOutChans; chan++) {
                    outBuffer[chan][i + samp] = *outPtr++;
                }
            }

        }

        return true;
    }


    CHUCKDESIGNERSHARED_API void sharedFloatCallback(const char* varName, t_CKFLOAT val) {
        myFloatVars[varName] = val;
    }

//    CHUCKDESIGNERSHARED_API void sharedFloatArrayCallback(const char* varName, t_CKFLOAT* vals, t_CKUINT numItems) {
//        auto vec = new float[numItems];
//        for (int i=0; i< numItems; i++) {
//            vec[i] = vals[i];
//        }
//
//        myFloatArrayVars[varName] = vec;
//        myFloatArrayVarSizes[varName] = numItems;
//    }

    CHUCKDESIGNERSHARED_API float getFloat(const char* varName) {
        if (myFloatVars.find(varName) != myFloatVars.end()) {
            return myFloatVars[varName];
        }
        return 0.f;
    }

//    CHUCKDESIGNERSHARED_API void getFloatArray(const char* varName, t_CKFLOAT* vals, int& numItems) {
//        if (myFloatArrayVars.find(varName) != myFloatArrayVars.end()) {
//            numItems = myFloatArrayVarSizes[varName];
//            // todo:
//            //*vals = &myFloatArrayVars[varName];
//        }
//        numItems = 0;
//    }

    
    CHUCKDESIGNERSHARED_API bool initChuckInstance( unsigned int chuckID, unsigned int sampleRate, unsigned int numInChannels, unsigned int numOutChannels, string globalDir )
    {
        if( chuck_instances.count( chuckID ) == 0 )
        {
            // if we aren't tracking a chuck vm on this ID, create a new one
            ChucK * chuck = new ChucK();
            
            // set params: sample rate, 2 in channels, 2 out channels,
            // don't halt the vm, and use our data directory
            chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, (t_CKINT) sampleRate );
            chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT) numInChannels);
            chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT) numOutChannels);
            chuck->setParam( CHUCK_PARAM_VM_HALT, (t_CKINT) 0 );
            chuck->setParam( CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT) 0 );
            // directory for compiled.code
            chuck->setParam( CHUCK_PARAM_WORKING_DIRECTORY, globalDir);
            // directories to search for chugins and auto-run ck files
            std::list< std::string > chugin_search;
            chugin_search.push_back(globalDir + "/Chugins" );
            chugin_search.push_back(globalDir + "/ChuGins" );
            chugin_search.push_back(globalDir + "/chugins" );
            chuck->setParam( CHUCK_PARAM_USER_CHUGIN_DIRECTORIES, chugin_search );
            
            // initialize and start
            chuck->init();
            chuck->start();
            
            chuck_instances[chuckID] = chuck;
        }
        return true;
    }


    CHUCKDESIGNERSHARED_API bool clearChuckInstance( unsigned int chuckID )
    {
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // the chuck to clear
            ChucK * chuck = chuck_instances[chuckID];
            
            // create a msg asking to clear the VM
            Chuck_Msg * msg = new Chuck_Msg;
            msg->type = MSG_CLEARVM;
            
            // null reply so that VM will delete for us when it's done
            msg->reply = ( ck_msg_func )NULL;
            
            // tell the VM to clear
            chuck->vm()->globals_manager()->execute_chuck_msg_with_globals( msg );
            
            return true;
        }
        
        return false;
    }


    CHUCKDESIGNERSHARED_API bool clearGlobals( unsigned int chuckID )
    {
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // the chuck to clear
            ChucK * chuck = chuck_instances[chuckID];
            
            // create a msg asking to clear the globals
            Chuck_Msg * msg = new Chuck_Msg;
            msg->type = MSG_CLEARGLOBALS;
            
            // null reply so that VM will delete for us when it's done
            msg->reply = ( ck_msg_func )NULL;
            
            // tell the VM to clear
            chuck->vm()->globals_manager()->execute_chuck_msg_with_globals( msg );
            
            return true;
        }
        
        return false;
    }


    CHUCKDESIGNERSHARED_API bool cleanupChuckInstance( unsigned int chuckID, unsigned int opId)
    {
        if( chuck_instances.count( chuckID ) > 0 )
        {
            ChucK * chuck = chuck_instances[chuckID];
            
            // don't track it anymore
            chuck_instances.erase( chuckID );

            if( data_instances.count( chuckID ) > 0 )
            {
                data_instances[chuckID]->myId = -1;
                data_instances.erase( chuckID );
            }

            op_ids_to_chuck_ids.erase(opId);

            // wait a bit
            usleep( 30000 );

            // cleanup this chuck early
            delete chuck;

        }

        return true;
    }
    
    
    CHUCKDESIGNERSHARED_API bool chuckManualAudioCallback( unsigned int chuckID, float * inBuffer, float * outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels )
    {
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // zero out the output buffer, in case chuck isn't running
            for( unsigned int n = 0; n < numFrames * outChannels; n++ )
            {
                outBuffer[n] = 0;
            }
            
            // call callback
            // TODO: check inChannels, outChannels
            chuck_instances[chuckID]->run( inBuffer, outBuffer, numFrames );
            
        }
        
        return true;
    }


    // on launch, reset all ids (necessary when relaunching a lot in unity editor)
    CHUCKDESIGNERSHARED_API void cleanRegisteredChucks() {
    
        // first, invalidate all callbacks' references to chucks
        for( std::map< unsigned int, EffectData::Data * >::iterator it =
             data_instances.begin(); it != data_instances.end(); it++ )
        {
            EffectData::Data * data = it->second;
            data->myId = -1;
        }
        
        // wait for callbacks to finish their current run
        usleep( 30000 );
        
        // next, delete chucks
        for( std::map< unsigned int, ChucK * >::iterator it =
             chuck_instances.begin(); it != chuck_instances.end(); it++ )
        {
            ChucK * chuck = it->second;
            delete chuck;
        }
        
        // delete stored chuck pointers
        chuck_instances.clear();
        // delete data instances
        data_instances.clear();
        
        // clear out callbacks also
        setStdoutCallback( NULL );
        setStderrCallback( NULL );
    }

    
    bool RegisterChuckData( EffectData::Data * data, const unsigned int id )
    {
        // only store if id has been used / a chuck is already initialized
        if( chuck_instances.count( id ) == 0 )
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
}
