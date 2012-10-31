#pragma once
#include "..\ZLPCore\ZPlayer.h"
#include "..\ZLPCore\Decoder.h"
#include "ZPlayList.h"
#include "PlayerBase.h"

class CZPlayerCtrl
{
public:
	__declspec(dllexport) CZPlayerCtrl(void);
	__declspec(dllexport) CZPlayerCtrl(int );
	__declspec(dllexport) virtual ~CZPlayerCtrl(void);

	// list operation
	__declspec(dllexport) int AddMedia(Media m);
	__declspec(dllexport) unsigned int GetMediaCount() ;
	__declspec(dllexport) const Media GetAt(unsigned int);
	__declspec(dllexport) void ChangeMedia(unsigned int,unsigned int);
	__declspec(dllexport) void RemoveAt(unsigned int);
	__declspec(dllexport) void Clear();
	__declspec(dllexport) unsigned int GetCurrentPlay() const;
	__declspec(dllexport) bool MediaExist(const Media );

	// player operation
	__declspec(dllexport) int Play(unsigned int media = 0);
	__declspec(dllexport) int Pause();
	__declspec(dllexport) int Stop();
	__declspec(dllexport) int Seek(float seconds);
	__declspec(dllexport) int SetMute(bool);
	__declspec(dllexport) int Resume();
	__declspec(dllexport) int SetVolume(int);
	__declspec(dllexport) PLAYER_STATE GetPlayerState();
	//__declspec(dllexport) const PlayerConfig GetPlayConfig();
	__declspec(dllexport) int SetCallback(ZCallbackFunction ,void * = NULL,void * = NULL,void * = NULL);
	__declspec(dllexport) float GetPlayDuration(bool s = true);
	__declspec(dllexport) WAVEINFO GetWaveInfo();
	__declspec(dllexport) CDecoder *GetDecoder();
private:
	unsigned int currentPlay;
	CZPlayer* player;
	CZPlayList playList;
	CDecoder decoder;
	WAVEINFO waveInfo;
	PLAYER_STATE ps;
	//PlayerConfig pc;
	CallbackPara m_callbakPara;
private:
	void  static __stdcall ZPlayerMessage(void* instance, ZMessage mes,void *client ,WPARAM,LPARAM);
};

