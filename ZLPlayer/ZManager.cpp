#include "StdAfx.h"
#include "ZManager.h"
#include "..\ZLP\ZCueSheet.h"
#include "..\ZLPCore\TagHelper.h"
#include "..\Utility\ZStringHelper.h"
#include "..\ZLPCore\Decoder.h"
#include "..\Utility\DirectoryHelper.h"
#include <time.h>
#include "ZFileChooseDlg.h"
#include "ZLPlayerDlg.h"
#include "..\Utility\FileHelper.h"
#include "ZSetting.h"
#include "DirectoryUtil.h"

CZManager::CZManager(void)
{
	player = new CZPlayerCtrl;
	player->SetCallback(ZPlayerMessage,this);
	zbStatus.tBarPlayPause = 0;
	zbStatus.playPause = 0;

	ps.mute = false;
	CZSetting set;
	ps.pm = set.ReadPlayMode();
	ps.volume = set.ReadVolume();
	player->SetVolume(ps.volume);
}


CZManager::~CZManager(void)
{
	if(player)
		delete player;
}

void CZManager::SetVolume(int v)
{
	ps.volume = v;
	player->SetVolume(ps.volume);
}
int CZManager::PlayerControl(PlayCommand cm,int value)
{
	switch (cm)
	{
	case MPlay:
		{
			// -1 for play next determined by pm
			if(-1 == value)
			{
				//
				if(PMSingle == ps.pm)
					value = (int)player->GetCurrentPlay();
				else if(PMRandom == ps.pm)
				{
					srand((unsigned)time(0));
					value = rand() % player->GetMediaCount();
				}
				else
					value = (int)player->GetCurrentPlay() + 1;
			}
			if(value >=  player->GetMediaCount() )
			{
				if(PMCircle == ps.pm)
					value = 0;
				else 
					break;
			}	

			return player->Play(value);
		}
		break;
	case MPause:return player->Pause();
		break;
	case MResume:return player->Resume();
		break;
	case MStop:return player->Stop();
		break;
	case MSeek:return player->Seek(value);
		break;
	case MVolume:return player->SetVolume(value);
		break;
	case MMute:return player->SetMute(value>0?false:true);
		break;
	case MPrevious:{	
						int cur = player->GetCurrentPlay() - 1;
						if(PMRandom == ps.pm)
						{
							srand((unsigned)time(0));
							cur = rand() % player->GetMediaCount();
						}
						return player->Play(cur<0?player->GetMediaCount()-1:cur);
				   }break;
	case MNext:{
					int cur = player->GetCurrentPlay()+ 1;
					int total = player->GetMediaCount();
					if(PMRandom == ps.pm)
					{
						srand((unsigned)time(0));
						cur = rand() % player->GetMediaCount();
					}
					return player->Play(cur>=total?0:cur);
			   }break;
	default:
		return -1;
	}
	return 0;
}

unsigned int CZManager::GetCurrentPlay()
{
	return player->GetCurrentPlay();
}

PLAYER_STATE CZManager::GetPlayerState()
{
	return player->GetPlayerState();
}

unsigned int CZManager::GetMediaCount()
{
	return player->GetMediaCount();
}

void CZManager::SetPlayMode(PlayMode pm)
{
	ps.pm = pm;
	//UpdateUI();
	PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd,1204,ZNONE,0);
}

int CZManager::AddFile(const wchar_t * mediaPath,bool updateUI)
{
	AudioFormat af = CAudioBase::IsSupportMedia(mediaPath);
	if(AutoDetect == af /*|| MP4 == af*/)
		return 0;
	if(CUE == af)
	{
		vector<Media> mm;
		CZCueSheet cue;
		if(!cue.PaseSheet(mediaPath) || cue.GetSize() <= 0)
			return 0;
		for(int i = 0; i < cue.GetSize(); i++)
		{
			if(!player->MediaExist(cue.GetAt(i)))
				mm.push_back(cue.GetAt(i));
		}
		
		
		::SetForegroundWindow(::AfxGetMainWnd()->GetSafeHwnd());   
		::SetForegroundWindow(::GetLastActivePopup(::AfxGetMainWnd()->GetSafeHwnd())); 
		CZFileChooseDlg dlg(&mm);
		if(0 == dlg.DoModal())
		{
			for(int i = 0;i < mm.size();i++)
			{
				if(player->AddMedia(mm[i]) >= 0)
					UpdateList(player->GetMediaCount()-1);	
			}
			if(mm.size() > 0 && updateUI)
				//UpdateUI();
				PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd,1204,ZNONE,0);
			return mm.size();
		}
		return 0;
	}
	else
	{
		Media m;
		m.fileName = mediaPath;
	
		m.mediaFormat = af;

	
		if(player->AddMedia(m)>=0)
			UpdateList(player->GetMediaCount()-1);
		
		if(updateUI)
			//UpdateUI();
			PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd,1204,ZNONE,0);
		return 1;
	}
}

