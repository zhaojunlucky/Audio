#include "Tags.h"
//#include "include\ximage.h"
#include "ImageProcess.h"

Tags::Tags(void)
{
	fp = NULL;
	memset(&header,0,sizeof(ID3Header));
	m_frames = NULL;
	id3Bitmap = (ID3Bitmap*)malloc(sizeof(ID3Bitmap));
	memset(id3Bitmap,0,sizeof(ID3Bitmap));
	memset(&tmpFrame,0,sizeof(ID3Frame));
	m_frameNum = 0;
}


Tags::~Tags(void)
{
	SafeCloseFile();
	FreeID3Frames();
	if(id3Bitmap)
	{
		if(id3Bitmap->hbm)
			::DeleteObject(id3Bitmap->hbm);
		free(id3Bitmap);
	}
	if(tmpFrame.data)
		free(tmpFrame.data);
	if(tmpFrame.other)
		free(tmpFrame.other);
	if(tmpFrame.Picture.mime)
		free(tmpFrame.Picture.mime);
	if(tmpFrame.Picture.picDes)
		free(tmpFrame.Picture.picDes);
	if(tmpFrame.Picture.picSize)
		free(tmpFrame.Picture.picSize);
	if(tmpFrame.Picture.picType)
		free(tmpFrame.Picture.picType);
	//_CrtDumpMemoryLeaks();
}

void Tags::FreeID3Frames()
{
	if(m_frames)
	for(unsigned int i = 0;i < m_frameNum;i++)
	{
		if(m_frames[i].data)
		{
			free(m_frames[i].data);
			m_frames[i].data = NULL;
		}
		if(0 == wcscmp(m_frames[i].FrameID ,L"TXXX"))
		{
			if(m_frames[i].other)
			{
				free(m_frames[i].other);
			}
		}
		if(0 == wcscmp(m_frames[i].FrameID ,L"APIC"))
		{
			if(m_frames[i].Picture.mime)
			{
				free(m_frames[i].Picture.mime);
			}
			if(m_frames[i].Picture.picDes)
			{
				free(m_frames[i].Picture.picDes);
			}
			if(m_frames[i].Picture.picSize)
			{
				free(m_frames[i].Picture.picSize);
			}
			if(m_frames[i].Picture.picType)
			{
				free(m_frames[i].Picture.picType);
			}
		}
	}
	free(m_frames);
}

int Tags::Link(const char *fileName)
{
	if(id3Bitmap)
		if(id3Bitmap->hbm)
		{
			DeleteObject(id3Bitmap->hbm);
			id3Bitmap->hbm = NULL;
		}
	//memset(id3Bitmap,0,sizeof(ID3Bitmap));
	SafeCloseFile();
	if(m_frames)
	{
		FreeID3Frames();
		m_frames = NULL;
	}
	//fp = fopen(fileName,"rb");
	if(0==strlen(fileName))
		return -1;
	fopen_s(&fp,fileName,"rb");
	if(NULL == fp)
	{
		return -1;
	}
	
	m_frameNum = 0;
	int ret = DecoderID3Header(&header);

	if(1 == ret)
	{
		switch(header.MajorVersion)
		{
		case 2:
			return DecoderID3v2_2(&header,&m_frames,&m_frameNum);
		case 3 :
			return DecoderID3v2_3(&header,&m_frames,&m_frameNum);
		case 4:
			return DecoderID3v2_4(&header,&m_frames,&m_frameNum);
		default:
			return -1;
		}
	}
	return 0;
}

ID3Header *Tags::GetID3Header()
{
	return &header;
}

int Tags::Link(const wchar_t *fileName)
{
	USES_CONVERSION;
	return Link(W2A(fileName));
}

int Tags::DecoderID3Header(ID3Header *header)
{
	if(NULL == fp)
		return -1;
	fseek(fp,0,SEEK_SET);
	char buffer[10];
	fread(buffer,HEADER_SIZE,1,fp);
	if(0 != strncmp(buffer,"ID3",3))
	{
		// there is no id3 tag
		SafeCloseFile();
		return 0;
	}
		// invalid major version and revision number
	if(buffer[3] == 0xFF || buffer[4] == 0xFF)
		return -1;

	// invalid tag size
	if(buffer[6] >= 0x80 || buffer[7] >= 0x80 || buffer[8] >= 0x80 || buffer[9] >= 0x80)
		return -1;

	memset(header,0,sizeof(ID3Header));
	header->MajorVersion = buffer[3];
	header->RevisionNumber = buffer[4];
	header->nFlags = buffer[5];
	header->TFlags.Unsync = GetBit(buffer[5], 7);
	header->TFlags.Extended = GetBit(buffer[5], 6);
	header->TFlags.Experimental = GetBit(buffer[5], 5);
	header->TFlags.Footer = GetBit(buffer[5], 4);

	header->TagSize = DecodeTagSize(buffer[6],buffer[7],buffer[8],buffer[9]);
	header->TotalSize = header->TagSize + HEADER_SIZE;
	fseek(fp,0,SEEK_END);
	long fileSize = ftell(fp);
	if((long)header->TotalSize >= fileSize)
	{
		SafeCloseFile();
		return -1;
	}

	return 1;
}

