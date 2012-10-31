#pragma once
#include "common.h"
#include "..\ZLP\PlayerBase.h"
#include "..\ZLP\ZPlayerCtrl.h"

class CZManager
{
public:
	CZManager(void);
	~CZManager(void);

	int PlayerControl(PlayCommand,int = 0);
	int AddFile(const wchar_t * mediaPath);
	int AddFile(const char *mediaPath);
	int AddDirectory(const wchar_t *dir);
	int RemoveMedia(unsigned int,bool updateUI = true);
	int Clear();
	const CZButtonStatus GetButtonStatus();
	unsigned int GetCurrentPlay();
	unsigned int GetMediaCount();
	PLAYER_STATE GetPlayerState();
	void SetPlayerConfig(PlayerConfig ps);
	void SetPlayMode(PlayMode);
	void SetVolume(int v);
	PlayerConfig GetPlayerConfig();
	CZPlayerCtrl *player;
public:

	PlayerConfig ps;
	CZButtonStatus zbStatus;
	void  static __stdcall ZPlayerMessage(void* instance, ZMessage mes,void *client,WPARAM ,LPARAM);
	void UpdateUI(ZMessage = ZNONE);
	void UpdateList(const unsigned int);

	int AddFile(const wchar_t * mediaPath,bool updateUI);
	int AddFile(const char *mediaPath,bool updateUI);
};

