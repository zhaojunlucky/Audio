#pragma once

#define STREAMING_BUFFER_SIZE 65536
#define MAX_BUFFER_COUNT 3
static BYTE buffers[MAX_BUFFER_COUNT][STREAMING_BUFFER_SIZE];
#define MAX_VOLUME 10000
#define MIN_VOLUME 0

typedef struct AUDIO_STATE
{
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	IXAudio2SourceVoice* pSourceVoice;
	//VoiceCallback voiceCallback;
	bool bInitialized;
	// reverb
	IUnknown *pReverbEffect;
	IXAudio2SubmixVoice *pSubmixVoice;
}AUDIO_STATE;

typedef struct 
{
	bool play;
	bool pause;
	bool stop;
	bool mute;
	int volume;
	float start;// second
	float end;//second
}PLAYER_STATE;

enum ZMessage
{
	ZPLAY = 0,
	ZPAUSE,
	ZSTOP,
	ZUSERSTOP,
	ZRESUM,
	ZOPEN,
	ZMUTE,
	ZUNMUTE,
	ZVOLUME,
	ZSEEK,
	ZNONE
};

typedef void (__stdcall * ZCallbackFunction)(void* instance, ZMessage mes,void *client ,WPARAM,LPARAM);

typedef struct CallbackPara
{
	void *client;
	ZCallbackFunction m_zCallback;
}CallbackPara;

typedef struct PLAYER_CONTROL
{
	DWORD start;
	DWORD end;
	DWORD current;
}PLAYER_CONTROL;