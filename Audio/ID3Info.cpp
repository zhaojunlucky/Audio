#include "ID3Info.h"


ID3Info::ID3Info(void)
{
	bitrate = 0;
	id3Bitmap = (ID3Bitmap*)malloc(sizeof(ID3Bitmap));
	memset(id3Bitmap,0,sizeof(ID3Bitmap));
}


ID3Info::~ID3Info(void)
{
	if(id3Bitmap)
	{
		if(id3Bitmap->hbm)
			::DeleteObject(id3Bitmap->hbm);
		free(id3Bitmap);
	}
}

void ID3Info::EmptyTags()
{
	title.Empty();
	artist.Empty();
	album.Empty();
	year.Empty();
	comment.Empty();
	genre.Empty();
	track.Empty();

	if(id3Bitmap)
	{
		if(id3Bitmap->hbm)
			::DeleteObject(id3Bitmap->hbm);
		memset(id3Bitmap,0,sizeof(ID3Bitmap));
	}
}