#pragma once
#include <vector>
#include "Media.h"

using namespace std;



class CZPlayList
{
public:
	__declspec(dllexport) CZPlayList(void);
	__declspec(dllexport) virtual ~CZPlayList(void);

	__declspec(dllexport) void AddItem(Media);
	__declspec(dllexport) void InsertItem(int,Media);
	__declspec(dllexport) void RemoveAt(int);
	__declspec(dllexport) void RemoveAll();
	__declspec(dllexport) Media GetItem(int);
	__declspec(dllexport) int GetSize();
	__declspec(dllexport) void RemoveAt(int start,int end);
	__declspec(dllexport) int Contains(const Media &m);
	__declspec(dllexport) bool Update(unsigned int,const Media &m);
private:
	vector<Media> list;
};

