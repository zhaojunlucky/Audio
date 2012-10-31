#pragma once
enum ZDDIRAction
{
	ZDAdd,ZDRemove,ZDModify,ZDRename,ZDRenameOld
};
class CZFileWatch
{
public:

	typedef void (__stdcall *ZDDIRNotification)( ZDDIRAction act,wchar_t * dir,wchar_t *filename,void *client);

	__declspec(dllexport) CZFileWatch(const wchar_t* dir,ZDDIRNotification callback,void *client = NULL);
	__declspec(dllexport) ~CZFileWatch(void);
	__declspec(dllexport) bool Start(void);
	__declspec(dllexport) void Stop(void);

private:
	wchar_t* m_dir;
	void* client;
	ZDDIRNotification m_callback;
    HANDLE hThread;
    HANDLE hDir;

	static DWORD WINAPI ThreadFunc( LPVOID lParam );
};

