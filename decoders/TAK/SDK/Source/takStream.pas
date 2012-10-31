unit takStream;

{
=====================================================================

This source module is part of TAK, Tom's lossless audio kompressor.

Homepage: http://www.thbeck.de/Tak/Tak.html

Version: 1.1.1

Copyright 2006 by Thomas Becker, D-49080 Osnabrueck, Germany.
All rights reserved.


Content:
--------

Hopefully anything needed to understand, how TAK's container works.


Purpose:
--------

I understand, that there is a demand for a documentation of TAK's container.
But unfortunately i am currently too short of time to write a comprehensive
documentation. Therefore the best thing i can do is to publish this source
module which contains all relevant code and definitions.

I have also added some comments, unfortunately in my rather bad english...


License:
--------

Do with it, whatever you want.

But please:

- Don't distribute this module without the SDK containing it.

- If for some reason you want to publish a modified version, please
  state clearly, that it isn't an official release.

=====================================================================
}

{ Relevant classes not contained in this module:


  === TBitOutStream =================================================

  TBitOutStream = class

    property DataStart : PInt32_Arr read FDataStart;

      Pointer to the start of the data buffer.

    property BytesWritten : TtakInt32 read GetBytesWritten;

      Count of bytes written to the data buffer: (BitsWritten + 7) div 8

    procedure AlignEndToByte;

      Write zero bits until the length in bits is an integer multiple of 8.

    procedure PutLong (ABits   : TtakInt32;
                       ABitNum : TtakInt32);

      Write ABitNum lower bits from ABits. Caution: The upper bits have to be
      0 because they are not beeing masked out.

    procedure PutBool (ABool : Boolean);

      Write a 0/1 bit if ABool is false/true.

  end;


  === TBitInStream ==================================================

  TBitInStream = class

    property Overflow : Boolean read FOverflow;

      Not enough bits available.

      If a read function requests more bits than are available, it will
      simply return bits with value 0. You have to check Overflow at some
      point, or use a function with a '_Check' Suffix, which will
      return not (~) Overflow.


    property DataStart : Pointer read GetDataStart;

      Pointer to the start of the data buffer.

    property BytesRead : TtakInt32 read GetBytesRead;

      Count of bytes read: (BitsRead + 7) div 8

    function AlignEndToByte : TtakInt32;

      Read bits until the total read length in bits is an integer
      multiple of 8.

    function GetLong (ABitNum : TtakInt32) : TtakInt32;
    function GetLong_Check (    ABitNum : TtakInt32;
                            var ALong   : TtakInt32)    : Boolean;

      Read ABitNum bits.

    function GetBit : TtakInt32;

      Read a bit.
  end;


  === Audio-Format ==================================================

  const
    tak_AudioFormat_DataType_PCM = 0;

  type
    TtakAudioFormat = packed record
      DataType : TtakInt32;
      SampleRate : TtakInt32;
      SampleBits : TtakInt32;
      ChannelNum : TtakInt32;
      BlockSize  : TtakInt32;
        = ChannelNum * ((SampleBits + 7) / 8)
    end;


  === Some remarks ==================================================

  - Anything is little endian.

  - Descriptions of the items of bit structures always start with the
    least significant bits.

  - Padding is always beeing done with zero bits.
}

interface

uses
  SysUtils,
  takCommon, takCrc, takMD5, takBits;



{=== Stream =========================================================}

{
  General structure:

    StreamId
    Meta data
    Audio data
    Tags
      Currentyl only APEv2 is supported by the decoder.

  Although StreamId and Meta data are obligatory for TAK files, the decoder
  can decode the audio data without them.
}

  {--- StreamId ---}

{
  The first item in the stream is the StreamId.
}

const
  tak_str_StreamIdLen  = 4;
  tak_str_StreamIdSize = tak_str_StreamIdLen;

type
  Ttak_str_StreamId = Array[0..tak_str_StreamIdLen - 1] of Char;

const
  tak_str_StreamId : Ttak_str_StreamId = 'tBaK';


  {--- Crc ---}

{
  Any CRC used in the stream is CRC 24 RFC2440
}

const
  tak_str_CrcBits = 24;
  tak_str_CrcSize = tak_str_CrcBits div 8;

type
  Ttak_str_Crc = Cardinal;
    { I still have to define a TtakUInt32 (long)...
    }



{=== Frame size =====================================================}

type
  TtakFrameSizeTypes = TtakInt32;

const
  tak_FrameSizeType_94_ms  =  0;
  tak_FrameSizeType_125_ms =  1;
  tak_FrameSizeType_188_ms =  2;
  tak_FrameSizeType_250_ms =  3;
  tak_FrameSizeType_4096   =  4;
  tak_FrameSizeType_8192   =  5;
  tak_FrameSizeType_16384  =  6;
  tak_FrameSizeType_512   =   7;
  tak_FrameSizeType_1024   =  8;
  tak_FrameSizeType_2048   =  9;

{
  These types have been removed in TAK 1.1.0:

    tak_FrameSizeType_6144   = 10;
    tak_FrameSizeType_12288  = 11;

  Not a big  problem, because earlier encoders never used them.
}

  tak_FrameSizeType_First      = tak_FrameSizeType_94_ms;
  tak_FrameSizeType_Last       = tak_FrameSizeType_2048;
  tak_FrameSizeType_Num        = tak_FrameSizeType_Last - tak_FrameSizeType_First + 1;
  tak_FrameSizeType_Time_First = tak_FrameSizeType_94_ms;
  tak_FrameSizeType_Time_Last  = tak_FrameSizeType_250_ms;
  tak_FrameSizeType_Time_Max   = tak_FrameSizeType_250_ms;
  tak_FrameSizeType_Size_First = tak_FrameSizeType_4096;
  tak_FrameSizeType_Size_Last  = tak_FrameSizeType_2048;
  tak_FrameSizeType_Size_Max   = tak_FrameSizeType_16384;
  tak_FrameSizeType_FrameSize_Min = 512;
    { Frame sizes are beeing expressed as samples per frame (size type) or
      duration in milliseconds (time type).
    }

const
  tak_str_FrameDurationQuantResolution = 32;
  tak_str_FrameDurationQuantShift      = 5;
    { Determine the sample count (per channel!) for a given frame duration
      time type with this formula:

        SampleCount = (SampleRate * QuantValue) / tak_str_FrameDurationQuantResolution

      QuantValues are defined in FrameDurationTypeQuants[].

      For more details see tak_str_CalcFrameSizeFromType().
    }

