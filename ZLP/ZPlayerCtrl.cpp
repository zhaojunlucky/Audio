#include "StdAfx.h"
#include "ZPlayerCtrl.h"


CZPlayerCtrl::CZPlayerCtrl(void)
{
	currentPlay = 0;
	player = new CZPlayer;
	player->InitAudio();
	player->SetVolume(5000);
	player->SetCallback(ZPlayerMessage,this);
}

CZPlayerCtrl::CZPlayerCtrl(int volume)
{
	currentPlay = 0;
	player->InitAudio();
	player->SetVolume(volume);
	player->SetCallback(ZPlayerMessage,this);
}

CZPlayerCtrl::~CZPlayerCtrl(void)
{
	 player->Stop();
	 delete player;
}

int CZPlayerCtrl::AddMedia(Media m)
{
	if(!MediaExist(m))
	{
		CDecoder d;
		d.Open(m.fileName,m.mediaFormat);
		if(!m.isCue)
		{			
			m.title = d.GetID3Info()->title.GetData();
			if(""==m.title.Trim())
			{
				int s = m.fileName.ReverseFind(L'\\')+1;
				int c = m.fileName.ReverseFind(L'.') -s;
				m.title = m.fileName.Mid(s,c);
			}
			m.amblum = d.GetID3Info()->album.GetData();
			m.aritst = d.GetID3Info()->artist.GetData();
			//m.track = d.GetID3Info()->track.GetData();
		}
		m.bitrate = d.GetWaveInfo()->bitrate;
		m.samplingRate = d.GetWaveInfo()->nSamplesPerSec;
		playList.AddItem(m);
		return playList.GetSize()-1;
	}
	return -1;
}

unsigned int CZPlayerCtrl::GetMediaCount()
{
	return playList.GetSize();
}

const Media CZPlayerCtrl::GetAt(unsigned int index)
{
	return playList.GetItem(index);
}

void CZPlayerCtrl::ChangeMedia(unsigned int srcIndex,unsigned int desIndex)
{
	
}

void CZPlayerCtrl::RemoveAt(unsigned int index)
{
	if(index == currentPlay)
	{
		Stop();
	}
	// have more than 1 files
	if(currentPlay > index)
	{
		currentPlay--;
	}
	else if(currentPlay == index)
	{
		if(1 == (int)playList.GetSize())
		{
			currentPlay =0;
		}
		else if((int)currentPlay > 0)
		{
			currentPlay--;
		}
	}
	//else// have more than 1 files, do nothing
	//{

	//}

	printf("after remove current play:%d\n",currentPlay);
	playList.RemoveAt(index);
}

void CZPlayerCtrl::Clear()
{
	Stop();
	currentPlay = 0;
	playList.RemoveAll();
}

unsigned int CZPlayerCtrl::GetCurrentPlay() const
{
	return currentPlay;
}

bool CZPlayerCtrl::MediaExist(const Media m)
{
	return (-1 != playList.Contains(m) );
}

//const PlayerConfig CZPlayerCtrl::GetPlayConfig()
//{
//	return pc;
//}


// 0 success
// -1 fail
// 1 fail
int CZPlayerCtrl::Play(unsigned int media)
{
	assert(media >=0 && media < playList.GetSize());
	this->currentPlay = media;
	player->Stop();
	Media m = playList.GetItem(media);
	
	int r = -1;
	
	r = decoder.Open(m.fileName,m.mediaFormat);
	if(0 == r)
	{
		//if(this->m_callbakPara.m_zCallback)
			//this->m_callbakPara.m_zCallback(this,ZOPEN,this->m_callbakPara.client,(m.isCue && !m.isSingleFile)?(m.end-m.start):decoder.GetWaveInfo()->durationTime,0);
		PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd,1204,ZOPEN,(m.isCue && !m.isSingleFile)?(m.end-m.start):decoder.GetWaveInfo()->durationTime);
		r = player->Open(&decoder);
		if(0 == r)
		{
			if(!m.isCue || m.isSingleFile)
				player->Play();
			else
			{
				player->Play(m.start,m.end);
			}
		}
	}	
	return r;
}

int CZPlayerCtrl::Pause()
{
	return player->Pause();
}

int CZPlayerCtrl::Stop()
{
	return player->Stop();
}

int CZPlayerCtrl::Seek(float seconds)
{
	return player->Seek(seconds);
}

int CZPlayerCtrl::SetMute(bool mute)
{
	return player->SetMute(mute);
}

int CZPlayerCtrl::Resume()
{
	return player->Resume();
}

int CZPlayerCtrl::SetVolume(int volume)
{
	return player->SetVolume(volume);
}

PLAYER_STATE CZPlayerCtrl::GetPlayerState()
{
	memset(&ps,0,sizeof(PLAYER_STATE));
	player->GetPlayerState(&ps);
	return ps;
}

CDecoder *CZPlayerCtrl::GetDecoder()
{
	return &decoder;
}

int CZPlayerCtrl::SetCallback(ZCallbackFunction zcallback,void *client,void * /* = NULL */,void * /* = NULL */)
{
	this->m_callbakPara.client = client;
	this->m_callbakPara.m_zCallback = zcallback;
	return 0;
}

float CZPlayerCtrl::GetPlayDuration(bool s)
{
	return player->GetPlayDuration(s);
}

WAVEINFO CZPlayerCtrl::GetWaveInfo( )
{
	waveInfo = *decoder.GetWaveInfo();
	return waveInfo;
}

void __stdcall CZPlayerCtrl::ZPlayerMessage(void* instance, ZMessage mes,void *client ,WPARAM wParam,LPARAM lParam)
{
	PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd,1204,mes,lParam);
	//printf("zlpcore %d\n",(int)mes);
	//CZPlayerCtrl *player = (CZPlayerCtrl*)client;
	//if(ZPLAY == mes)
	//{
	//	if(player && player->m_callbakPara.m_zCallback)
	//		player->m_callbakPara.m_zCallback(instance,ZPLAY,player->m_callbakPara.client,wParam,lParam);
	//}
	//else if(ZPAUSE == mes)
	//{
	//	if(player && player->m_callbakPara.m_zCallback)
	//		player->m_callbakPara.m_zCallback(instance,ZPAUSE,player->m_callbakPara.client,wParam,lParam);
	//}
	//else if(ZSTOP == mes)
	//{
	//	if(player && player->m_callbakPara.m_zCallback)
	//		player->m_callbakPara.m_zCallback(instance,ZSTOP,player->m_callbakPara.client,wParam,lParam);
	//}
	//else if(ZUSERSTOP == mes)
	//{
	//	if(player && player->m_callbakPara.m_zCallback)
	//		player->m_callbakPara.m_zCallback(instance,ZUSERSTOP,player->m_callbakPara.client,wParam,lParam);
	//}
	//else if(ZRESUM == mes)
	//{
	//	if(player && player->m_callbakPara.m_zCallback)
	//		player->m_callbakPara.m_zCallback(instance,ZRESUM,player->m_callbakPara.client,wParam,lParam);
	//}
	//else if(ZOPEN == mes)
	//{
	//	if(player && player->m_callbakPara.m_zCallback)
	//		player->m_callbakPara.m_zCallback(instance,ZOPEN,player->m_callbakPara.client,wParam,lParam);
	//}
}

