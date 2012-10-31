#pragma once
#include "..\Utility\ZString.h"
#include "..\TagLibrary\global.h"
using namespace DDString;
class ID3Info
{
public:
	__declspec(dllexport) ID3Info(void);
	__declspec(dllexport) ~ID3Info(void);

	__declspec(dllexport)  void EmptyTags();
public:
	float duration_times;
	double bitrate;//kbps
	
	// 
	CZString title;
	CZString artist;
	CZString album;
	CZString year;
	CZString comment;
	CZString genre;
	CZString track;

	ID3Bitmap *id3Bitmap;
};

