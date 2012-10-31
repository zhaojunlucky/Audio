#pragma once
#include <stdio.h>
#include <atlbase.h>
#include <assert.h>
#include "global.h"

#include <stdlib.h>
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC

#define FRAMES_ALLOC_SIZE 5
class Tags
{
public:
	__declspec(dllexport) Tags(void);
	__declspec(dllexport) ~Tags(void);
	// -1 for fail,0 for no id3 tag v2,1 for sucess
	//__declspec(dllexport) int getTags(wchar_t *fileName,ID3Header *header,ID3Frame *frames,unsigned int *framesSize);
	//__declspec(dllexport) int getTags(char *fileName,ID3Header *header,ID3Frame *frames,unsigned int *framesSize);
	__declspec(dllexport) int Link(const wchar_t *fileName);
	__declspec(dllexport) int Link(const char *fileName);
	__declspec(dllexport) unsigned int GetFrameNum();
	__declspec(dllexport) ID3Header *GetID3Header();
	__declspec(dllexport) ID3Frame *FindFrame(char *frameID,unsigned int *index);
	__declspec(dllexport) ID3Frame *FindFrame(wchar_t *frameID,unsigned int *index);
	__declspec(dllexport) ID3Frame *GetFrame(unsigned int i);
	__declspec(dllexport) ID3Bitmap *GetHBITMAP(unsigned int i = -1);

private:
	int DecoderID3Header(ID3Header *header);
	int DecoderID3v2_2(ID3Header *header,ID3Frame **frames,unsigned int *fNum);
	int DecoderID3v2_3(ID3Header *header,ID3Frame **frames,unsigned int *fNum);
	int DecoderID3v2_4(ID3Header *header,ID3Frame **frames,unsigned int *fNum);
	unsigned int id3_deunsynchronise(unsigned char *data, unsigned int length);
	int id3_get_picture_unicode(unsigned char *buf, unsigned int len, ID3Frame *frame);
	wchar_t *id3_get_unicode_str(unsigned char *buf, unsigned int *len);
	wchar_t *id3_decode_unicode_str(unsigned char **src, unsigned int *in_size, unsigned int encoding, unsigned int *out_size);
	wchar_t *id3_get_comment_unicode_str(unsigned char *buf, unsigned int *len);
	wchar_t *id3_get_uslt_unicode_str(unsigned char *buf, unsigned int *len);
	wchar_t *id3_get_url_unicode_str(unsigned char *buf, unsigned int *len);
	int id3_get_txxx_unicode_str(unsigned char *buf, unsigned int len,ID3Frame *frame);
	void SafeCloseFile();
	void FreeID3Frames();
private:
	FILE *fp;
	ID3Header header;
	
	ID3Frame *m_frames;
	ID3Frame tmpFrame;
	unsigned int m_frameNum;
	ID3Bitmap *id3Bitmap;
	int CopyID3Frame(ID3Frame *des,const ID3Frame *src);
public:// declare public for test

};