int Tags::DecoderID3v2_2(ID3Header *header,ID3Frame **frames,unsigned int *fNum)
{	
	ID3Frame *tmp_frames;
	unsigned int allocSize = FRAMES_ALLOC_SIZE;
	unsigned int size = 0;
	unsigned int i = 0;
	tmp_frames = (ID3Frame *)malloc(FRAMES_ALLOC_SIZE * sizeof(ID3Frame));
	if(!tmp_frames)
	{
		return -1;
	}
	if(header->TFlags.Extended) // compression not supported
		return 0;
	fseek(fp,HEADER_SIZE,SEEK_SET);
	unsigned int tmpBufSize = header->TagSize;
	unsigned char *buf = new unsigned char[tmpBufSize];
	if(NULL == buf)
	{
		return -1;
	}
	
	fread(buf,header->TagSize,1,fp);
	SafeCloseFile();
	size = tmpBufSize;
	if(header->TFlags.Unsync)
	{
		size = id3_deunsynchronise(buf,tmpBufSize);
	}
	unsigned char *ptr;
	ptr = buf;

	unsigned int frameLen ;
	USES_CONVERSION;
	char id[4];
	while(size > 6 || *ptr != 0)
	{
		if(ptr[3] > 0x80 || ptr[4] > 0x80 || ptr[5] > 0x80)
			break;
		frameLen = DecodeFrameSize2(ptr[3], ptr[4], ptr[5]);
		if(frameLen + 6  > size)
			break;

		if(i >= allocSize)
		{
			tmp_frames = (ID3Frame *)realloc(tmp_frames,(allocSize + FRAMES_ALLOC_SIZE)*sizeof(ID3Frame));
			allocSize += FRAMES_ALLOC_SIZE;
		}
		memset(&tmp_frames[i],0,sizeof(ID3Frame));
		strncpy_s(id,(char*)ptr,3);
		id[3] = '\0';
		wcscpy_s(tmp_frames[i].FrameID,A2W(id));
		tmp_frames[i].FrameID[3] = '\0';

		unsigned int data_length = frameLen;
		FrameType fType = getFrameType(tmp_frames[i].FrameID);
		ptr += 6;
		size -= 6;
		switch(fType)
		{
			case UFI:
				{
					break;
				}
			case PIC:
				{
					break;
				}
			case TT1:
			case TT2:
			case TT3:
			case TP1:
			case TP2:
			case TP3:
			case TP4:
			case TCM:
			case TXT:
			case TLA:
			case TCO:
			case TAL:
			case TPA:
			case TRK:
			case TRC:
			case TYE:
			case TDA:
			case TIM:
			case TRD:
			case TMT:
			case TFT:
			case TBP:
			case TCR:
			case TPB:
			case TEN:
			case TSS:
			case TOF:
			case TLE:
			case TSI:
			case TDY:
			case TKE:
			case TOT:
			case TOA:
			case TOL:
			case TOR:
			case TXX:
				{
					tmp_frames[i].data = id3_get_unicode_str(ptr, &data_length);
					break;
				}
			case COM:
			case WXX:
				{
					tmp_frames[i].data = id3_get_comment_unicode_str(ptr,&data_length);
					break;
				}
			case ULT:
				{
					tmp_frames[i].data = id3_get_uslt_unicode_str(ptr,&data_length);
					break;
				}
			case UNKNOWN:
				i--;break;
			default:break;
		}

		ptr += frameLen;
		size -= frameLen;
		i++;
	}
	delete []buf;
	*frames = tmp_frames;
	*fNum = i;
	return 0;
}
// frames must be a null pointer
int Tags::DecoderID3v2_3(ID3Header *header,ID3Frame **frames,unsigned int *fNum)
{
	ID3Frame *tmp_frames;
	unsigned int allocSize = FRAMES_ALLOC_SIZE;
	unsigned int size = 0;
	unsigned int i = 0;
	tmp_frames = (ID3Frame *)malloc(FRAMES_ALLOC_SIZE * sizeof(ID3Frame));
	if(!tmp_frames)
	{
		return -1;
	}
	fseek(fp,HEADER_SIZE,SEEK_SET);
	unsigned int tmpBufSize = header->TagSize;
	unsigned char *buf = new unsigned char[tmpBufSize];
	if(NULL == buf)
	{
		return -1;
	}
	fread(buf,header->TagSize,1,fp);
	SafeCloseFile();
	size = tmpBufSize;
	if(header->TFlags.Unsync)
	{
		size = id3_deunsynchronise(buf,tmpBufSize);
	}
	unsigned char *ptr;
	ptr = buf;
	
	if(header->TFlags.Extended)
	{
		unsigned int pading = (ptr[6] << 24) + (ptr[7] << 16) +(ptr[8] << 8) + (ptr[9]);
		size = size - 10 - ptr[3];
		ptr = ptr + 10 + ptr[3];
	}
	unsigned int frameLen = 0;
	USES_CONVERSION;
	char id[5];
	while(size > 10 && *ptr != 0)
	{
		frameLen = DecodeFrameSize3(ptr[4], ptr[5], ptr[6], ptr[7]);
		if(frameLen > size + 10)
			break;

		if(i >= allocSize)
		{
			tmp_frames = (ID3Frame *)realloc(tmp_frames,(allocSize + FRAMES_ALLOC_SIZE)*sizeof(ID3Frame));
			allocSize += FRAMES_ALLOC_SIZE;
		}
		memset(&tmp_frames[i],0,sizeof(ID3Frame));
		
		
		strncpy_s(id,(char *)ptr,4);
		id[4] = '\0';
		wcscpy_s(tmp_frames[i].FrameID,A2W(id));
		tmp_frames[i].FrameID[4] = '\0';
		// check Grouping identity flag
		unsigned int nExtraSize = 0;
		
		tmp_frames[i].FFlags.GroupingIdentity = GetBit(ptr[9], 5);
		if(tmp_frames[i].FFlags.GroupingIdentity/*GetBit(ptr[9], 5)*/)
			nExtraSize++;
		

		int fExit = 0;
		// check compression flag
		tmp_frames[i].FFlags.Compressed = GetBit(ptr[9], 7);
		if(tmp_frames[i].FFlags.Compressed/*GetBit(ptr[9], 7)*/)
		{
			nExtraSize += 4;
			fExit = 1;
		}
		
		// check encryption flag
		tmp_frames[i].FFlags.Encryption = GetBit(ptr[9], 6);
		if(tmp_frames[i].FFlags.Encryption/*GetBit(ptr[9], 6)*/)
		{
			nExtraSize++;
			fExit = 1;
		}	
			

		if(nExtraSize > size)
			break;
	

		// check if we have unsupported flags
		if(fExit)
		{
			
			ptr += 10;
			size -= 10;
			ptr += nExtraSize;
			size -= nExtraSize;

			ptr += frameLen;
			size -= frameLen;
			
			continue;	// compression and encryption not supported
		}

		unsigned int data_length = frameLen;

		FrameType fType = getFrameType(tmp_frames[i].FrameID);
		ptr += 10;
		size -= 10;
		ptr += nExtraSize;
		size -= nExtraSize;
		switch(fType)
		{
			case UFID:
				{
					break;
				}
			case APIC:
				{
					id3_get_picture_unicode(ptr,data_length,&tmp_frames[i]);
					break;
				}
			case TALB: //ablum
			case TCOM:
			case TIT2:
			case TOPE:
			case TPUB:
			case TPE1:
			case TCON:
			case TBPM:
			case TENC:
			case TCOP:
			case TPE2:
			case TRCK:
			case TYER:
			case TDLY:
			case TEXT:
			case TFLT:
			case TIME:
			case TIT1:
			case TIT3:
			case TKEY:
			case TLAN:
			case TLEN:
			case TMED:
			case TOAL:
			case TOFN:
			case TOLY:
			case TORY:
			case TOWN:
			case TPE4:
			case TPE3:
			case TPOS:
			case TRDA:
			case TRSN:
			case TRSO:
			case TSIZ:
			case TSRC:
			case TSSE:
			case IPLS:
				{
					tmp_frames[i].data = id3_get_unicode_str(ptr,&data_length);
					break;
				}
			case TXXX:
				{
					//wchar_t *tdata = id3_get_unicode_str(ptr,&data_length);
					//tmp_frames[i].data = id3_get_unicode_str(ptr,&data_length);
					id3_get_txxx_unicode_str(ptr,data_length,&tmp_frames[i]);
					break;
				}
				//
			case WCOM:
			case WCOP:
			case WOAF:
			case WOAS:
			case WORS:
			case WPAY:
			case WPUB:
			case WXXX:
				//
				{
					tmp_frames[i].data = id3_get_url_unicode_str(ptr,&data_length);
					break;
				}
			case USLT:
				{
					tmp_frames[i].data = id3_get_uslt_unicode_str(ptr,&data_length);
					break;
				}
			case COMM:
			
				{
					tmp_frames[i].data = id3_get_comment_unicode_str(ptr,&data_length);
					break;
				}
			case UNKNOWN:
				i--;break;
			default:break;
		}

		ptr += frameLen;
		size -= frameLen;
		i++;
	}
	delete []buf;
	*frames = tmp_frames;
	*fNum = i;
	return 0;
}

