#include "StdAfx.h"
#include "HotKeyHelper.h"
#include <assert.h>

CHotKeyHelper::CHotKeyHelper(HWND hwnd)
{
	assert(hwnd);
	for(int i = 0;i < HOT_KEY_NUM ;i++)
		sucess[i] = false;
	this->hwnd = hwnd;
}


CHotKeyHelper::~CHotKeyHelper(void)
{
	hwnd = 0;
}

bool CHotKeyHelper::Register()
{
	//sucess[0] = RegisterHotKey(hwnd,VOLUME_MUTE,NULL,VK_VOLUME_MUTE);
	//sucess[1] = RegisterHotKey(hwnd,VOLUME_DOWN,NULL,VK_VOLUME_DOWN);
	//sucess[2] = RegisterHotKey(hwnd,VOLUME_UP,NULL,VK_VOLUME_UP);
	sucess[3] = RegisterHotKey(hwnd,MEDIA_NEXT,NULL,VK_MEDIA_NEXT_TRACK);
	sucess[4] = RegisterHotKey(hwnd,MEDIA_PREV,NULL,VK_MEDIA_PREV_TRACK);
	sucess[5] = RegisterHotKey(hwnd,MEDIA_STOP,NULL,VK_MEDIA_STOP);
	sucess[6] = RegisterHotKey(hwnd,MEDIA_PLAY_PAUSE,NULL,VK_MEDIA_PLAY_PAUSE);
	return true;
}

bool CHotKeyHelper::UnRegister()
{
	//sucess[0] = UnregisterHotKey(hwnd,VOLUME_MUTE);//,NULL,VK_VOLUME_MUTE);
	//sucess[1] = UnregisterHotKey(hwnd,VOLUME_DOWN);///,NULL,VK_VOLUME_DOWN);
	//sucess[2] = UnregisterHotKey(hwnd,VOLUME_UP);//,NULL,VK_VOLUME_UP);
	if(sucess[3])
		UnregisterHotKey(hwnd,MEDIA_NEXT);//,NULL,VK_MEDIA_NEXT_TRACK);
	if(sucess[4])
		UnregisterHotKey(hwnd,MEDIA_PREV);//,NULL,VK_MEDIA_PREV_TRACK);
	if(sucess[5])
		UnregisterHotKey(hwnd,MEDIA_STOP);//,NULL,VK_MEDIA_STOP);
	if(sucess[6])
		UnregisterHotKey(hwnd,MEDIA_PLAY_PAUSE);//,NULL,VK_MEDIA_PLAY_PAUSE);
	return true;
}