int CZManager::AddFile(const char *mediaPath,bool updateUI)
{
	CString mediaFile(mediaPath);
	BSTR bstr = mediaFile.AllocSysString();
	int r = AddFile(bstr,updateUI);
	SysFreeString(bstr);
	mediaFile.Empty();
	return r;
}

int CZManager::AddFile(const wchar_t * mediaPath)
{
	if(ZStringHelper::CZStringHelper::EndWith(mediaPath,L"lrc"))
	{
		CString s(mediaPath);
		CString d;
		wchar_t exeFullPath[MAX_PATH];
		GetModuleFileName(NULL, exeFullPath, MAX_PATH);
		PathRemoveFileSpec(exeFullPath);
		wcscat(exeFullPath,L"\\ZDLRC");
		int index = s.ReverseFind('\\');
		d.Format(L"%s%s",exeFullPath,s.Right(s.GetLength()-index));
		CFileHelper fH;
		fH.Move(s,d);
		s.Empty();
		d.Empty();
		return 0;
	}
	else

	return AddFile(mediaPath,true);
}

int CZManager::AddFile(const char *mediaPath)
{
	return AddFile(mediaPath,true);
}

int CZManager::AddDirectory(const wchar_t *dir)
{
	/*CDirectoryHelper dirHelper;
	int files = 0;
	if(dirHelper.IsDirectory(dir))
	{
		if(dirHelper.FindDirectoryFiles(dir) > 0)
		{
			for(unsigned int i = 0; i < dirHelper.GetSize();i++)
			{
				if(AutoDetect != CAudioBase::IsSupportMedia(dirHelper.GetAt(i)) && !dirHelper.IsDirectory(dirHelper.GetAt(i)))
				{
					files += AddFile(dirHelper.GetAt(i),false);					
				}
			}
		}
	}*/

	CDirectoryUtil dirUtil;
	int numOfFiles = 0;
	dirUtil.FindFiles(dir);
	for(int i = 0; i < dirUtil.GetFileCount(); i++)
	{
		CString file(dirUtil.GetFilePath(i));
		numOfFiles += AddFile(file,false);	

	}

	//UpdateUI();
	PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd,1204,ZNONE,0);
	return numOfFiles;
}

void CZManager::SetPlayerConfig(PlayerConfig ps)
{
	this->ps = ps;
}
PlayerConfig CZManager::GetPlayerConfig()
{
	return ps;
}

int CZManager::RemoveMedia(unsigned int index,bool updateUI)
{
	player->RemoveAt(index);
	if(updateUI)
		//UpdateUI();
		PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd,1204,ZNONE,0);
	return 0;
}

int CZManager::Clear()
{
	player->Clear();
	//UpdateUI();
	PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd,1204,ZNONE,0);
	return 0;
}

const CZButtonStatus CZManager::GetButtonStatus()
{
	return this->zbStatus;
}