int Tags::DecoderID3v2_4(ID3Header *header,ID3Frame **frames,unsigned int *fNum)
{
	ID3Frame *tmp_frames;
	unsigned int allocSize = FRAMES_ALLOC_SIZE;
	unsigned int size = 0;
	unsigned int i = 0;
	tmp_frames = (ID3Frame *)malloc(FRAMES_ALLOC_SIZE * sizeof(ID3Frame));
	if(!tmp_frames)
	{
		return -1;
	}
	fseek(fp,HEADER_SIZE,SEEK_SET);
	unsigned int tmpBufSize = header->TagSize;
	unsigned char *buf = new unsigned char[tmpBufSize];
	if(NULL == buf)
	{
		return -1;
	}
	fread(buf,header->TagSize,1,fp);
	SafeCloseFile();
	size = tmpBufSize;
	unsigned char *ptr;
	ptr = buf;
	if(header->TFlags.Extended)
	{
		
		if(ptr[0] > 0x80 || ptr[1] > 0x80 || ptr[2] > 0x80 || ptr[3] > 0x80)
			return 0;

		unsigned int ext_size = DecodeFrameSize4(ptr[0], ptr[1], ptr[2], ptr[3]);
		if(ext_size < 6 || ext_size > size)
			return 0;

		size -= ext_size;
		ptr += ext_size;
	}
	unsigned int frameLen;
	USES_CONVERSION;
	char id[5];
	while(size > 10 && *ptr != 0)
	{
		if(ptr[4] > 0x80 || ptr[5] > 0x80 || ptr[6] > 0x80 || ptr[7] > 0x80)
			break;

		frameLen = DecodeFrameSize4(ptr[4], ptr[5], ptr[6], ptr[7]);
		if(frameLen  > size + 10)
			break;

		if(i >= allocSize)
		{
			tmp_frames = (ID3Frame *)realloc(tmp_frames,(allocSize + FRAMES_ALLOC_SIZE)*sizeof(ID3Frame));
			allocSize += FRAMES_ALLOC_SIZE;
		}
		memset(&tmp_frames[i],0,sizeof(ID3Frame));
		strncpy_s(id,(char*)ptr,4);
		id[4] = '\0';
		wcscpy_s(tmp_frames[i].FrameID,A2W(id));
		tmp_frames[i].FrameID[4] = '\0';

		unsigned int nExtraSize = 0;

		tmp_frames[i].FFlags.GroupingIdentity = GetBit(ptr[9], 6);
		if(tmp_frames[i].FFlags.GroupingIdentity/*GetBit(ptr[9], 6)*/) // check Grouping identity flag
			nExtraSize++;

		tmp_frames[i].FFlags.DataLengthIndicator = GetBit(ptr[9], 0);
		if(tmp_frames[i].FFlags.DataLengthIndicator/*GetBit(ptr[9], 0)*/)	// data length indicator
			nExtraSize += 4;

		int fExit = 0;
		// check compression flag
		tmp_frames[i].FFlags.Compressed = GetBit(ptr[9], 3);
		if(tmp_frames[i].FFlags.Compressed/*GetBit(ptr[9], 3)*/)
		{
			fExit = 1;
		}
		
		// check encryption flag
		tmp_frames[i].FFlags.Encryption = GetBit(ptr[9], 2);
		if(tmp_frames[i].FFlags.Encryption/*GetBit(ptr[9], 2)*/)
		{
			nExtraSize += 1;
			fExit = 1;
		}	
			

		if(nExtraSize > size)
			break;

		// check if we have unsupported flags
		if(fExit)
		{
			
			ptr += 10;
			size -= 10;
			ptr += frameLen;
			size -= frameLen;
			
			continue;	// compression and encryption not supported
		}


		unsigned int data_length = frameLen;
		tmp_frames[i].FFlags.Unsynchronisation = GetBit(ptr[9], 1);//
		//int unsync = GetBit(ptr[9], 1); // unsync flag
		if(tmp_frames[i].FFlags.Unsynchronisation)
		{
			data_length = id3_deunsynchronise(ptr + 10, frameLen - 10);
		}
FrameType fType = getFrameType(tmp_frames[i].FrameID);
		ptr += 10;
		size -= 10;
		ptr += nExtraSize;
		size -= nExtraSize;
		switch(fType)
		{
			case UFID:
				{
					break;
				}
			case APIC:
				{
					id3_get_picture_unicode(ptr,data_length,&tmp_frames[i]);
					break;
				}
			case TALB: //ablum
			case TCOM:
			case TIT2:
			case TOPE:
			case TPUB:
			case TPE1:
			case TCON:
			case TBPM:
			case TENC:
			case TCOP:
			case TPE2:
			case TRCK:
			case TYER:
			case TDLY:
			case TEXT:
			case TFLT:
			case TIME:
			case TIT1:
			case TIT3:
			case TKEY:
			case TLAN:
			case TLEN:
			case TMED:
			case TOAL:
			case TOFN:
			case TOLY:
			case TORY:
			case TOWN:
			case TPE4:
			case TPE3:
			case TPOS:
			case TRDA:
			case TRSN:
			case TRSO:
			case TSIZ:
			case TSRC:
			case TSSE:
			case IPLS:
			//case TXXX:
			case TDEN:
			case TDOR:
			case TDRC:
			case TDRL:
			case TDTG:
			case TIPL:
			case TMCL:
			case TMOO:
			case TPRO:
			case TSOA:
			case TSOP:
			case TSOT:
			case TSST:
				{
					tmp_frames[i].data = id3_get_unicode_str(ptr,&data_length);
					break;
				}
			case TXXX:
				{
					id3_get_txxx_unicode_str(ptr,data_length,&tmp_frames[i]);
					break;
				}
				//
			case WCOM:
			case WCOP:
			case WOAF:
			case WOAS:
			case WORS:
			case WPAY:
			case WPUB:
			case WXXX:
				//
				{
					tmp_frames[i].data = id3_get_url_unicode_str(ptr,&data_length);
					break;
				}
				
			case USLT:
				{
					tmp_frames[i].data = id3_get_uslt_unicode_str(ptr,&data_length);
					break;
				}
			case COMM:
			
				{
					tmp_frames[i].data = id3_get_comment_unicode_str(ptr,&data_length);
					break;
				}
			case UNKNOWN:
				i--;break;
			default:break;
		}

		ptr += frameLen;
		size -= frameLen;
		i++;

	}
	delete []buf;
	*frames = tmp_frames;
	*fNum = i;
	return 0;
}

