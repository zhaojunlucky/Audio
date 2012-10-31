#pragma once
#include <vector>

#include "Media.h"

using namespace std;

class CZCueSheet
{
public:
	__declspec(dllexport) CZCueSheet(void);
	__declspec(dllexport) virtual ~CZCueSheet(void);
	__declspec(dllexport) unsigned int GetSize();
	__declspec(dllexport) const Media GetAt(unsigned int);
	__declspec(dllexport) bool PaseSheet(const wchar_t *);
	//__declspec(dllexport) bool PaseSheetW(const wchar_t*);
private:
	void EmptyMedia(Media &m,bool emptyFileName = false)
	{
		m.amblum.Empty();		
		m.end = m.start = 0;
		m.title.Empty();
		if(emptyFileName)
		{
			m.fileName.Empty();
			m.aritst.Empty();
		}
		m.end = -1;
	}

	
private:
	vector < Media > medias;
};