const
  tak_str_InfoFrameDistanceInDurationQuants = 2 * tak_str_FrameDurationQuantResolution;
    { An info frame every 2 seconds is obligatory.
    }


function tak_str_CalcFrameSizeFromSizeType (    AType : TtakFrameSizeTypes;
                                            var ASize : TtakInt32) : Boolean;
  { Writes the frame size in samples for the size type AType in ASize.

    If AType isn't a valid size type, ASize will be undefined and the function
    returns False.
  }

function tak_str_CalcFrameSizeFromType (    ASampleRate : TtakInt32;
                                        var AType       : TtakFrameSizeTypes;
                                        var ASize       : TtakInt32) : Boolean;

  { Calculates the frame size in samples for ASampleRate and AType and writes
    the result in ASize.

    Returns True, if the result is valid. Conditions:

      - Frame size     <= 16384,  if AType is a time type.
      - Frame duration <= 250 ms, if AType is a size type.

    Otherwises AType is beeing adapted as follows and ASize contains the
    corresponding size:

      - tak_FrameSizeType_Size_Max, if AType is a time type.
      - tak_FrameSizeType_Time_Max, if AType is a size type.
  }

function tak_str_CalcFrameTimeTypeFromSize (    ASampleRate : TtakInt32;
                                                ASize       : TtakInt32;
                                            var AType       : TtakFrameSizeTypes) : Boolean;

  { Writes the smallest (shortest) time type for ASampleRate in AType, which
    corresponds to a frame size of at least ASize samples.

    If ASize is too large for even the biggest time type, the functions returns
    false and AType contains tak_FrameSizeType_Time_Last.
  }

function tak_str_CalcFrameDurationFromSize (ASampleRate : TtakInt32;
                                            AFrameSize  : TtakInt32) : TtakInt32;

  { Returns the rounded frame duration in ms for a frame size AFrameSize
    and a sample rate ASampleRate.
  }

function tak_str_FramesPerQuantTime (ASampleRate   : TtakInt32;
                                     AFrameSize    : TtakInt32;
                                     ATimeInQuants : TtakInt32) : TtakInt32;

  { Returns the number of frames for ASampleRate and AFrameSize, which
    fall into the time interval ATimeInQuants (see adc_FrameDurationQuantResolution).
    Rounds always down.

    Current Limitations:

      - Exception, if ATimeInQuants = 0
      - Overflows possible, if ASampleRate and/or ATimeInQuants are too big.
  }


function tak_str_CalcFrameNumFromSampleNum (ASampleNum : TtakInt32;
                                            AFrameSize : TtakInt32) : TtakInt32;
  { How many frames are required to store ASampleNum samples, if the
    frame size is AFrameSize?
  }

function tak_str_CalcLastFrameSize (ASampleNum : TtakInt32;
                                    AFrameNum  : TtakInt32;
                                    AFrameSize : TtakInt32) : TtakInt32;
  { What is the size of the last frame in a stream of ASampleNum Samples
    and with AFrameNum frames of AFrameSize samples?
  }

function tak_str_CalcInfoFrameNum (AFrameNum : TtakInt32;
                                   ADistance : TtakInt32) : TtakInt32;
  { How many info frames contains a stream of AFrameNum frames, if the
    info frame distance (in frames) is ADistance?
  }



{=== Audio frames ===================================================}

{
  Structure of an audio frame:

    Header
    Data
    Footer (Data-Crc)

  Any frame starts with the FrameInfo-Header. If it's InfoFrame flag is set,
  the header contains additional information, for example about the audio
  format. The whole structure (including the SyncId) is protected by a Crc.

    FrameInfo
    Info (if it is an info frame)
    Crc

  The footer contains only the Data-Crc, which is beeing calculated from
  the encoded (!!!) audio data. This has several advantages:

   - You can check the data integrity without decoding the audio data.

   - It's very useful for fast seeking without seek table: Seek a pair
     of SyncId's and you have 3 (Header1 - Data - Header2) 24-bit checksums
     for (a first) verification.

   - The encoded data is smaller and less data means better error
     detection by a Crc.

   - For me data protection is part of the container. It should work
     even if the data is a black box for the container. But that's a
     matter of taste and not always appropriate.
}



{=== Frame-Info =====================================================}

{
  Obligatory header for any frame:

  16 Sync code
       const

   1 Is LastFrame
       0 = no
       1 = yes

   1 Is InfoFrame
        0 = no
        1 = yes

   1 Contains metadata
        0 = no
        1 = yes (not supported)

  21 Frame number (First is 0)

     Maximum file duration depending on the frame duration:

       85 ms =  49.7 hours  (Sample count = 16384 for 192 KHz)   192 GB
       94 ms =  54.6 hours
      250 ms = 145.6 hours

  14 Sample count - 1
       0 - 16383
       Only if LastFrame = True

   2 reserved
       Padding.
       const = 0
       only if LastFrame = True

------------
  40 or 56 bits
}

const
  tak_str_FrameInfoSyncIdBits      = 16;
  tak_str_FrameInfoSyncIdSize      = 2;
  tak_str_FrameInfoSyncId          = $A0FF;  // 10100000.11111111    41215
//  tak_str_FrameInfoSyncId          = $0000;  // For probability determination.

  tak_str_FrameInfoFrameNoBits     = 21;

  tak_str_FrameInfoBits            = 40;
  tak_str_FrameInfoSize            = (tak_str_FrameInfoBits + 7) div 8;

  tak_str_FrameInfoSampleCountBits = 14;
  tak_str_FrameInfoLastBits        = tak_str_FrameInfoBits + 16;
  tak_str_FrameInfoLastSize        = (tak_str_FrameInfoLastBits + 7) div 8;
  tak_str_FrameInfoSizeMax         = tak_str_FrameInfoLastSize;

  tak_str_FrameNumMax              = 1 shl tak_str_FrameInfoFrameNoBits;

type
  Ttak_str_FrameInfo = record
    LastFrame   : Boolean;
    InfoFrame   : Boolean;  
    MetaData    : Boolean;
    FrameNo     : TtakInt32;
    SampleCount : TtakInt32;
  end;



{=== Encoder-Info ===================================================}

{ Info about codec and profile:

   6 Codec
       0-63

   4 Profile
       0-15

------------
  10 bits
}

const
  tak_str_EncoderInfoCodecBits   =  6;
  tak_str_EncoderInfoProfileBits =  4;
  tak_str_EncoderInfoBits        = 10;

