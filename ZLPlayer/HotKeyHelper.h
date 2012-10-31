#pragma once

#define VOLUME_MUTE			1001
#define VOLUME_DOWN			1002
#define VOLUME_UP			1003
#define MEDIA_NEXT			1004
#define MEDIA_PREV			1005
#define MEDIA_STOP			1006
#define MEDIA_PLAY_PAUSE	1007

#define HOT_KEY_NUM 7
class CHotKeyHelper
{
public:
	CHotKeyHelper(HWND hwnd);
	~CHotKeyHelper(void);
private:
	BOOL sucess[HOT_KEY_NUM];
	HWND hwnd;
public:
	bool Register();
	bool UnRegister();
};

