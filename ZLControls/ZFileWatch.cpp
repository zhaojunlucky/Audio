#include "StdAfx.h"
#include "ZFileWatch.h"
#include "..\Utility\DirectoryHelper.h"


CZFileWatch::CZFileWatch(const wchar_t* dir,ZDDIRNotification callback,void* client)
{
	CDirectoryHelper dirHelper;
	assert(NULL != dir && NULL != callback);
	if(!dirHelper.DirectoryExist(dir))
		dirHelper.CreateDirectoryD(dir);
	int l = wcslen(dir);
	m_dir = new wchar_t[l + 1];
	wcscpy(m_dir,dir);
	m_dir[l] = L'\0';
	hThread = 0;
	hDir = 0;
	this->client = client;
	m_callback = callback;
}


CZFileWatch::~CZFileWatch(void)
{
	Stop();
	delete m_dir;
}

bool CZFileWatch::Start(void)
{
	if(NULL == hThread)
	{
		DWORD ThreadId;
		hThread=CreateThread( NULL,0,ThreadFunc,this,0,&ThreadId );
	}
	return (NULL != hThread);
}

void CZFileWatch::Stop(void)
{
	if( NULL != hThread )
	{
		TerminateThread( hThread, 0 );
		hThread = NULL;
	}
	if( 0 != hDir )
	{
		CloseHandle( hDir );
		hDir = 0;
	}
}

DWORD WINAPI CZFileWatch::ThreadFunc( LPVOID lParam )
{
	CZFileWatch *p = (CZFileWatch*)lParam;
	p->hDir = CreateFile( p->m_dir, FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | 
		FILE_SHARE_WRITE | 
		FILE_SHARE_DELETE, NULL, 
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | 
		FILE_FLAG_OVERLAPPED, NULL); 

	if (p->hDir == INVALID_HANDLE_VALUE) 
	{ 	
		return -1;
	} 
	char notify[1024]; 
	FILE_NOTIFY_INFORMATION *pnotify=(FILE_NOTIFY_INFORMATION *)notify; 
	FILE_NOTIFY_INFORMATION *tmp; 
	DWORD cbBytes;
	while(true)
	{
		if(ReadDirectoryChangesW(p->hDir, &notify, sizeof(notify),
			FALSE, FILE_NOTIFY_CHANGE_FILE_NAME| FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_ACTION_ADDED|FILE_ACTION_MODIFIED
			|FILE_ACTION_RENAMED_NEW_NAME|FILE_ACTION_RENAMED_OLD_NAME, 
			&cbBytes, NULL, NULL)) 
		{ 

			tmp = pnotify; 
			switch(tmp->Action) 
			{ 
			case FILE_ACTION_ADDED: 
				p->m_callback(ZDAdd,0,0,p->client);
				break; 
			case FILE_ACTION_REMOVED:
				p->m_callback(ZDRemove,0,0,p->client);
				break; 
			case FILE_ACTION_MODIFIED: 
				p->m_callback(ZDModify,0,0,p->client);
				break; 
			case FILE_ACTION_RENAMED_OLD_NAME: 
				p->m_callback(ZDRenameOld,0,0,p->client);
				break;
			case FILE_ACTION_RENAMED_NEW_NAME: 
				p->m_callback(ZDRename,0,0,p->client);
				break; 
			default: 
				break; 
			}
		}
	}
	return 0;
}