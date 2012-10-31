#pragma once
#include "..\ZLPCore\AudioBase.h"
class Media
{
public:

	__declspec(dllexport) Media()
	{
		fileName = "";
		title = "";
		amblum = "";
		aritst = "";
		mediaFormat = AutoDetect ;
		isCue = false;
		isSingleFile = false;
		start = 0;
		end = 0;
		track = 0;
		bitrate = 0;
		samplingRate = 0;
	}
	__declspec(dllexport) virtual ~Media()
	{
		fileName.Empty();
		title.Empty();
		amblum.Empty();
		aritst.Empty();
	}

	CString		fileName;
	CString		title;
	CString		amblum;
	CString		aritst;
	AudioFormat mediaFormat;
	// Cue media info
	bool isCue;
	double start;
	double end;
	bool isSingleFile;	
	int track;
	double bitrate;
	double samplingRate;

	__declspec(dllexport) Media(const Media& m)
	{
		fileName = m.fileName;
		title = m.title;
		amblum = m.amblum;
		aritst = m.aritst;
		mediaFormat = m.mediaFormat;
		isCue = m.isCue;
		isSingleFile = m.isSingleFile;
		start = m.start;
		end = m.end;
		bitrate = m.bitrate;
		samplingRate = m.samplingRate;
	}
	__declspec(dllexport) Media &operator=(const Media& m)
	{
		if(this != &m)
		{
			fileName = m.fileName;
			title = m.title;
			amblum = m.amblum;
			aritst = m.aritst;
			mediaFormat = m.mediaFormat;
			isCue = m.isCue;
			isSingleFile = m.isSingleFile;
			start = m.start;
			end = m.end;
			bitrate = m.bitrate;
			samplingRate = m.samplingRate;
		}
		return *this;
	}
};