#pragma once

#include <d3d10_1.h>
#include <xaudio2.h>

#include "Decoder.h"
#include "PlayerBase.h"

class CZPlayer
{
public:
	__declspec(dllexport) CZPlayer(void);
	__declspec(dllexport) virtual ~CZPlayer(void);
	__declspec(dllexport) int Open(CDecoder *);
	__declspec(dllexport) int Play();
	__declspec(dllexport) int Play(float,float);
	__declspec(dllexport) int Pause();
	__declspec(dllexport) int Resume();
	__declspec(dllexport) int Stop();
	__declspec(dllexport) int Seek(float);
	__declspec(dllexport) float GetPlayDuration(bool s = true);// get play time
	__declspec(dllexport) int GetPlayerState(PLAYER_STATE *);
	__declspec(dllexport) int SetVolume(int);
	__declspec(dllexport) int SetMute(bool);
	__declspec(dllexport) int SetCallback(ZCallbackFunction ,void * = NULL,void * = NULL,void * = NULL);
	__declspec(dllexport) int InitAudio();
private:	
	CallbackPara m_callbakPara;
	PLAYER_STATE m_playerState;
	PLAYER_CONTROL m_playerControl;
	CDecoder *m_decoder;

	AUDIO_STATE *m_audioState;
	XAUDIO2_DEVICE_DETAILS m_details;
	HANDLE m_hThread;
	DWORD m_hThreadID;
	
	void CleanupAudio();
	static int WINAPI PlayThread(void* lpdwParam);
};