// 0 disable,1 play 2 pause
void CZManager::UpdateUI(ZMessage mes)
{
	zbStatus.mes = mes;
	int curPlay = player->GetCurrentPlay();
	int total = player->GetMediaCount();
	if(total <=0 )
	{
		zbStatus.playPause = 0;
		zbStatus.tBarPlayPause = 0;
		zbStatus.next = false;
		zbStatus.previous = false;
		zbStatus.tBarPreious = 0;
		zbStatus.tBarNext = 0;
	}
	else
	{
		if(PMCircle == ps.pm || PMRandom == ps.pm || PMSingle == ps.pm)
		{ 
			zbStatus.next = true;
			zbStatus.previous = true;

			zbStatus.tBarNext = 1;

			zbStatus.tBarPreious = 1;
		}
		else if(total > 1)
		{
			if(curPlay == total -1)
			{
				zbStatus.next = false;
				zbStatus.previous = true;

				zbStatus.tBarNext = 0;
				zbStatus.tBarPreious = 1;
			}
			else if(0 == curPlay)
			{
				zbStatus.next = true;
				zbStatus.previous = false;

				zbStatus.tBarNext = 1;

				zbStatus.tBarPreious = 0;
			}
			else
			{
				zbStatus.next = true;
				zbStatus.previous = true;

				zbStatus.tBarNext = 1;

				zbStatus.tBarPreious = 1;
			}

			
		}
		else
		{
			zbStatus.next = false;
			zbStatus.previous = false;

			zbStatus.tBarNext = 0;

			zbStatus.tBarPreious = 0;
		}
	}
	if(0 == zbStatus.tBarPlayPause && total > 0)
	{
		zbStatus.tBarPlayPause = 1;
	}
	if(0 == zbStatus.playPause && total > 0)
	{
		zbStatus.playPause = 1;
	}
	//if(ZOPEN == mes/* || ZUSERSTOP == mes || ZSTOP == mes*/)
	//{
	//	SendMessageW(::AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_UPDATE_UI,(WPARAM)&zbStatus,0);
	//}
	//else
	//	PostMessageW(::AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_UPDATE_UI,(WPARAM)&zbStatus,0);

	//((CZLPlayerDlg*)::AfxGetApp()->GetMainWnd())->ZSendMessage(WM_UPDATE_UI,(WPARAM)&zbStatus,0);
		//SendNotifyMessage(::AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_UPDATE_UI,(WPARAM)&zbStatus,0);
		//SendMessageTimeout(::AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_UPDATE_UI,(WPARAM)&zbStatus,0,SMTO_BLOCK,10,0);
}

void CZManager::UpdateList(const unsigned int index)
{
	Media m = player->GetAt(index);
	SendMessageW(::AfxGetMainWnd()->GetSafeHwnd(),WM_UPDATE_PLAYLIST,(WPARAM)&m,0);
}

void __stdcall CZManager::ZPlayerMessage(void* instance, ZMessage mes,void *client,WPARAM wParam,LPARAM lParam)
{
	//CZManager * manager = (CZManager*)client;
	//if(!manager)
	//	return ;
	////printf("%d\n",mes);
	//if(ZPLAY == mes)
	//{
	//	manager->zbStatus.tBarPlayPause = 2;
	//	manager->zbStatus.playPause = 2;
	//}
	//else if(ZPAUSE == mes)
	//{
	//	manager->zbStatus.tBarPlayPause = 1;
	//	manager->zbStatus.playPause = 1;
	//}
	//else if(ZSTOP == mes)
	//{
	//	//manager->zbStatus.tBarPlayPause = 1;
	//	//manager->zbStatus.playPause = 1;
	//	// play next
	//	if(manager->player->GetMediaCount() <= 0)
	//	{
	//		manager->zbStatus.tBarPlayPause = 0;
	//		manager->zbStatus.playPause = 0;
	//	}
	//	else
	//	{
	//		manager->zbStatus.tBarPlayPause = 1;
	//		manager->zbStatus.playPause = 1;
	//	}

	//}
	//else if(ZUSERSTOP == mes)
	//{		
	//	if(manager->player->GetMediaCount() <= 0)
	//	{
	//		manager->zbStatus.tBarPlayPause = 0;
	//		manager->zbStatus.playPause = 0;
	//	}
	//	else
	//	{
	//		manager->zbStatus.tBarPlayPause = 1;
	//		manager->zbStatus.playPause = 1;
	//	}
	//}
	//else if(ZRESUM == mes)
	//{
	//	manager->zbStatus.tBarPlayPause = 2;
	//	manager->zbStatus.playPause = 2;
	//}
	//else if(ZOPEN == mes)
	//{		
	//	manager->zbStatus.time = (int)wParam;
	//	//printf("total times:%d\n",manager->zbStatus.time);
	//}
	//manager->UpdateUI(mes);
}