type
  Ttak_str_EncoderInfo = packed record
    { Info about codec and profile.
    }
    Codec : TtakInt32;
      { Codec-Type. Range: 0-63.

        Always 0 for TAK 1.x.x.
      }
    Profile : TtakInt32;
      { Profile. Codec-specific. Range: 0-15.

        Profiles define maximum processing (CPU and RAM) requirements for
        the decoder.

        Until now they correspond with the presets, but this may change.
      }
  end;



{=== Size-Info ======================================================}

{ Info about stream and frame sizes:

   4 Frame duration
         0 =     94 ms
         1 =    125 ms
         2 =    188 ms
         3 =    250 ms
         4 =  4096 Samples
         5 =  8192 Samples
         6 = 16384 Samples

         7 =   512 Samples
         8 =  1024 Samples
         9 =  2048 Samples

     10-14 = reserved
        15 = read size following sample count (not implemented)


  35 Sample count of the whole stream
        0, if the value has to be taken from the meta data stream info.


------------
  39 bits
}

const
  tak_str_SizeInfoFrameDurationBits = 4;
  tak_str_SizeInfoSampleNumBits     = 35;
  tak_str_SizeInfoBits              = 39;

type
  Ttak_str_SizeInfo = packed record
    { Info about stream and frame sizes.
    }
    FrameSize : TtakFrameSizeTypes;
      { Frame size type.
      }
    FrameSizeInSamples : TtakInt32;
      { Frame size in samples per channel.
      }
    SampleNum : TInt64;
      { Stream size in samples.
      }
  end;



{=== Audio-Format ===================================================}

{ Audio format:

   3 Data type
       0   = PCM
       1-7 = reserved

  18 Sample rate
       Sample rate - 6000

   5 Bits per sample
       0-16 = 8-24

   5 Channel count
       0-31 = 1-32

------------
  31 bits

  Will be extended for more than 2 channels.
}

const
  tak_str_SampleRateMin =   6000;
  tak_str_SampleRateMax = 262144;
  tak_str_SampleBitsMin =  8;
  tak_str_SampleBitsMax = 24;
  tak_str_ChannelNumMin = 1;
  tak_str_ChannelNumMax = 2;

const
  tak_str_AudioFormatDataTypeBits   = 3;
  tak_str_AudioFormatDataType_PCM   = 0;
  tak_str_AudioFormatSampleRateBits = 18;
  tak_str_AudioFormatSampleBitsBits = 5;
  tak_str_AudioFormatChannelNumBits = 5;
  tak_str_AudioFormatBits           = 31;


function tak_str_AudioFormatSupported (const AFormat : TtakAudioFormat) : Boolean;

  { Returns True, if AFormat is supported by the container.

    This doesn't mean that it's also supported by the codec!
  }



{=== Stream-Info ====================================================}

const
  tak_str_StreamInfoBits =   tak_str_EncoderInfoBits
                           + tak_str_SizeInfoBits
                           + tak_str_AudioFormatBits;
  tak_str_StreamInfoSize = (tak_str_StreamInfoBits + 7) div 8;

type
  Ttak_str_StreamInfo = packed record
    Encoder : Ttak_str_EncoderInfo;
    Sizes   : Ttak_str_SizeInfo;
    Audio   : TtakAudioFormat;
  end;
  Ttak_str_StreamInfoCode = Array[0..tak_str_StreamInfoSize - 1] of Byte;


function tak_str_EncodeStreamInfo (const AFormat : Ttak_str_StreamInfo;
                                         ABits   : TBitOutStream) : Boolean;

  { Encodes AFormat and writes it in ABits. Returns False, if AFormat
    is invalid.
  }

function tak_str_DecodeStreamInfo (     ABits   : TBitInStream;
                                   var  AFormat : Ttak_str_StreamInfo) : Boolean;

  { Decodes data from ABits and writes the result in AFormat. Returns False,
    if the data is invalid.
  }


function tak_str_StreamInfoEqual (const AA : Ttak_str_StreamInfo;
                                  const AB : Ttak_str_StreamInfo) : Boolean;

  { Returns true, if AA and AB contain equal values.
  }



{=== Frame-Header ===================================================}

{ An InfoFrame contains some additional data following the FrameInfo
  and the StreamInfo:

   1 HasPrevDist flag
       0 = no
       1 = yes

   5 Reserved
       const = 0

  25 PrevDist
       in Bytes. Only if HasPrevDist.

  xx Whole structure (incl. FrameInfo and StreamInfo) always padded to
     have a multiple of 8 bits.

------------

  If the HasPrevDist flag is set, PrevDist contains the offset (in bytes) from
  the end of the header to the start of the previous InfoFrame.

  This may be utilized for seeking without seek table.

  Currently this feature is not beeing used and i am unsure, if it is really
  useful.
}

const
  tak_str_InfoFrameFlagNum       = 6;
  tak_str_InfoFrameFlagUsedNum   = 1;
  tak_str_InfoFrameFlagUnusedNum = tak_str_InfoFrameFlagNum - tak_str_InfoFrameFlagUsedNum;
  tak_str_InfoFrameDistanceBits  = 25;

  tak_str_InfoFrameExtBitsMin   = 8;
  tak_str_InfoFrameExtSizeMin   = tak_str_InfoFrameExtBitsMin div 8;
  tak_str_InfoFrameExtBitsMax   = 32;
  tak_str_InfoFrameExtSizeMax   = tak_str_InfoFrameExtBitsMax div 8;

const
  tak_str_FrameHeaderSizeMin =   tak_str_FrameInfoSize
                               + tak_str_CrcSize;
  tak_str_FrameHeaderSizeStd =   tak_str_FrameInfoSize
                               + tak_str_StreamInfoSize
                               + tak_str_InfoFrameExtSizeMin
                               + tak_str_CrcSize;
  tak_str_FrameHeaderSizeMax =   tak_str_FrameInfoSizeMax
                               + tak_str_StreamInfoSize
                               + tak_str_InfoFrameExtSizeMax
                               + tak_str_CrcSize;

type
  Ttak_str_FrameHeader = record
    Info        : Ttak_str_FrameInfo;
    Format      : Ttak_str_StreamInfo;
    HasPrevDist : Boolean;
    PrevDist    : TtakInt32;
  end;
  Ttak_str_FrameHeaderCode = Array[0..tak_str_FrameHeaderSizeMax - 1] of Byte;


function tak_str_EncodeFrameHeader (const AHeader : Ttak_str_FrameHeader;
                                          ABits   : TBitOutStream) : Boolean;

  { Encodes AHeader and writes it in ABits. Returns False, if AHeader
    is invalid.
  }

