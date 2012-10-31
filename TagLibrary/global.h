#pragma once
#include <windows.h>
#define HEADER_SIZE 10

#define DecodeTagSize(value1, value2, value3, value4) (((value1 & 0x7F) << 21) | ((value2 & 0x7F) << 14) | ((value3 & 0x7F) << 7) | (value4 & 0x7F))
#define DecodeFrameSize2(value1, value2, value3) ((value1 << 16) | (value2 << 8) |  value3 )
#define DecodeFrameSize3(value1, value2, value3, value4) ((value1 << 24) | (value2 << 16) | ( value3 << 8 ) | value4 )
#define DecodeFrameSize4(value1, value2, value3, value4) (((value1 & 0x7F) << 21) | ((value2 & 0x7F) << 14) | ((value3 & 0x7F) << 7) | (value4 & 0x7F))
#define GetBit(value, pos) ( (value >> pos) & 0x01)

typedef struct ID3Header
{
	//char Header[3];// must be ID3
	unsigned int MajorVersion;
	unsigned int RevisionNumber;
	unsigned int nFlags;
	//unsigned char *unsync_buf;
	struct
	{
		bool Unsync;
		bool Extended;
		bool Experimental;
		bool Footer;
	}TFlags;
	unsigned int TagSize;
	unsigned int TotalSize;
}ID3Header;


typedef struct _ID3Frame
{
	wchar_t FrameID[5];
	unsigned int FrameSize;
	struct
	{
		bool TagAlterDiscarded;//0 for preserved ,1 for discarded
		bool FileAlterDiscarded;//0 for preserved ,1 for discarded
		bool ReadOnly;
		bool Compressed;
		bool Encryption;
		bool GroupingIdentity;// 0 does not contain,0 contain
		bool DataLengthIndicator;
		bool Unsynchronisation;
	}FFlags;
	struct
	{
		wchar_t *mime;
		wchar_t *picSize;
		wchar_t *picDes;
		wchar_t *picType;
	}Picture;
	wchar_t *data;
	wchar_t *other;// only work for TXXX frame
}ID3Frame;

static void freeID3Frame(ID3Frame *f)
{
	if(f)
	{
		if(f->data)
			free(f->data);
		if(f->other)
			free(f->other);
		if(f->Picture.mime)
			free(f->Picture.mime);
		if(f->Picture.picDes)
			free(f->Picture.picDes);
		if(f->Picture.picSize)
			free(f->Picture.picSize);
		if(f->Picture.picType)
			free(f->Picture.picType);
	}
	memset(f,0,sizeof(ID3Frame));
	f = NULL;
}

typedef struct 
{
	unsigned int width;
	unsigned int height;
	HBITMAP hbm;
}ID3Bitmap;

