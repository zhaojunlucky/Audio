#pragma once

typedef struct
{
	int width;
	int height;
	void SetSize(int w,int h)
	{
		width = w;
		height = h;
	}
}BMPSIZE;