function tak_str_DecodeFrameHeader (    ABits   : TBitInStream;
                                    var AHeader : Ttak_str_FrameHeader) : Boolean;

  { Decodes data from ABits and writes the result in AHeader. Returns False,
    if the data is invalid.
  }



{=== MetaData =======================================================}

{
  The MetaData section contains a list of one or more meta data objects.
  Currently the following are defined:

    tak_str_MetaDataId_End              End of MetaData
    tak_str_MetaDataId_StreamInfo       StreamInfo
    tak_str_MetaDataId_SeekTable        SeekTable
    tak_str_MetaDataId_SimpleWaveData   WaveData header and -footer
    tak_str_MetaDataId_EncoderInfo      Info about the encoder
    tak_str_MetaDataId_UnusedSpace      Padding

  Although the decoder should be capable to read these objects in an arbitrary
  order, currently StreamInfo has to be the first object. Otherwises the decoder
  will signal a damage of the meta data. I will fix this in one of the next
  TAK releases.

  Each object has this structure:

    MetaDataHeader
    Data
    Crc of the Data (excluding the header)

  The MetaDataHeader specifies the object type and the size of the Data
  (excluding header and Crc).
}



{=== Meta: IDs ======================================================}

const
  tak_str_MetaDataId_End            = 0;
  tak_str_MetaDataId_StreamInfo     = 1;
  tak_str_MetaDataId_SeekTable      = 2; // Removed in TAK 1.1.1
  tak_str_MetaDataId_SimpleWaveData = 3;
  tak_str_MetaDataId_EncoderInfo    = 4;
  tak_str_MetaDataId_UnusedSpace    = 5; // New in TAK 1.0.3
  tak_str_MetaDataId_Md5            = 6; // New in TAK 1.1.1
  tak_str_MetaDataId_LastFrameInfo  = 7; // New in TAK 1.1.1
  tak_str_MetaDataId_UsedMax        = tak_str_MetaDataId_LastFrameInfo;



{=== Meta: Data header ==============================================}

{ Structure:

   7 Object type
       0-127

   1 Reserved
       const = 0

  24 Data size

------------
  32 bits
}

const
  tak_str_MedaDataHeaderIdBits = 7;
  tak_str_MedaDataHeaderIdMax  = (1 shl tak_str_MedaDataHeaderIdBits) - 1;
  tak_str_MedaDataHeaderIdMask = tak_str_MedaDataHeaderIdMax;
  tak_str_MedaDataHeaderDataSizePos  = 8;
  tak_str_MedaDataHeaderDataSizeBits = 24;
  tak_str_MedaDataHeaderDataSizeMax  = (1 shl tak_str_MedaDataHeaderDataSizeBits) - 1;
  tak_str_MedaDataHeaderSize = 4;

type
  Ttak_str_MetaDataHeader = record
    Id   : TtakInt32;
    Size : TtakInt32;
  end;
  Ttak_str_MetaDataHeaderData = TInt64;


function tak_str_EncodeMetaDataHeader (const AHeader : Ttak_str_MetaDataHeader;
                                       var   AData   : Ttak_str_MetaDataHeaderData) : Boolean;

function tak_str_DecodeMetaDataHeader (const AData   : Ttak_str_MetaDataHeaderData;
                                       var   AHeader : Ttak_str_MetaDataHeader) : Boolean;



{=== Meta: StreamInfo ===============================================}

const
  tak_str_MetaStreamInfoSize =  tak_str_StreamInfoSize
                              + tak_str_CrcSize;

type
  Ttak_str_MetaStreamInfoCode = Array[0..tak_str_MetaStreamInfoSize - 1] of Byte;


function tak_str_EncodeMetaStreamInfo (const AInfo : Ttak_str_StreamInfo;
                                             ABits : TBitOutStream) : Boolean;

function tak_str_DecodeMetaStreamInfo (    ABits : TBitInStream;
                                       var AInfo : Ttak_str_StreamInfo) : Boolean;



{=== Meta: Seektable ================================================}


{
  The seektable consists of a header and a list of seek points.

  Removed in TAK 1.1.1.
}



{=== Meta: SimpleWaveData ===========================================}


{
  The SimpleWaveData consists of a header and the data of the wave file header
  and footer.
}


  {--- Header -----------------------------------}

{
  24 HeadSize
  24 TailSize

------------
  48 bits
}

const
  tak_str_SimpleWaveDataSizeMax    = 1024 * 1024;
  tak_str_SimpleWaveDataSizeBits   = 24;
  tak_str_SimpleWaveDataSizeSize   = 3;
  tak_str_SimpleWaveDataSizeMask   = (     TtakInt64 (1)
                                       shl tak_str_SimpleWaveDataSizeBits) - 1;
  tak_str_SimpleWaveDataHeaderBits = 2 * tak_str_SimpleWaveDataSizeBits;
  tak_str_SimpleWaveDataHeaderSize = (tak_str_SimpleWaveDataHeaderBits + 7) div 8;
  tak_str_SimpleWaveDataSize       =   tak_str_SimpleWaveDataHeaderSize
                                     + tak_str_CrcSize;

type
  Ttak_str_SimpleWaveDataHeader = packed record
    FHeadSize : TtakInt32;
    FTailSize : TtakInt32;
  end;
  Ttak_str_SimpleWaveDataData = Array[0..tak_str_SimpleWaveDataSize - 1] of Byte;


function tak_str_EncodeSimpleWaveData (const AInfo : Ttak_str_SimpleWaveDataHeader;
                                       var   AData : Ttak_str_SimpleWaveDataData) : Boolean;

function tak_str_DecodeSimpleWaveData (const AData : Ttak_str_SimpleWaveDataData;
                                       var   AInfo : Ttak_str_SimpleWaveDataHeader) : Boolean;

function tak_str_SimpleWaveDataEqual (const AA : Ttak_str_SimpleWaveDataHeader;
                                      const AB : Ttak_str_SimpleWaveDataHeader) : Boolean;



{=== Meta: EncoderInfo ==============================================}

{
  24 Encoder version (3 digits, each 8 bits)
   4 Preset
   2 Evaluation
   2 Reserved
------------
  32 bits
}