wchar_t *Tags::id3_get_unicode_str(unsigned char *buf, unsigned int *len)
{

	unsigned int length = *len;
	if(length < 2)
		return NULL;

	length--;

	unsigned int enc = buf[0];
	unsigned int out_size;
	unsigned char *ptr = buf + 1;
	wchar_t *str = id3_decode_unicode_str(&ptr, &length, enc, &out_size);
	*len = out_size;
	return str;
	
}

// decode src string, allocate dest memory and copy string into dest memory
wchar_t *Tags::id3_decode_unicode_str(unsigned char **src, unsigned int *in_size, unsigned int encoding, unsigned int *out_size)
{

	unsigned char *str = *src;
	unsigned char *dest = NULL;
	wchar_t *out = NULL;
	unsigned int block_length = *in_size;
	*out_size = 0;
	switch(encoding)
	{
		case 0:  // Latin1
		{
			// determine length of string
			unsigned int i;
			unsigned int len = 0;
			for(i = 0; i < block_length; i++)
			{
				if(str[i] == 0)
					break;

				len++;
			}

			dest = (unsigned char*) malloc(len + 1);
			if(dest == NULL)
				return NULL;

			memcpy(dest, str, len);
			dest[len] = 0;

			unsigned int null_terminated = 0;

			if(len < block_length && str[len] == 0) // null terminated
				null_terminated = 1;

			*src = str + len + null_terminated;
			*in_size = block_length - len - null_terminated;

			// convert to unicode

			int size = 	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*) dest, -1, NULL, 0);
			out = (wchar_t*) malloc((size + 1) * sizeof(wchar_t));
			if(out == NULL)
			{
				free(dest);
				return NULL;
			}

			// null terminate out
			out[0] = 0;
			size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*) dest, -1, out, size);
			out[size] = 0;
			
			free(dest);
			*out_size = size;

		}
		return out;


		case 1: // UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM
		{
			// make internal copy

			unsigned char *internal_copy = NULL;

			if(block_length < 2)
				return NULL;

			unsigned char *ptr = str;

			// determine byte order

			int byteorder = (( str[0] << 8 ) | str[1] );
			unsigned int header_size = 0;

			if (byteorder == 0xFFFE) // LE
			{
				ptr += 2;
				block_length -= 2;
				header_size = 2;
				
			}
			else if (byteorder == 0xFEFF)  // BE
			{
				unsigned char *internal_copy = (unsigned char*) malloc(block_length);
				if(internal_copy == NULL)
					return NULL;

				memcpy(internal_copy, str, block_length);

				ptr = internal_copy;


				ptr += 2;
				block_length -= 2;
				header_size = 2;

				// determine string size

				unsigned int i;
				unsigned int len = 0;

				unsigned short *ch = (unsigned short*) ptr;
				for(i = 0; i < block_length / 2; i++)
				{
					if(ch[i] == 0)
						break;
						
					len++;	
				}

				for(i = 0; i < len * 2; i += 2) // swap bytes
				{
					unsigned char c;
					c = ptr[i];
					ptr[i] = ptr[i + 1];
					ptr[i + 1] = c;	
				}	
			}


			// determine string length
			unsigned int i;
			unsigned int len = 0;

			unsigned short *ch = (unsigned short*) ptr;
			for(i = 0; i < block_length / 2; i++)
			{
				if(ch[i] == 0)
					break;
						
				len++;	
			}

			out = (wchar_t*) malloc((len + 1) * sizeof(wchar_t));
			if(out == NULL)
			{
				if(internal_copy)
					free(internal_copy);

				return NULL;
			}

			// copy 
			ch = (unsigned short*) ptr;
			for(i = 0; i < len; i++)
			{
				out[i] = ch[i];	
			}

			out[len] = 0;	// null terminate string

			if(internal_copy)
				free(internal_copy);

		
			unsigned int null_terminated = 0;

			if(len < block_length && ptr[len] == 0) // null terminated
				null_terminated = 2;

			*src = str + header_size + len + null_terminated;
			*in_size = block_length - len - null_terminated;

			*out_size = len;


		}
		return out;


		case 2:  // UTF-16 [UTF-16] encoded Unicode [UNICODE] without BOM
		{

			// determine string length
			unsigned int i;
			unsigned int len = 0;

			unsigned short *ch = (unsigned short*) str;
			for(i = 0; i < block_length / 2; i++)
			{
				if(ch[i] == 0)
					break;
						
				len++;	
			}

			out = (wchar_t*) malloc((len + 1) * sizeof(wchar_t));
			if(out == NULL)
				return NULL;
		

			// copy 
			ch = (unsigned short*) str;
			for(i = 0; i < len; i++)
			{
				out[i] = ch[i];	
			}

			out[len] = 0;	// null terminate string

	
			unsigned int null_terminated = 0;

			if(len < block_length && str[len] == 0) // null terminated
				null_terminated = 2;

			*src = str + len + null_terminated;
			*in_size = block_length - len - null_terminated;

			*out_size = len;
		

		}
		return out;


		case 3: // UTF-8 [UTF-8] encoded Unicode [UNICODE]. Terminated with $00.
		{

			// determine length
			unsigned int i;
			unsigned int len = 0;
			for(i = 0; i < block_length; i++)
			{
				if(str[i] == 0)
					break;

				len++;
			}


			int size = MultiByteToWideChar(CP_UTF8,  0, (char*) str, len, NULL, 0);
			out = (wchar_t*) malloc((size + 1) * sizeof(wchar_t));
			if(out == NULL)
				return NULL;

			out[0] = 0;

			size = MultiByteToWideChar(CP_UTF8,  0, (char*) str, len, out, size);
			out[size] = 0;

			unsigned int null_terminated = 0;

			if(len < block_length && str[len] == 0) // null terminated
				null_terminated = 1;

			*src = str + len + null_terminated;
			*in_size = block_length - len - null_terminated;

			*out_size = size;

		}
		return out;


		default:
		return NULL;
	}

}