enum FrameType
{
	AENC = 0,//   [#sec4.20 Audio encryption] 
	APIC,//    [#sec4.15 Attached picture]
	ASPI,//Audio seek point index
	COMM,//    [#sec4.11 Comments]
	COMR   ,//[#sec4.25 Commercial frame]
	ENCR   ,//[#sec4.26 Encryption method registration]
	EQUA   ,//[#sec4.13 Equalization]
	ETCO   ,//[#sec4.6 Event timing codes]
	GEOB   ,//[#sec4.16 General encapsulated object]
	GRID   ,//[#sec4.27 Group identification registration]
	IPLS   ,//[#sec4.4 Involved people list]
	LINK   ,//[#sec4.21 Linked information]
	MCDI   ,//[#sec4.5 Music CD identifier]
	MLLT   ,//[#sec4.7 MPEG location lookup table]
	OWNE   ,//[#sec4.24 Ownership frame]
	PRIV   ,//[#sec4.28 Private frame]
	PCNT   ,//[#sec4.17 Play counter]
	POPM   ,//[#sec4.18 Popularimeter]
	POSS   ,//[#sec4.22 Position synchronisation frame]
	RBUF   ,//[#sec4.19 Recommended buffer size]
	RVAD   ,//[#sec4.12 Relative volume adjustment]
	RVRB   ,//[#sec4.14 Reverb]
	SYLT   ,//[#sec4.10 Synchronized lyric/text]
	SYTC   ,//[#sec4.8 Synchronized tempo codes]
	TALB   ,//[#TALB Album/Movie/Show title]
	TBPM   ,//[#TBPM BPM (beats per minute)]
	TCOM   ,//[#TCOM Composer]
	TCON   ,//[#TCON Content type]
	TCOP   ,//[#TCOP Copyright message]
	TDAT   ,//[#TDAT Date]
	TDLY   ,//[#TDLY Playlist delay]
	TENC   ,//[#TENC Encoded by]
	TEXT   ,//[#TEXT Lyricist/Text writer]
	TFLT   ,//[#TFLT File type]
	TIME   ,//[#TIME Time]
	TIT1   ,//[#TIT1 Content group description]
	TIT2   ,//[#TIT2 Title/songname/content description]
	TIT3   ,//[#TIT3 Subtitle/Description refinement]
	TKEY   ,//[#TKEY Initial key]
	TLAN   ,//[#TLAN Language(s)]
	TLEN   ,//[#TLEN Length]
	TMED   ,//[#TMED Media type]
	TOAL   ,//[#TOAL Original album/movie/show title]
	TOFN   ,//[#TOFN Original filename]
	TOLY   ,//[#TOLY Original lyricist(s)/text writer(s)]
	TOPE   ,//[#TOPE Original artist(s)/performer(s)]
	TORY   ,//[#TORY Original release year]
	TOWN   ,//[#TOWN File owner/licensee]
	TPE1   ,//[#TPE1 Lead performer(s)/Soloist(s)]
	TPE2   ,//[#TPE2 Band/orchestra/accompaniment]
	TPE3   ,//[#TPE3 Conductor/performer refinement]
	TPE4   ,//[#TPE4 Interpreted, remixed, or otherwise modified by]
	TPOS   ,//[#TPOS Part of a set]
	TPUB   ,//[#TPUB Publisher]
	TRCK   ,//[#TRCK Track number/Position in set]
	TRDA   ,//[#TRDA Recording dates]
	TRSN   ,//[#TRSN Internet radio station name]
	TRSO   ,//[#TRSO Internet radio station owner]
	TSIZ   ,//[#TSIZ Size]
	TSRC   ,//[#TSRC ISRC (international standard recording code)]
	TSSE   ,//[#TSEE Software/Hardware and settings used for encoding]
	TYER   ,//[#TYER Year]
	TXXX   ,//[#TXXX User defined text information frame]
	UFID   ,//[#sec4.1 Unique file identifier]
	USER   ,//[#sec4.23 Terms of use]
	USLT   ,//[#sec4.9 Unsychronized lyric/text transcription]
	WCOM   ,//[#WCOM Commercial information]
	WCOP   ,//[#WCOP Copyright/Legal information]
	WOAF   ,//[#WOAF Official audio file webpage]
	WOAR   ,//[#WOAR Official artist/performer webpage]
	WOAS   ,//[#WOAS Official audio source webpage]
	WORS   ,//[#WORS Official internet radio station homepage]
	WPAY   ,//[#WPAY Payment]
	WPUB   ,//[#WPUB Publishers official webpage]
	WXXX   ,//[#WXXX User defined URL link frame]
	EQU2,// Equalisation (2)
	RVA2,
	SEEK,
	SIGN,
	TDEN,
	TDOR,
	TDRC,
	TDTG,
	TIPL,
	TMCL,
	TMOO,
	TPRO,
	TSOA,
	TSOP,
	TSOT,
	TSST,
	TDRL,
	// ID3 v2.2 frames
	BUF,// Recommended buffer size

    CNT,// Play counter
    COM ,//Comments
    CRA ,//Audio encryption
    CRM,// Encrypted meta frame
    ETC,// Event timing codes
    EQU ,//Equalization

    GEO,// General encapsulated object

    IPL,// Involved people list

    LNK,// Linked information

    MCI,// Music CD Identifier
    MLL,// MPEG location lookup table

    PIC,// Attached picture
    POP,// Popularimeter

    REV,// Reverb
    //RVA,// Relative volume adjustment

    SLT,// Synchronized lyric/text
    STC,// Synced tempo codes