const
  tak_str_MetaEncoderInfoVersionBits    = 24;
  tak_str_MetaEncoderInfoPresetBits     = tak_str_EncoderInfoProfileBits; // 4
  tak_str_MetaEncoderInfoEvaluationBits = 2;
  tak_str_MetaEncoderInfoDataBits       =   tak_str_MetaEncoderInfoVersionBits
                                          + tak_str_MetaEncoderInfoPresetBits
                                          + tak_str_MetaEncoderInfoEvaluationBits;
  tak_str_MetaEncoderInfoDataSize       = (tak_str_MetaEncoderInfoDataBits + 7) div 8;
  tak_str_MetaEncoderInfoSize           =   tak_str_MetaEncoderInfoDataSize
                                          + tak_str_CrcSize;

type
  Ttak_str_MetaEncoderInfo = packed record
    Version    : TtakInt32;
    Preset     : TtakPresets;
    Evaluation : TtakPresetEvaluations;
  end;
  Ttak_str_MetaEncoderInfoData = Array[0..tak_str_MetaEncoderInfoSize - 1] of Byte;



function tak_str_EncodeMetaEncoderInfo (const AInfo : Ttak_str_MetaEncoderInfo;
                                              ABits : TBitOutStream) : Boolean;

function tak_str_DecodeMetaEncoderInfo (    ABits : TBitInStream;
                                        var AInfo : Ttak_str_MetaEncoderInfo) : Boolean;


                                        
{=== Meta: Md5 ======================================================}

{
  128 MD5
------------
  128 bits
}

const
  tak_str_MetaMd5DataSize = 16;
  tak_str_MetaMd5Size     =   tak_str_MetaMd5DataSize
                            + tak_str_CrcSize;

type
  Ttak_str_MetaMd5 = packed record
    Sum : TMD5State;
  end;
  Ttak_str_MetaMd5Data = Array[0..tak_str_MetaMd5Size - 1] of Byte;



function tak_str_EncodeMetaMd5 (const AMd5  : Ttak_str_MetaMd5;
                                var   AData : Ttak_str_MetaMd5Data) : Boolean;

function tak_str_DecodeMetaMd5 (const AData : Ttak_str_MetaMd5Data;
                                var   AMd5  : Ttak_str_MetaMd5) : Boolean;



{=== Meta: LastFrameInfo ============================================}

{ Position and Size of the last frame relative to the beginning of the
  compressed audio data (first frame).

  40 Position
  24 Size
------------
  64 bits
}

const
  tak_str_MetaLastFrameInfoPosSize  = 5;
  tak_str_MetaLastFrameInfoSizeSize = 3;
  tak_str_MetaLastFrameInfoDataSize =   tak_str_MetaLastFrameInfoPosSize
                                      + tak_str_MetaLastFrameInfoSizeSize;
  tak_str_MetaLastFrameInfoSize     =   tak_str_MetaLastFrameInfoDataSize
                                      + tak_str_CrcSize;

type
  Ttak_str_MetaLastFrameInfo = packed record
    Pos  : TtakInt64;
    Size : TtakInt32;
  end;
  Ttak_str_MetaLastFrameInfoData = Array[0..tak_str_MetaLastFrameInfoSize - 1] of Byte;



function tak_str_EncodeMetaLastFrameInfo (const AFrame : Ttak_str_MetaLastFrameInfo;
                                          var   AData  : Ttak_str_MetaLastFrameInfoData) : Boolean;

function tak_str_DecodeMetaLastFrameInfo (const AData  : Ttak_str_MetaLastFrameInfoData;
                                          var   AFrame : Ttak_str_MetaLastFrameInfo) : Boolean;



implementation

{=== Frame size =====================================================}

const
  FrameDurationTypeQuants :
    Array[tak_FrameSizeType_First..tak_FrameSizeType_Last] of TtakInt32 =
    (3, 4, 6, 8,
     4096, 8192, 16384, 512, 1024, 2048);


function CalcFrameSizeFromTimeType (ASampleRate : TtakInt32;
                                    AType       : TtakFrameSizeTypes) : Integer;

begin
  Result :=     (ASampleRate * FrameDurationTypeQuants[AType])
            shr tak_str_FrameDurationQuantShift;
end;


function tak_str_CalcFrameSizeFromSizeType (    AType : TtakFrameSizeTypes;
                                            var ASize : TtakInt32) : Boolean;

begin
  if (AType < tak_FrameSizeType_Size_First)
  or (AType > tak_FrameSizeType_Size_Last) then
    Result := False
  else
  begin
    ASize  := FrameDurationTypeQuants[AType];
    Result := True;
  end;
end;


function tak_str_CalcFrameSizeFromType (    ASampleRate : TtakInt32;
                                        var AType       : TtakFrameSizeTypes;
                                        var ASize       : TtakInt32) : Boolean;

var
  MaxSize : TtakInt32;

begin
  if AType <= tak_FrameSizeType_Time_Last then begin
    ASize := CalcFrameSizeFromTimeType (ASampleRate, AType);
    if ASize > adc_BlockSizeMax then begin
      ASize  := adc_BlockSizeMax;
      AType  := tak_FrameSizeType_Size_Max;
      Result := False;
    end
    else
      Result := True;
  end
  else
  begin
    ASize   := FrameDurationTypeQuants[AType];
    MaxSize := CalcFrameSizeFromTimeType (ASampleRate, tak_FrameSizeType_Time_Max);
    if ASize > MaxSize then begin
      ASize  := MaxSize;
      AType  := tak_FrameSizeType_Time_Max;
      Result := False;
    end
    else
      Result := True;
  end;
end;


function tak_str_CalcFrameTimeTypeFromSize (    ASampleRate : TtakInt32;
                                                ASize       : TtakInt32;
                                            var AType       : TtakFrameSizeTypes) : Boolean;

var
  TimeType : TtakFrameSizeTypes;
  MaxSize  : TtakInt32;

begin
  for TimeType := tak_FrameSizeType_Time_First to tak_FrameSizeType_Time_Last do
  begin
    MaxSize := CalcFrameSizeFromTimeType (ASampleRate, TimeType);
    if ASize <= MaxSize then begin
      AType  := TimeType;
      Result := True;
      EXIT;
    end;
  end;
  AType  := tak_FrameSizeType_Time_Last;
  Result := False;
end;


function tak_str_CalcFrameDurationFromSize (ASampleRate : TtakInt32;
                                            AFrameSize  : TtakInt32) : TtakInt32;

begin
  Result := (AFrameSize * 1000 + ASampleRate div 2) div ASampleRate;
end;


function tak_str_FramesPerQuantTime (ASampleRate   : TtakInt32;
                                     AFrameSize    : TtakInt32;
                                     ATimeInQuants : TtakInt32) : TtakInt32;

begin
  Result :=      (ASampleRate * ATimeInQuants)
             div (AFrameSize * tak_str_FrameDurationQuantResolution);