unsigned int Tags::id3_deunsynchronise(unsigned char *data, unsigned int length)
{
  unsigned char  *old;
  unsigned char *end = data + length;
  unsigned char *new_data;

  if (length == 0)
    return 0;

  for (old = new_data = data; old < end - 1; ++old)
  {
    *new_data++ = *old;
    if (old[0] == 0xff && old[1] == 0x00)
      ++old;
  }

  *new_data++ = *old;

  return new_data - data;
}

void Tags::SafeCloseFile()
{
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
}

wchar_t *Tags::id3_get_url_unicode_str(unsigned char *buf, unsigned int *len)
{

	if(*len < 1)
		return NULL;

	unsigned int enc = buf[0];
	unsigned int out_size;
	unsigned char *ptr = buf + 1;
	unsigned int length = *len - 1;
	wchar_t *str = id3_decode_unicode_str(&ptr, &length, enc, &out_size);
	if(str)
		free(str);

	str = id3_decode_unicode_str(&ptr, &length, 0, &out_size);
	*len = out_size;
	return str;
}

int Tags::id3_get_txxx_unicode_str(unsigned char *buf, unsigned int len,ID3Frame *frame)
{

	if(len < 1)
		return NULL;

	unsigned int enc = buf[0];
	unsigned int out_size;
	unsigned char *ptr = buf + 1;
	unsigned int length = len - 1;
	wchar_t *str = id3_decode_unicode_str(&ptr, &length, enc, &out_size);
	frame->other = str;

	str = id3_decode_unicode_str(&ptr, &length, 0, &out_size);
	//*len = out_size;
	frame->data = str;
	return 1;
}

