#pragma once
#include <windows.h>
#include <xaudio2.h>

class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    HANDLE hBufferEndEvent;
    VoiceCallback(): hBufferEndEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) ){}
    ~VoiceCallback(){ CloseHandle( hBufferEndEvent ); }

    //Called when the voice has just finished playing a contiguous audio stream.
     STDMETHOD_(void, OnStreamEnd() ){  }

    //Unused methods are stubs
    STDMETHOD_(void, OnVoiceProcessingPassEnd()) { }
    STDMETHOD_(void, OnVoiceProcessingPassStart(UINT32 SamplesRequired)) {    }
    STDMETHOD_(void, OnBufferEnd(void * pBufferContext) )   {SetEvent( hBufferEndEvent ); }
    STDMETHOD_(void, OnBufferStart(void * pBufferContext)) {    }
    STDMETHOD_(void, OnLoopEnd(void * pBufferContext)) {    }
    STDMETHOD_(void, OnVoiceError(void * pBufferContext, HRESULT Error)) { }
};