end;


function tak_str_CalcFrameNumFromSampleNum (ASampleNum : TtakInt32;
                                            AFrameSize : TtakInt32) : TtakInt32;

begin
  Result := (ASampleNum + AFrameSize - 1) div AFrameSize;
end;


function tak_str_CalcLastFrameSize (ASampleNum : TtakInt32;
                                    AFrameNum  : TtakInt32;
                                    AFrameSize : TtakInt32) : TtakInt32;

begin
  Result := ASampleNum - ((AFrameNum - 1) * AFrameSize);
end;


function tak_str_CalcInfoFrameNum (AFrameNum : TtakInt32;
                                   ADistance : TtakInt32) : TtakInt32;

{             1    2    3   Distance
 AFrameNum
    1         1    1    1
    2         2    2    2
    3         3    2    2
    4         4    3    2
    5         5    3    3
    6         6    4    3
    7         7    4    3
    8         8    5    4
}

begin
  Result := 1 + (AFrameNum + ADistance - 2) div ADistance;
    { Possibly clearer:

        1 + ((AFrameNum - 1) + (ADistance - 1)) div ADistance

      - First position is 1.
      - Following positions on (1 + x * ADistance).
      - Since the last frame is always an InfoFrame, we add 1 if
        ((AFrameNum - 1) mod ADistance) > 0.
    }
end;



{=== FrameInfo ======================================================}

function tak_str_EncodeFrameInfo (const AInfo : Ttak_str_FrameInfo;
                                        ABits : TBitOutStream) : Boolean;

begin
  ABits.PutLong (tak_str_FrameInfoSyncId, tak_str_FrameInfoSyncIdBits);
  ABits.PutBool (AInfo.LastFrame);
  ABits.PutBool (AInfo.InfoFrame);
  ABits.PutBool (AInfo.MetaData);
  ABits.PutLong (AInfo.FrameNo, tak_str_FrameInfoFrameNoBits);

  if AInfo.LastFrame then begin
    ABits.PutLong (AInfo.SampleCount - 1, tak_str_FrameInfoSampleCountBits);
    ABits.PutLong (0, 2);
  end;

  Result := True;
end;


function tak_str_DecodeFrameInfo (    ABits : TBitInStream;
                                  var AInfo : Ttak_str_FrameInfo) : Boolean;

var
  Bits : TtakInt32;

begin
  if not ABits.GetLong_Check (tak_str_FrameInfoSyncIdBits, Bits)
  or (Bits <> tak_str_FrameInfoSyncId) then begin
    Result := False;
    EXIT;
  end;

  AInfo.LastFrame := ABits.GetBit <> 0;
  AInfo.InfoFrame := ABits.GetBit <> 0;
  AInfo.MetaData  := ABits.GetBit <> 0;
  AInfo.FrameNo   := ABits.GetLong (tak_str_FrameInfoFrameNoBits);

  if AInfo.LastFrame then begin
    AInfo.SampleCount := ABits.GetLong (tak_str_FrameInfoSampleCountBits) + 1;
    ABits.GetLong (2);
  end
  else
    AInfo.SampleCount := 0;
  Result := not ABits.Overflow;
end;



{=== Encoder-Info ===================================================}

function EncodeEncoderInfo (const AInfo : Ttak_str_EncoderInfo;
                                  ABits : TBitOutStream) : Boolean;

begin
  ABits.PutLong (AInfo.Codec,   tak_str_EncoderInfoCodecBits);
  ABits.PutLong (AInfo.Profile, tak_str_EncoderInfoProfileBits);
  Result := True;
end;


function DecodeEncoderInfo (    ABits : TBitInStream;
                            var AInfo : Ttak_str_EncoderInfo) : Boolean;

begin
  AInfo.Codec   := ABits.GetLong (tak_str_EncoderInfoCodecBits);
  AInfo.Profile := ABits.GetLong (tak_str_EncoderInfoProfileBits);
  Result        := not ABits.Overflow;
end;



{=== Size-Info ======================================================}

procedure EncodeSizeInfo (const AInfo : Ttak_str_SizeInfo;
                                ABits : TBitOutStream);

begin
  ABits.PutLong (Ord (AInfo.FrameSize),       tak_str_SizeInfoFrameDurationBits);
  ABits.PutLong (AInfo.SampleNum and $FFFFFF, 24);
  ABits.PutLong (AInfo.SampleNum shr 24,      tak_str_SizeInfoSampleNumBits - 24);
end;


function DecodeSizeInfo (    ABits   : TBitInStream;
                         var AInfo : Ttak_str_SizeInfo) : Boolean;

var
  Value : TtakInt32;

begin
  Value := ABits.GetLong (tak_str_SizeInfoFrameDurationBits);
  if Value > Ord (tak_FrameSizeType_Last) then begin
    Result := False;
    EXIT;
  end;
  AInfo.FrameSize := TtakFrameSizeTypes (Value);

  AInfo.SampleNum := ABits.GetLong (24);
  AInfo.SampleNum :=    AInfo.SampleNum
                     or (ABits.GetLong (tak_str_SizeInfoSampleNumBits - 24) shl 24);
  Result := not ABits.Overflow;
end;



{=== Audio-Format ===================================================}

function tak_str_AudioFormatSupported (const AFormat : TtakAudioFormat) : Boolean;

begin
  Result :=     (AFormat.DataType    = tak_str_AudioFormatDataType_PCM)
            and (AFormat.SampleRate >= tak_str_SampleRateMin)
            and (AFormat.SampleRate <= tak_str_SampleRateMax)
            and (AFormat.SampleBits >= tak_str_SampleBitsMin)
            and (AFormat.SampleBits <= tak_str_SampleBitsMax)
            and (AFormat.ChannelNum >= tak_str_ChannelNumMin)
            and (AFormat.ChannelNum <= tak_str_ChannelNumMax);
end;


function EncodeAudioFormat (const AFormat : TtakAudioFormat;
                                  ABits   : TBitOutStream) : Boolean;

begin
  if not tak_str_AudioFormatSupported (AFormat) then begin
    Result := False;
    EXIT;
  end;

  ABits.PutLong (AFormat.DataType, tak_str_AudioFormatDataTypeBits);
  ABits.PutLong (AFormat.SampleRate - tak_str_SampleRateMin,
                 tak_str_AudioFormatSampleRateBits);
  ABits.PutLong (AFormat.SampleBits - tak_str_SampleBitsMin,
                 tak_str_AudioFormatSampleBitsBits);
  ABits.PutLong (AFormat.ChannelNum - tak_str_ChannelNumMin,
                 tak_str_AudioFormatChannelNumBits);

  Result := True;