wchar_t *Tags::id3_get_comment_unicode_str(unsigned char *buf, unsigned int *len)
{
	if(*len < 5)
		return NULL;

	unsigned int enc = buf[0];
	unsigned int out_size;
	unsigned char *ptr = buf + 4;
	unsigned int length = *len - 4;
	wchar_t *str = id3_decode_unicode_str(&ptr, &length, enc, &out_size);
	if(str)
		free(str);

	str = id3_decode_unicode_str(&ptr, &length, enc, &out_size);

	*len = out_size;

	return str;

}

int Tags::id3_get_picture_unicode(unsigned char *buf, unsigned int len, ID3Frame *frame)
{
	if(len < 3)
		return 0;

	unsigned int enc = buf[0];
	unsigned int out_size = 0;

	buf++;
	len--;

	wchar_t *str = id3_decode_unicode_str(&buf, &len, 0, &out_size);
	/*wchar_t **field = &fields[ID3_INFO_PICTURE_MIME];

	if(*field)
		free(*field);*/

	//*field = str;
	frame->Picture.mime = str;

	wchar_t *ptype = (wchar_t*) malloc(12 * sizeof(wchar_t));
	if(ptype)
	{	
		//field = &fields[ID3_INFO_PICTURE_TYPE];

		unsigned int t = buf[0];
		swprintf(ptype, L"%u", t);

		/*if(*field)
			free(*field);*/

		//*field =  ptype;
		frame->Picture.picType = ptype;
	}


	buf++;
	len--;

	if(len < 1)
		return 1;

	//field = &fields[ID3_INFO_PICTURE_DESCRIPTION];
	str = id3_decode_unicode_str(&buf, &len, enc, &out_size);
	/*if(*field)
		free(*field);*/

	//*field =  str;
	frame->Picture.picDes = str;
	//field = &fields[ID3_INFO_PICTURE_DATA];

	wchar_t *size = (wchar_t*) malloc(12 * sizeof(wchar_t));
	if(size == NULL)
		return 1;	

	//wchar_t **field1 = &fields[ID3_INFO_PICTURE_DATA_SIZE];
	swprintf(size, L"%u", len);

	//if(*field1)
	//	free(*field1);

	//*field1 = size;
	frame->Picture.picSize = size;

	char *tmp = (char*) malloc(len);
	if(tmp == NULL)
	{
		swprintf(frame->Picture.picSize, L"0");	//len
		return 1;
	}

	memcpy(tmp, buf, len);

	//if(*field)
	//	free(*field);

	//*field = (wchar_t*) tmp;
	frame->data = (wchar_t*) tmp;
	return 1;
}