    TAL,// Album/Movie/Show title
    TBP,// BPM (Beats Per Minute)
    TCM,// Composer
    TCO,// Content type
    TCR,// Copyright message
    TDA,// Date
    TDY,// Playlist delay
    TEN,// Encoded by
    TFT,// File type
    TIM,// Time
    TKE,// Initial key
    TLA,// Language(s)
    TLE,// Length
    TMT,// Media type
    TOA,// Original artist(s)/performer(s)
    TOF,// Original filename
    TOL,// Original Lyricist(s)/text writer(s)
    TOR,// Original release year
    TOT,// Original album/Movie/Show title
    TP1,// Lead artist(s)/Lead performer(s)/Soloist(s)/Performing group
    TP2,// Band/Orchestra/Accompaniment
    TP3,// Conductor/Performer refinement
    TP4,// Interpreted, remixed, or otherwise modified by
    TPA,// Part of a set
    TPB,// Publisher
    TRC,// ISRC (International Standard Recording Code)
    TRD,// Recording dates
    TRK,// Track number/Position in set
    TSI,// Size
    TSS,// Software/hardware and settings used for encoding
    TT1,// Content group description
    TT2,// Title/Songname/Content description
    TT3,// Subtitle/Description refinement
    TXT,// Lyricist/text writer
    TXX,// User defined text information frame
    TYE,// Year

    UFI,// Unique file identifier
    ULT,// Unsychronized lyric/text transcription

    WAF,// Official audio file webpage
    WAR,// Official artist/performer webpage
    WAS,// Official audio source webpage
    WCM,// Commercial information
    WCP,// Copyright/Legal information
    WPB,// Publishers official webpage
    WXX,// User defined URL link frame
	UNKNOWN
};

