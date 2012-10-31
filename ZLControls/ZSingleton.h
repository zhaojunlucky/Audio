#pragma once
class CZSingleton
{
public:
	__declspec(dllexport) CZSingleton(void);
	__declspec(dllexport) ~CZSingleton(void);

	__declspec(dllexport) BOOL Check(wchar_t **arg,int num,const wchar_t *key);
	__declspec(dllexport) void SetDrop();
	__declspec(dllexport) void RemoveDrop();
};