wchar_t *Tags::id3_get_uslt_unicode_str(unsigned char *buf, unsigned int *len)
{
	if(*len < 5)
		return NULL;

	unsigned int enc = buf[0];
	unsigned int out_size,out_size1;
	unsigned char *ptr = buf + 1;
	unsigned int length = 3;//*len - 1;
	wchar_t *str = id3_decode_unicode_str(&ptr, &length, 0, &out_size);
	//if(str)
		//free(str);
	wchar_t *oStr;
	ptr = buf + 4;
	length = *len - 4;
	wchar_t *str1 = id3_decode_unicode_str(&ptr, &length, enc, &out_size1);
	if(str1)
		free(str1);
	str1 = id3_decode_unicode_str(&ptr, &length, enc, &out_size1);
	//*len = out_size1+2+out_size;
	out_size = wcslen(str);
	if(str && str1)
	{
		*len = out_size + out_size1 + 2;
		oStr = (wchar_t *)malloc((*len+1)*sizeof(wchar_t));
		if(!oStr)
		{
			if(str)
				free(str);
			return str1;
		}
		unsigned int i = 0;
		for(;i<out_size;i++)
		{
			oStr[i] = str[i];
		}
		oStr[i++] = L'|';
		oStr[i++] = L'|';
		for(unsigned int j = 0;j<out_size1;j++)
		{
			oStr[i++] = str1[j];
		}
		oStr[i] = L'\0';
		if(str)
			free(str);
		if(str1)
			free(str1);
	}
	else
	{
		if(str)
			free(str);
		return str1;
	}
	return oStr;

}