end;


function DecodeAudioFormat (    ABits   : TBitInStream;
                            var AFormat : TtakAudioFormat) : Boolean;

begin
  AFormat.DataType   := ABits.GetLong (tak_str_AudioFormatDataTypeBits);
  AFormat.SampleRate :=   ABits.GetLong (tak_str_AudioFormatSampleRateBits)
                        + tak_str_SampleRateMin;
  AFormat.SampleBits :=   ABits.GetLong (tak_str_AudioFormatSampleBitsBits)
                        + tak_str_SampleBitsMin;
  AFormat.ChannelNum :=   ABits.GetLong (tak_str_AudioFormatChannelNumBits)
                        + tak_str_ChannelNumMin;
  Result := not ABits.Overflow and tak_str_AudioFormatSupported (AFormat);
  if Result then
    takAudioFormat_CalcAndSetBlockSize (AFormat);
end;



{=== Stream-Info ====================================================}

function tak_str_EncodeStreamInfo (const AFormat : Ttak_str_StreamInfo;
                                         ABits   : TBitOutStream) : Boolean;

begin
  EncodeEncoderInfo (AFormat.Encoder, ABits);
  EncodeSizeInfo (AFormat.Sizes, ABits);
  Result := EncodeAudioFormat (AFormat.Audio, ABits);
end;


function tak_str_DecodeStreamInfo (    ABits   : TBitInStream;
                                   var AFormat : Ttak_str_StreamInfo) : Boolean;

begin
  Result :=     DecodeEncoderInfo (ABits, AFormat.Encoder)
            and DecodeSizeInfo    (ABits, AFormat.Sizes)
            and DecodeAudioFormat (ABits, AFormat.Audio)
            and tak_str_CalcFrameSizeFromType (AFormat.Audio.SampleRate,
                                               AFormat.Sizes.FrameSize,
                                               AFormat.Sizes.FrameSizeInSamples);
end;


function tak_str_StreamInfoEqual (const AA : Ttak_str_StreamInfo;
                                  const AB : Ttak_str_StreamInfo) : Boolean;

begin
  Result :=     (AA.Encoder.Codec       = AB.Encoder.Codec)
            and (AA.Encoder.Profile     = AB.Encoder.Profile)
            and (AA.Audio.DataType      = AB.Audio.DataType)
            and (AA.Audio.SampleRate    = AB.Audio.SampleRate)
            and (AA.Audio.SampleBits    = AB.Audio.SampleBits)
            and (AA.Audio.ChannelNum    = AB.Audio.ChannelNum)
            and (AA.Sizes.FrameSize     = AB.Sizes.FrameSize)
            and (AA.Sizes.SampleNum     = AB.Sizes.SampleNum);
end;



{=== Header =========================================================}

function tak_str_EncodeFrameHeader (const AHeader : Ttak_str_FrameHeader;
                                          ABits   : TBitOutStream) : Boolean;

begin
  tak_str_EncodeFrameInfo (AHeader.Info, ABits);

  if AHeader.Info.InfoFrame then begin
    if not tak_str_EncodeStreamInfo (AHeader.Format, ABits) then begin
      Assert (False, 'Extended stream format not supported');
    end;

    ABits.PutBool (AHeader.HasPrevDist);
    ABits.PutLong (0, tak_str_InfoFrameFlagUnusedNum);
    if AHeader.HasPrevDist then
      ABits.PutLong (AHeader.PrevDist, tak_str_InfoFrameDistanceBits);
    ABits.AlignEndToByte;
  end;

  ABits.PutLong (CalcCrc24_RFC2440 (@ABits.DataStart^, ABits.BytesWritten),
                 tak_str_CrcBits);

  Assert (ABits.BytesWritten <= tak_str_FrameHeaderSizeMax,
          'InfoFrameHeader too big');

  Result := True;
end;


function tak_str_DecodeFrameHeader (    ABits   : TBitInStream;
                                    var AHeader : Ttak_str_FrameHeader) : Boolean;

var
  Flags : TtakInt32;
  Crc   : Ttak_str_Crc;

begin
  if not tak_str_DecodeFrameInfo (ABits, AHeader.Info)
  or AHeader.Info.MetaData then begin
    Result := False;
    EXIT;
  end;

  if AHeader.Info.InfoFrame then begin
    if not tak_str_DecodeStreamInfo (ABits, AHeader.Format) then begin
      Result := False;
      EXIT;
    end;
    AHeader.HasPrevDist := ABits.GetBit <> 0;
    Flags               := ABits.GetLong (tak_str_InfoFrameFlagUnusedNum);
    if AHeader.HasPrevDist then
      AHeader.PrevDist := ABits.GetLong (tak_str_InfoFrameDistanceBits);
    Flags := Flags or ABits.AlignEndToByte;
  end
  else
    Flags := 0;

  Crc := ABits.GetLong (tak_str_CrcBits);

  Result := not ABits.Overflow
            and (Flags = 0)
            and (CalcCrc24_RFC2440 (@ABits.DataStart^,
                                    ABits.BytesRead - tak_str_CrcSize) = Crc);
end;



{=== Meta: Data header ==============================================}

function tak_str_EncodeMetaDataHeader (const AHeader : Ttak_str_MetaDataHeader;
                                       var   AData   : Ttak_str_MetaDataHeaderData) : Boolean;

begin
  AData :=    AHeader.Id
           or (AHeader.Size shl tak_str_MedaDataHeaderDataSizePos);
  Result := True;
end;


function tak_str_DecodeMetaDataHeader (const AData   : Ttak_str_MetaDataHeaderData;
                                       var   AHeader : Ttak_str_MetaDataHeader) : Boolean;

begin
  AHeader.Id   := AData and tak_str_MedaDataHeaderIdMask;
  AHeader.Size :=
        (AData shr tak_str_MedaDataHeaderDataSizePos)
    and ((Ttak_str_MetaDataHeaderData (1) shl tak_str_MedaDataHeaderDataSizeBits) - 1);
  Result := True;
end;



{=== Meta: StreamInfo ===============================================}

function tak_str_EncodeMetaStreamInfo (const AInfo : Ttak_str_StreamInfo;
                                             ABits : TBitOutStream) : Boolean;

begin
  if tak_str_EncodeStreamInfo (AInfo, ABits) then begin
    ABits.PutLong (CalcCrc24_RFC2440 (@ABits.DataStart^, ABits.BytesWritten),
                   tak_str_CrcBits);
    Result := True;
  end
  else
    Result := False;
