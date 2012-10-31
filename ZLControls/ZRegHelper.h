#pragma once

class CZRegHelper
{
public:
	__declspec(dllexport) CZRegHelper ();
	__declspec(dllexport) virtual ~CZRegHelper() ;
	__declspec(dllexport) void RegisterFileRelation(const wchar_t *ext,const wchar_t *appName,
		const wchar_t *appKey,const wchar_t *defaultIcon,const wchar_t * des);
	__declspec(dllexport) BOOL CheckFileRelation(const wchar_t *ext,const wchar_t *appKey);
	__declspec(dllexport) void UnRegisterFileRelation(const wchar_t *ext,const wchar_t *appKey);
	__declspec(dllexport) void ParseReg(wchar_t **arg,int num);
};