int Tags::CopyID3Frame(ID3Frame *des,const ID3Frame *src)
{
	if(des->data)
		free(des->data);
	if(des->other)
		free(des->other);
	if(des->Picture.mime)
		free(des->Picture.mime);
	if(des->Picture.picDes)
		free(des->Picture.picDes);
	if(des->Picture.picSize)
		free(des->Picture.picSize);
	if(des->Picture.picType)
		free(des->Picture.picType);
	des->data = NULL;
	des->other = NULL;
	des->Picture.mime = NULL;
	des->Picture.picDes = NULL;
	des->Picture.picSize = NULL;
	des->Picture.picType = NULL;
	wcscpy_s(des->FrameID,src->FrameID);
	des->FrameSize = src->FrameSize;
	des->FFlags = src->FFlags;
	int len;
	if(src->data)
	{
		len = wcslen(src->data)+1;
		des->data = (wchar_t*)malloc(len*sizeof(wchar_t));
		wcscpy_s(des->data,len,src->data);
	}
	if(src->other)
	{
		len = wcslen(src->other)+1;
		des->other = (wchar_t*)malloc(len*sizeof(wchar_t));
		wcscpy_s(des->other,len,src->other);
	}
	if(src->Picture.mime)
	{
		len = wcslen(src->Picture.mime)+1;
		des->Picture.mime = (wchar_t*)malloc(len*sizeof(wchar_t));
		wcscpy_s(des->Picture.mime,len,src->Picture.mime);
	}
	if(src->Picture.picSize)
	{
		len = wcslen(src->Picture.picSize)+1;
		des->Picture.picSize = (wchar_t*)malloc(len*sizeof(wchar_t));
		wcscpy_s(des->Picture.picSize,len,src->Picture.picSize);
	}
	if(src->Picture.picDes)
	{
		len = wcslen(src->Picture.picDes)+1;
		des->Picture.picDes = (wchar_t*)malloc(len*sizeof(wchar_t));
		wcscpy_s(des->Picture.picDes,len,src->Picture.picDes);
	}
	if(src->Picture.picType)
	{
		len = wcslen(src->Picture.picType)+1;
		des->Picture.picType = (wchar_t*)malloc(len*sizeof(wchar_t));
		wcscpy_s(des->Picture.picType,len,src->Picture.picType);
	}
	return 0;
}

unsigned int Tags::GetFrameNum()
{
	return m_frameNum;
}

ID3Frame *Tags::FindFrame(char *frameID,unsigned int *index)
{
	USES_CONVERSION;
	return FindFrame(A2W(frameID),index);
}

ID3Frame *Tags::FindFrame(wchar_t *frameID,unsigned int *index)
{
	*index = -1;
	for(unsigned int i = 0;i < m_frameNum; i++)
	{
		if(0 == wcscmp(frameID,m_frames[i].FrameID) )
		{
			if(0 == m_frames[i].data)
				return NULL;
			*index = i;
			return GetFrame(i);
		}
	}
	return NULL;
}

ID3Frame *Tags::GetFrame(unsigned int i)
{
	assert(i >= 0 && i < m_frameNum);
	CopyID3Frame(&tmpFrame,&m_frames[i]);
	return &tmpFrame;
}

ID3Bitmap * Tags::GetHBITMAP(unsigned int i )
{
	if(i >= 0 && i < m_frameNum)
	{
		if(0 == wcscmp(m_frames[i].FrameID,L"APIC"))
		{
			if(NULL != id3Bitmap->hbm)
			{
				return id3Bitmap;
			}
		}
	}
	for(unsigned int j = 0;j < m_frameNum;j++)
	{
		if(0 == wcscmp(m_frames[j].FrameID,L"APIC"))
		{
			USES_CONVERSION;
			char *mime = W2A(m_frames[j].Picture.mime);
			unsigned int size = _wtoi(m_frames[j].Picture.picSize);
			if(mime && size)
			{
				id3Bitmap->hbm = DecodePicture(mime,(char*)m_frames[j].data,size,&id3Bitmap->width,&id3Bitmap->height);
				return id3Bitmap;
			}
			/*if( size)
			{
				ENUM_CXIMAGE_FORMATS format;
				if( 0 == wcscmp(m_frames[j].Picture.mime,L"image/jpeg")
					||0 == wcscmp(m_frames[j].Picture.mime,L"image/jpg"))
					format = CXIMAGE_FORMAT_JPG;
				else if( 0 == wcscmp(m_frames[j].Picture.mime,L"image/png"))
					format = CXIMAGE_FORMAT_PNG;
				else if(0 == wcscmp(m_frames[j].Picture.mime,L"image/bmp"))
					format = CXIMAGE_FORMAT_BMP;
				else
					format = CXIMAGE_FORMAT_UNKNOWN;
				CxImage *image = new CxImage((uint8_t*)m_frames[j].data,size,format);
				
				id3Bitmap->hbm = image->MakeBitmap();
				delete image;
				return id3Bitmap;
			}*/
		}
	}
	return NULL;
}
