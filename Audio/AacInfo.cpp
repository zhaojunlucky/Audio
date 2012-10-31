#include "AacInfo.h"
//#include <atlbase.h>
#include "..\Utility\ZString.h"
using namespace DDString;

FILE_STREAM *open_filestream(char *filename)
{
	FILE_STREAM *fs;

	fs = (FILE_STREAM*)LocalAlloc(LPTR, sizeof(FILE_STREAM) + local_buffer_size_ * 1024);

	if(fs == NULL)
		return NULL;

	fs->data = (unsigned char *)&fs[1];
	/*USES_CONVERSION;
	LPCWSTR filepath = A2W(filename);*/
	CZString filepath;
	//filepath.SetData((char*)fs->data ,strlen((char*)fs->data ));
	filepath.SetData(filename,strlen(filename));
	fs->stream = CreateFile(filepath.GetData(), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if (fs->stream == INVALID_HANDLE_VALUE)
	{
		LocalFree(fs);
		return NULL;
	}

	fs->http = 0;
    fs->buffer_length = 0;
    fs->buffer_offset = 0;
    fs->file_offset = 0;

    return fs;
}

unsigned long filelength_filestream(FILE_STREAM *fs)
{
    unsigned long fsize;

    if (fs->http)
    {
        fsize = fs->http_file_length;
    }
	else
	{
        fsize = GetFileSize(fs->stream, NULL);
    }

    return fsize;
}

int read_buffer_filestream(FILE_STREAM *fs, void *data, int length)
{
    int i, tmp;
    unsigned char *data2 = (unsigned char *)data;

    for(i=0; i < length; i++)
    {
        if((tmp = read_byte_filestream(fs)) < 0)
        {
            if(i)
			{
                break;
            }
			else
			{
                return -1;
            }
        }
        data2[i] = tmp;
    }

    return i;
}

int read_byte_filestream(FILE_STREAM *fs)
{
    if(fs->buffer_offset == fs->buffer_length)
    {
        fs->buffer_offset = 0;

       /* if(fs->http)
            fs->buffer_length = recv(fs->inetStream, fs->data, m_stream_buffer_size * 1024, 0);
        else*/
            ReadFile(fs->stream, fs->data, local_buffer_size_ * 1024,(LPDWORD) &fs->buffer_length, 0);

        if(fs->buffer_length <= 0)
        {
            //if(fs->http)
            //{
            //    int x;
            //    x = WSAGetLastError();

            //    if(x == 0)
            //    {
            //        /* Equivalent of a successful EOF for HTTP */
            //    }
            //}

            fs->buffer_length = 0;
            return -1;
        }
    }

    fs->file_offset++;

    return fs->data[fs->buffer_offset++];
}

void close_filestream(FILE_STREAM *fs)
{
    if(fs)
    {
        //if (fs->http)
        //{
        //    if (fs->inetStream)
        //    {
        //        /* The 'proper' way to close a TCP connection */
        //        if(fs->inetStream)
        //        {
        //            CloseTCP(fs->inetStream);
        //        }
        //    }
        //}
        //else
        {
            if(fs->stream)
                CloseHandle(fs->stream);
        }

        LocalFree(fs);
        fs = NULL;
    }
}

unsigned long tell_filestream(FILE_STREAM *fs)
{
    return fs->file_offset;
}

int read_ADIF_header(FILE_STREAM *file, faadAACInfo *info)
{
    int bitstream;
    unsigned char buffer[ADIF_MAX_SIZE];
    int skip_size = 0;
    int sf_idx;

    /* Get ADIF header data */
    info->headertype = 1;

    if(read_buffer_filestream(file, buffer, ADIF_MAX_SIZE) < 0)
		return -1;

    /* copyright string */
    if(buffer[0] & 0x80)
        skip_size += 9; /* skip 9 bytes */

    bitstream = buffer[0 + skip_size] & 0x10;
    info->bitrate = ((unsigned int)(buffer[0 + skip_size] & 0x0F)<<19)|
        ((unsigned int)buffer[1 + skip_size]<<11)|
        ((unsigned int)buffer[2 + skip_size]<<3)|
        ((unsigned int)buffer[3 + skip_size] & 0xE0);

    if (bitstream == 0)
    {
        info->object_type =  ((buffer[6 + skip_size]&0x01)<<1)|((buffer[7 + skip_size]&0x80)>>7);
        sf_idx = (buffer[7 + skip_size]&0x78)>>3;
    } else {
        info->object_type = (buffer[4 + skip_size] & 0x18)>>3;
        sf_idx = ((buffer[4 + skip_size] & 0x07)<<1)|((buffer[5 + skip_size] & 0x80)>>7);
    }
    info->sampling_rate = sample_rates[sf_idx];

    return 0;
}

static int read_ADTS_header(FILE_STREAM *file, faadAACInfo *info,
                            unsigned long **seek_table, int *seek_table_len,
                            int tagsize, int no_seek_table)
{
    /* Get ADTS header data */
    unsigned char buffer[ADTS_MAX_SIZE];
    int frames, framesinsec=0, t_framelength = 0, frame_length, sr_idx, ID;
    int second = 0, pos;
	int i;
    float frames_per_sec = 0;
    unsigned long bytes;
	unsigned long *tmp_seek_table = NULL;

    info->headertype = 2;

    /* Read all frames to ensure correct time and bitrate */
    for(frames=0; /* */; frames++, framesinsec++)
    {
		/* If streaming, only go until we hit 5 seconds worth */

        pos = tell_filestream(file);

        /* 12 bit SYNCWORD */
        bytes = read_buffer_filestream(file, buffer, ADTS_MAX_SIZE);

        if(bytes != ADTS_MAX_SIZE)
        {
            /* Bail out if no syncword found */
            break;
        }

		/* check syncword */
        if (!((buffer[0] == 0xFF)&&((buffer[1] & 0xF6) == 0xF0)))
            break;

        if(!frames)
        {
            /* fixed ADTS header is the same for every frame, so we read it only once */
            /* Syncword found, proceed to read in the fixed ADTS header */
            ID = buffer[1] & 0x08;
            info->object_type = (buffer[2]&0xC0)>>6;
            sr_idx = (buffer[2]&0x3C)>>2;
            info->channels = ((buffer[2]&0x01)<<2)|((buffer[3]&0xC0)>>6);

            frames_per_sec = sample_rates[sr_idx] / 1024.f;
        }

        /* ...and the variable ADTS header */
        if (ID == 0) {
            info->version = 4;
        } else { /* MPEG-2 */
            info->version = 2;
        }
        frame_length = ((((unsigned int)buffer[3] & 0x3)) << 11)
            | (((unsigned int)buffer[4]) << 3) | (buffer[5] >> 5);

        t_framelength += frame_length;

		if(!file->http)
		{
			if(framesinsec == 43)
				framesinsec = 0;

			if(framesinsec == 0 && seek_table_len)
			{
				tmp_seek_table = (unsigned long *) realloc(tmp_seek_table, (second + 1) * sizeof(unsigned long));
				tmp_seek_table[second] = pos;
			}
			if(framesinsec == 0)
				second++;
		}

		/* NOTE: While simply skipping ahead by reading may seem to be more work than seeking,
			it is actually much faster, and keeps compatibility with streaming */
		for(i=0; i < frame_length - ADTS_MAX_SIZE; i++)
        {
			if(read_byte_filestream(file) < 0)
				break;
        }
    }

	if(seek_table_len)
	{
		*seek_table_len = second;
		*seek_table = tmp_seek_table;
	}

    info->sampling_rate = sample_rates[sr_idx];
    info->bitrate = (int)(((t_framelength / frames) * (info->sampling_rate/1024.0)) +0.5)*8;

	//if(file->http)
	//{
	//	/* Since we only use 5 seconds of aac data to get a rough bitrate, we must use a different
	//	   method of calculating the overall length */
	//	if(filelength_filestream(file))
	//	{
	//		info->length = (int)((filelength_filestream(file)/(((info->bitrate*8)/1024)*16))*1000);
	//	}
	//	else
	//	{
	//		/* Since the server didnt tell us how long the file is, 
	//		   we have no way of determining length */
	//		info->length = 0;
	//	}
	//}
	//else
	{
		info->length = (int)((float)(frames/frames_per_sec))*1000;
	}
    return 0;
}

int get_AAC_format(char *filename, faadAACInfo *info,
                   unsigned long **seek_table, int *seek_table_len,
                   int no_seek_table)
{
    unsigned long tagsize;
    FILE_STREAM *file;
	char buffer[10];
    unsigned long file_len;
    unsigned char adxx_id[5];
    unsigned long tmp;

    memset(info, 0, sizeof(faadAACInfo));

    file = open_filestream(filename);

    if(file == NULL)
        return -1;

    file_len = filelength_filestream(file);

    /* Skip the tag, if it's there */
    tmp = read_buffer_filestream(file, buffer, 10);

    if (StringComp(buffer, "ID3", 3) == 0)
	{
		unsigned int i;

        /* high bit is not used */
        tagsize = (buffer[6] << 21) | (buffer[7] << 14) |
            (buffer[8] <<  7) | (buffer[9] <<  0);

        for(i=0; i < tagsize; i++)
			if(read_byte_filestream(file) < 0)
			{
				close_filestream(file);
				return -1;
			}

        tagsize += 10;
    }
	else
	{
        tagsize = 0;

		/* Simple hack to reset to the beginning */
		file->buffer_offset = 0;
		file->file_offset = 0;
    }

	if(file_len)
	    file_len -= tagsize;

    tmp = read_buffer_filestream(file, adxx_id, 2);
    //seek_filestream(file, tagsize, FILE_BEGIN);

    adxx_id[5-1] = 0;
    info->length = 0;

	/* Determine the header type of the file, check the first two bytes */
    if(StringComp((char*)adxx_id, "AD", 2) == 0)
    {
		/* We think its an ADIF header, but check the rest just to make sure */
		tmp = read_buffer_filestream(file, adxx_id + 2, 2);
		
		if(StringComp((char*)adxx_id, "ADIF", 4) == 0)
		{
			//read_ADIF_header(file, info);
			close_filestream(file);
			return -1;
		}
    }
    else
    {
		/* No ADIF, check for ADTS header */
        if ((adxx_id[0] == 0xFF)&&((adxx_id[1] & 0xF6) == 0xF0))
        {
			/* ADTS  header located */
			/* Since this routine must work for streams, we can't use the seek function to go backwards, thus 
			   we have to use a quick hack as seen below to go back where we need to. */
			
			if(file->buffer_offset >= 2)
			{
				// simple seeking hack, though not really safe, the probability of it causing a problem is low.
				file->buffer_offset -= 2;
				file->file_offset -= 2;
			}

            read_ADTS_header(file, info, seek_table, seek_table_len, tagsize,
                no_seek_table);
        }
        else
        {
            /* Unknown/headerless AAC file, assume format: */
            info->version = 2;
            info->bitrate = 128000;
            info->sampling_rate = 44100;
            info->channels = 2;
            info->headertype = 0;
            info->object_type = 1;
			close_filestream(file);
			return -1;
        }
    }

    close_filestream(file);

    return 0;
}

int StringComp(char const *str1, char const *str2, unsigned long len)
{
    signed int c1 = 0, c2 = 0;

    while (len--) {
        c1 = *str1++;
        c2 = *str2++;

        if (c1 == 0 || c1 != c2)
            break;
    }

    return c1 - c2;
}