end;


function tak_str_DecodeMetaStreamInfo (    ABits : TBitInStream;
                                       var AInfo : Ttak_str_StreamInfo) : Boolean;

var
  Crc : Ttak_str_Crc;

begin
  Result :=     tak_str_DecodeStreamInfo (ABits, AInfo)
            and ABits.GetLong_Check (tak_str_CrcBits, TtakInt32 (Crc))
            and (Crc = CalcCrc24_RFC2440
                        (@ABits.DataStart^,
                         ABits.BytesRead - tak_str_CrcSize));
end;



{=== Meta: SimpleWaveData ===========================================}

function tak_str_EncodeSimpleWaveData (const AInfo : Ttak_str_SimpleWaveDataHeader;
                                       var   AData : Ttak_str_SimpleWaveDataData) : Boolean;

begin
  Move (AInfo.FHeadSize, AData[0],
        tak_str_SimpleWaveDataSizeSize);
  Move (AInfo.FTailSize, AData[tak_str_SimpleWaveDataSizeSize],
        tak_str_SimpleWaveDataSizeSize);
  Result := True;
end;


function tak_str_DecodeSimpleWaveData (const AData : Ttak_str_SimpleWaveDataData;
                                       var   AInfo : Ttak_str_SimpleWaveDataHeader) : Boolean;

begin
  AInfo.FHeadSize := 0;
  Move (AData[0], AInfo.FHeadSize,
        tak_str_SimpleWaveDataSizeSize);
  AInfo.FTailSize := 0;
  Move (AData[tak_str_SimpleWaveDataSizeSize],
        AInfo.FTailSize, tak_str_SimpleWaveDataSizeSize);
  Result := True;
end;


function tak_str_SimpleWaveDataEqual (const AA : Ttak_str_SimpleWaveDataHeader;
                                      const AB : Ttak_str_SimpleWaveDataHeader) : Boolean;

begin
  Result :=     (AA.FHeadSize = AB.FTailSize)
            and (AA.FTailSize = AB.FTailSize);
end;



{=== Meta: EncoderInfo ==============================================}

function tak_str_EncodeMetaEncoderInfo (const AInfo : Ttak_str_MetaEncoderInfo;
                                              ABits : TBitOutStream) : Boolean;

begin
  ABits.PutLong (AInfo.Version,    tak_str_MetaEncoderInfoVersionBits);
  ABits.PutLong (AInfo.Preset,     tak_str_MetaEncoderInfoPresetBits);
  ABits.PutLong (AInfo.Evaluation, tak_str_MetaEncoderInfoEvaluationBits);
  ABits.AlignEndToByte;
  ABits.PutLong (CalcCrc24_RFC2440 (@ABits.DataStart^, ABits.BytesWritten),
                 tak_str_CrcBits);
  Result := True;
end;


function tak_str_DecodeMetaEncoderInfo (    ABits : TBitInStream;
                                        var AInfo : Ttak_str_MetaEncoderInfo) : Boolean;

var
  Crc : Ttak_str_Crc;

begin
  AInfo.Version    := ABits.GetLong (tak_str_MetaEncoderInfoVersionBits);
  AInfo.Preset     := ABits.GetLong (tak_str_MetaEncoderInfoPresetBits);
  AInfo.Evaluation := ABits.GetLong (tak_str_MetaEncoderInfoEvaluationBits);
  ABits.AlignEndToByte;
  Result := not ABits.Overflow
            and ABits.GetLong_Check (tak_str_CrcBits, TtakInt32 (Crc))
            and (Crc = CalcCrc24_RFC2440
                        (@ABits.DataStart^,
                         ABits.BytesRead - tak_str_CrcSize));
end;



{=== Meta: Md5 ======================================================}

function tak_str_EncodeMetaMd5 (const AMd5  : Ttak_str_MetaMd5;
                                var   AData : Ttak_str_MetaMd5Data) : Boolean;

var
  Crc : Ttak_str_Crc;

begin
  Move (AMd5.Sum, AData[0], tak_str_MetaMd5DataSize);
  Crc := CalcCrc24_RFC2440 (@AData, tak_str_MetaMd5DataSize);
  Move (Crc, AData[tak_str_MetaMd5DataSize], tak_str_CrcSize);
  Result := True;
end;


function tak_str_DecodeMetaMd5 (const AData : Ttak_str_MetaMd5Data;
                                var   AMd5  : Ttak_str_MetaMd5) : Boolean;

var
  Crc : Ttak_str_Crc;

begin
  Move (AData[0], AMd5.Sum, tak_str_MetaMd5DataSize);
  Crc := 0;
  Move (AData[tak_str_MetaMd5DataSize], Crc, tak_str_CrcSize);
  Result := Crc = CalcCrc24_RFC2440 (@AData[0], tak_str_MetaMd5DataSize);
end;



{=== Meta: LastFrameInfo ============================================}

function tak_str_EncodeMetaLastFrameInfo (const AFrame : Ttak_str_MetaLastFrameInfo;
                                          var   AData  : Ttak_str_MetaLastFrameInfoData) : Boolean;

var
  Crc : Ttak_str_Crc;

begin
  Move (AFrame.Pos,  AData[0],                            tak_str_MetaLastFrameInfoPosSize);
  Move (AFrame.Size, AData[tak_str_MetaLastFrameInfoPosSize], tak_str_MetaLastFrameInfoSizeSize);
  Crc := CalcCrc24_RFC2440 (@AData, tak_str_MetaLastFrameInfoDataSize);
  Move (Crc, AData[tak_str_MetaLastFrameInfoDataSize], tak_str_CrcSize);
  Result := True;
end;


function tak_str_DecodeMetaLastFrameInfo (const AData  : Ttak_str_MetaLastFrameInfoData;
                                          var   AFrame : Ttak_str_MetaLastFrameInfo) : Boolean;

var
  Crc : Ttak_str_Crc;

begin
  AFrame.Pos := 0;
  Move (AData[0], AFrame.Pos, tak_str_MetaLastFrameInfoPosSize);
  AFrame.Size := 0;
  Move (AData[tak_str_MetaLastFrameInfoPosSize], AFrame.Size, tak_str_MetaLastFrameInfoSizeSize);
  Crc := 0;
  Move (AData[tak_str_MetaLastFrameInfoDataSize], Crc, tak_str_CrcSize);
  Result := Crc = CalcCrc24_RFC2440 (@AData[0], tak_str_MetaLastFrameInfoDataSize);
end;



end.