static FrameType getFrameType(wchar_t *str)
{
	if( 0 == wcscmp(L"TDRL",str))
		return TDRL;
	if( 0 == wcscmp(L"TYER",str))
		return TYER;
	if( 0 == wcscmp(L"AENC",str)) //   [#sec4.20 Audio encryption] 
		return AENC;
	if(0 == wcscmp(L"APIC",str))//    [#sec4.15 Attached picture]
		return APIC;
	if( 0 == wcscmp(L"ASPI",str)) //   [#sec4.20 Audio encryption] 
		return ASPI;

	if(0 == wcscmp(L"COMM",str))//    [#sec4.15 Attached picture]
		return COMM ;
	if(0 == wcscmp(L"COMR",str))//    [#sec4.15 Attached picture]
		return COMR ;

	if(0 == wcscmp(L"ENCR",str))//    [#sec4.15 Attached picture]
		return ENCR ;
	if(0 == wcscmp(L"EQU2",str))//    [#sec4.15 Attached picture]
		return EQU2 ;

	if( 0 == wcscmp(L"ETCO",str)) //   [#sec4.20 Audio encryption] 
		return ETCO;
	if(0 == wcscmp(L"GEOB",str))//    [#sec4.15 Attached picture]
		return GEOB;
	if( 0 == wcscmp(L"GRID",str)) //   [#sec4.20 Audio encryption] 
		return GRID;

	if( 0 == wcscmp(L"LINK",str)) //   [#sec4.20 Audio encryption] 
		return LINK;
	if(0 == wcscmp(L"MCDI",str))//    [#sec4.15 Attached picture]
		return MCDI;
	if( 0 == wcscmp(L"MLLT",str)) //   [#sec4.20 Audio encryption] 
		return MLLT;

	if( 0 == wcscmp(L"OWNE",str)) //   [#sec4.20 Audio encryption] 
		return OWNE;
	if(0 == wcscmp(L"PRIV",str))//    [#sec4.15 Attached picture]
		return PRIV;
	if( 0 == wcscmp(L"PCNT",str)) //   [#sec4.20 Audio encryption] 
		return PCNT;

	if( 0 == wcscmp(L"POPM",str)) 
		return POPM;
	if(0 == wcscmp(L"POSS",str))
		return POSS;
	if( 0 == wcscmp(L"RBUF",str))
		return RBUF;

	if( 0 == wcscmp(L"RVA2",str)) 
		return RVA2;
	if(0 == wcscmp(L"RVRB",str))
		return RVRB;
	if( 0 == wcscmp(L"SEEK",str))
		return SEEK;

	if( 0 == wcscmp(L"SIGN",str)) 
		return SIGN;
	if(0 == wcscmp(L"SYLT",str))
		return SYLT;
	if( 0 == wcscmp(L"SYTC",str))
		return SYTC;

	if( 0 == wcscmp(L"TALB",str)) 
		return TALB;
	if(0 == wcscmp(L"TBPM",str))
		return TBPM;
	if( 0 == wcscmp(L"TCOM",str))
		return TCOM;

	if( 0 == wcscmp(L"TCON",str)) 
		return TCON;
	if(0 == wcscmp(L"TCOP",str))
		return TCOP;
	if( 0 == wcscmp(L"TDEN",str))
		return TDEN;


	if( 0 == wcscmp(L"TDLY",str)) 
		return TDLY;
	if(0 == wcscmp(L"TDOR",str))
		return TDOR;
	if( 0 == wcscmp(L"TDRC",str))
		return TDRC;

	if( 0 == wcscmp(L"TDRL",str)) 
		return TDLY;
	if(0 == wcscmp(L"TDTG",str))
		return TDTG;
	if( 0 == wcscmp(L"TENC",str))
		return TENC;

	if( 0 == wcscmp(L"TEXT",str)) 
		return TEXT;
	if(0 == wcscmp(L"TFLT",str))
		return TFLT;
	if( 0 == wcscmp(L"TIPL",str))
		return TIPL;

	if( 0 == wcscmp(L"TIT1",str)) 
		return TIT1;
	if(0 == wcscmp(L"TIT2",str))
		return TIT2;
	if( 0 == wcscmp(L"TIT3",str))
		return TIT3;

	if( 0 == wcscmp(L"TKEY",str)) 
		return TKEY;
	if(0 == wcscmp(L"TLAN",str))
		return TLAN;
	if( 0 == wcscmp(L"TLEN",str))
		return TLEN;

	if( 0 == wcscmp(L"TMCL",str)) 
		return TMCL;
	if(0 == wcscmp(L"TMED",str))
		return TMED;
	if( 0 == wcscmp(L"TMOO",str))
		return TMOO;

	if( 0 == wcscmp(L"TOAL",str)) 
		return TOAL;
	if(0 == wcscmp(L"TOFN",str))
		return TOFN;
	if( 0 == wcscmp(L"TOLY",str))
		return TOLY;

	if( 0 == wcscmp(L"TOPE",str)) 
		return TOPE;
	if(0 == wcscmp(L"TOWN",str))
		return TOWN;
	if( 0 == wcscmp(L"TPE1",str))
		return TPE1;

	if( 0 == wcscmp(L"TPE2",str)) 
		return TPE2;
	if(0 == wcscmp(L"TPE3",str))
		return TPE3;
	if( 0 == wcscmp(L"TPE4",str))
		return TPE4;

	if( 0 == wcscmp(L"TPOS",str)) 
		return TPOS;
	if(0 == wcscmp(L"TPRO",str))
		return TPRO;
	if( 0 == wcscmp(L"TPUB",str))
		return TPUB;

	if( 0 == wcscmp(L"TRCK",str)) 
		return TRCK;
	if(0 == wcscmp(L"TRSN",str))
		return TRSN;
	if( 0 == wcscmp(L"TRSO",str))
		return TRSO;

	if( 0 == wcscmp(L"TSOA",str)) 
		return TSOA;
	if(0 == wcscmp(L"TSOP",str))
		return TSOP;
	if( 0 == wcscmp(L"TSOT",str))
		return TSOT;

	if( 0 == wcscmp(L"TSRC",str)) 
		return TSRC;
	if(0 == wcscmp(L"TSSE",str))
		return TSSE;
	if( 0 == wcscmp(L"TSST",str))
		return TSST;

	if( 0 == wcscmp(L"TXXX",str)) 
		return TXXX;
	if(0 == wcscmp(L"UFID",str))
		return UFID;
	if( 0 == wcscmp(L"USER",str))
		return USER;

	if( 0 == wcscmp(L"USLT",str)) 
		return USLT;
	if(0 == wcscmp(L"WCOM",str))
		return WCOM;
	if( 0 == wcscmp(L"WCOP",str))
		return WCOP;

	if( 0 == wcscmp(L"WOAF",str)) 
		return WOAF;
	if(0 == wcscmp(L"WOAR",str))
		return WOAR;
	if( 0 == wcscmp(L"WOAS",str))
		return WOAS;

	if( 0 == wcscmp(L"WORS",str)) 
		return WORS;
	if(0 == wcscmp(L"WPAY",str))
		return WPAY;
	if( 0 == wcscmp(L"WPUB",str))
		return WPUB;

	if( 0 == wcscmp(L"WXXX",str))
		return WPUB;
	// id3v2.2
	if( 0 == wcscmp(L"BUF",str)) 
		return BUF ;
	if(0 == wcscmp(L"CNT",str))
		return CNT;
	if( 0 == wcscmp(L"COM",str))
		return COM;

	if( 0 == wcscmp(L"CRA",str)) 
		return CRA;
	if(0 == wcscmp(L"CRM",str))
		return CRM;
	if( 0 == wcscmp(L"ETC",str))
		return ETC;

	if( 0 == wcscmp(L"EQU",str))
		return EQU;


	if( 0 == wcscmp(L"GEO",str)) 
		return GEO;
	if(0 == wcscmp(L"IPL",str))
		return IPL;
	if( 0 == wcscmp(L"LNK",str))
		return LNK;

	if( 0 == wcscmp(L"MCI",str))
		return EQU;


	if( 0 == wcscmp(L"MLL",str)) 
		return MLL;
	if(0 == wcscmp(L"PIC",str))
		return PIC;
	if( 0 == wcscmp(L"POP",str))
		return POP;

	if( 0 == wcscmp(L"REV",str))
		return REV;

	/*if( 0 == wcscmp(L"RVA",str)) 
		return RVA;*/
	if(0 == wcscmp(L"SLT",str))
		return SLT;
	if( 0 == wcscmp(L"STC",str))
		return STC;

	if( 0 == wcscmp(L"TAL",str))
		return TAL;

	if( 0 == wcscmp(L"TBP",str)) 
		return TBP;
	if(0 == wcscmp(L"TCM",str))
		return TCM;
	if( 0 == wcscmp(L"TCO",str))
		return TCO;

	if( 0 == wcscmp(L"TCR",str))
		return TCR;

	if( 0 == wcscmp(L"TDA",str)) 
		return TDA;
	if(0 == wcscmp(L"TDY",str))
		return TDY;
	if( 0 == wcscmp(L"TEN",str))
		return TEN;

	if( 0 == wcscmp(L"TFT",str))
		return TFT;

	if( 0 == wcscmp(L"TIM",str)) 
		return TIM;
	if(0 == wcscmp(L"TKE",str))
		return TKE;
	if( 0 == wcscmp(L"TLA",str))
		return TLA;

	if( 0 == wcscmp(L"TLE",str))
		return TLE;

	if( 0 == wcscmp(L"TMT",str)) 
		return TMT;
	if(0 == wcscmp(L"TOA",str))
		return TOA;
	if( 0 == wcscmp(L"TOF",str))
		return TOF;

	if( 0 == wcscmp(L"TOL",str))
		return TOL;

	if( 0 == wcscmp(L"TOR",str)) 
		return TOR;
	if(0 == wcscmp(L"TOT",str))
		return TOT;
	if( 0 == wcscmp(L"TP1",str))
		return TP1;

	if( 0 == wcscmp(L"TP2",str))
		return TP2;

	if( 0 == wcscmp(L"TP3",str)) 
		return TP3;
	if(0 == wcscmp(L"TP4",str))
		return TP4;
	if( 0 == wcscmp(L"TPA",str))
		return TPA;

	if( 0 == wcscmp(L"TPB",str))
		return TPB;

	if( 0 == wcscmp(L"TRC",str)) 
		return TRC;
	if(0 == wcscmp(L"TRD",str))
		return TRD;
	if( 0 == wcscmp(L"TRK",str))
		return TRK;

	if( 0 == wcscmp(L"TSI",str))
		return TSI;

	if( 0 == wcscmp(L"TSS",str)) 
		return TSS;
	if(0 == wcscmp(L"TT1",str))
		return TT1;
	if( 0 == wcscmp(L"TT2",str))
		return TT2;

	if( 0 == wcscmp(L"TT3",str))
		return TT3;

	if( 0 == wcscmp(L"TXT",str)) 
		return TXT;
	if(0 == wcscmp(L"TXX",str))
		return TXX;
	if( 0 == wcscmp(L"TYE",str))
		return TYE;
	if( 0 == wcscmp(L"UFI",str))
		return UFI;

	if( 0 == wcscmp(L"ULT",str)) 
		return ULT;
	if(0 == wcscmp(L"WAF",str))
		return WAF;
	if( 0 == wcscmp(L"WAR",str))
		return WAR;
	if( 0 == wcscmp(L"WAS",str))
		return WAS;

	if( 0 == wcscmp(L"WCM",str)) 
		return WCM;
	if(0 == wcscmp(L"WCP",str))
		return WCP;
	if( 0 == wcscmp(L"WPB",str))
		return WPB;
	if( 0 == wcscmp(L"WXX",str))
		return WXX;
	return UNKNOWN;
}