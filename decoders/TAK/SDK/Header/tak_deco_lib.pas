unit tak_deco_lib;

interface


{=== TAK SDK ========================================================}

{
  Software Developement Kit for TAK, (T)om's lossless (A)udio (K)ompressor:
  Decoder library.

  Version:  1.1.1
  Date:     09-02-13
  Language: Pascal (Delphi 6.0)

  Copyright 2007 by Thomas Becker, D-49080 Osnabrueck.
  All rights reserved.
}



{=== Elementary Types ===============================================}

type
  TtakInt64  = Int64;
  TtakInt32  = Integer;
  TtakUInt32 = Cardinal;
  TtakBool   = TtakInt32;

const
  tak_False = 0;
  tak_True  = 1;



{=== Function results and errors ====================================}

type
  TtakResult = TtakInt32;

const
  tak_ErrorStringLenMax  = 60;
  tak_ErrorStringSizeMax = tak_ErrorStringLenMax + 1;

const
  tak_res_Ok = 0;

  {---    1 to 1023: Object results ---}

  {--- 1024 to 2047: System errors ---}

  tak_res_InternalError       = 1024;
  tak_res_NotImplemented      = 1025;
  tak_res_IncompatibleVersion = 1026;
  tak_res_OutOfMemory         = 1027;

  {--- 2048 to 3071: User errors ---}

  tak_res_InvalidParameter   = 2048;
  tak_res_InvalidIoInterface = 2049;
  tak_res_InvalidMode        = 2050;
  tak_res_BufferTooSmall     = 2051;
  tak_res_NotEnoughAudioData = 2052;
  tak_res_TooMuchAudioData   = 2053;




{=== System =========================================================}

const
  tak_MaxPathLen  = 256;
  tak_MaxPathSize = tak_MaxPathLen + 1;

type
  TtakCpuOptions = TtakInt32;

const
  tak_Cpu_Asm  = $0001;
  tak_Cpu_MMX  = $0002;
  tak_Cpu_SSE  = $0004;
  tak_Cpu_None = $0000;
  tak_Cpu_Any  = tak_Cpu_Asm or tak_Cpu_MMX or tak_Cpu_SSE;



{=== Library ========================================================}

const
  tak_InterfaceVersion = $010001;


function tak_GetLibraryVersion (var AVersion       : TtakInt32;
                                var ACompatibility : TtakInt32) : TtakResult;
                                cdecl; external 'tak_deco_lib.dll';



{=== Audio-Format ===================================================}

const
  tak_AudioFormat_DataType_PCM = 0;

type
  TtakAudioFormat = packed record
    DataType   : TtakInt32;
    SampleRate : TtakInt32;
    SampleBits : TtakInt32;
    ChannelNum : TtakInt32;
    BlockSize  : TtakInt32;
  end;



{=== Codecs =========================================================}

const
  tak_CodecNameLenMax  = 30;
  tak_CodecNameSizeMax = tak_CodecNameLenMax + 1;


function tak_GetCodecName (ACodec    : TtakInt32;
                           AName     : PChar;
                           ANameSize : TtakInt32) : TtakResult;
                           cdecl; external 'tak_deco_lib.dll';



{=== Presets ========================================================}


  {-- Presets/Profiles ---}

type
  TtakPresets = TtakInt32;


  {-- Evaluation ---}

type
  TtakPresetEvaluations = TtakInt32;

const
  tak_PresetEval_Standard = 0;
  tak_PresetEval_Extra    = 1;
  tak_PresetEval_Max      = 2;
  tak_PresetEval_First    = tak_PresetEval_Standard;
  tak_PresetEval_Last     = tak_PresetEval_Max;
  tak_PresetEval_Num      = tak_PresetEval_Last - tak_PresetEval_First + 1;



{=== Stream / Container =============================================}

const
  tak_FrameSizeMax              = 16384;
  tak_FrameDurationMax          = 250;
  tak_str_SimpleWaveDataSizeMax = 1024 * 1024;

type
  Ttak_str_EncoderInfo = packed record
    Codec   : TtakInt32;
    Profile : TtakPresets;
  end;

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
  tak_FrameSizeType_512    =  7;
  tak_FrameSizeType_1024   =  8;
  tak_FrameSizeType_2048   =  9;

  tak_FrameSizeType_First  = tak_FrameSizeType_94_ms;
  tak_FrameSizeType_Last   = tak_FrameSizeType_2048;
  tak_FrameSizeType_Num    = tak_FrameSizeType_Last - tak_FrameSizeType_First + 1;

type
  Ttak_str_SizeInfo = packed record
    FrameSize          : TtakFrameSizeTypes;
    FrameSizeInSamples : TtakInt32;
    SampleNum          : TtakInt64;
  end;

type
  Ttak_str_StreamInfo = packed record
    Encoder : Ttak_str_EncoderInfo;
    Sizes   : Ttak_str_SizeInfo;
    Audio   : TtakAudioFormat;
  end;

type
  Ttak_str_SimpleWaveDataHeader = packed record
    HeadSize : TtakInt32;
    TailSize : TtakInt32;
  end;

type
  Ttak_str_MetaEncoderInfo = packed record
    Version    : TtakInt32;
    Preset     : TtakPresets;
    Evaluation : TtakPresetEvaluations;
  end;



{=== TtakStreamIoInterface ==========================================}

type
  PtakStreamIoInterface = ^TtakStreamIoInterface;
  TtakStreamIoInterface = packed record
    CanRead   : function (AUser : Pointer) : TtakBool; cdecl;
    CanWrite  : function (AUser : Pointer) : TtakBool; cdecl;
    CanSeek   : function (AUser : Pointer) : TtakBool; cdecl;
    Read      : function (    AUser    : Pointer;
                              ABuf     : Pointer;
                              ANum     : TtakInt32;
                          var AReadNum : TtakInt32) : TtakBool; cdecl;
    Write     : function (AUser : Pointer;
                          ABuf  : Pointer;
                          ANum  : TtakInt32) : TtakBool; cdecl;
    Flush     : function (AUser : Pointer) : TtakBool; cdecl;
    Truncate  : function (AUser : Pointer) : TtakBool; cdecl;
    Seek      : function (AUser : Pointer;
                          APos  : TtakInt64) : TtakBool; cdecl;
    GetLength : function (    AUser   : Pointer;
                          var ALength : TtakInt64) : TtakBool; cdecl;
  end;



{=== APEv2-Tag (APE) ================================================}

type
  TtakAPEv2Tag = Pointer;

const
  tak_apev2_Version    = 2000;
  tak_apev2_ItemNumMax = 100;
  tak_apev2_TagSizeMax = 16 * 1024 * 1024;

type
  TtakAPEv2ItemType = TtakInt32;

const
  tak_apev2_ItemType_Text     = 0;
  tak_apev2_ItemType_Binary   = 1;
  tak_apev2_ItemType_External = 2;
  tak_apev2_ItemType_Last     = tak_apev2_ItemType_External;

type
  TtakAPEv2ItemDesc = packed record
    ItemType  : TtakAPEv2ItemType;
    Flags     : TtakUInt32;
    KeySize   : TtakUInt32;
    ValueSize : TtakUInt32;
    ValueNum  : TtakInt32;
  end;

type
  TtakAPEv2TagDesc = packed record
    Version   : TtakUInt32;
    Flags     : TtakUInt32;
    StreamPos : TtakInt64;
    TotSize   : TtakInt64;
  end;


const

  {--- Warnings ---}

  tak_res_ape_NotAvail       = 1;
  tak_res_ape_InvalidType    = 2;
  tak_res_ape_BufferTooSmall = 3;

  {--- Fatal errors ---}

  tak_res_ape_None         = 4;
  tak_res_ape_Incompatible = 5;
  tak_res_ape_Invalid      = 6;
  tak_res_ape_IoErr        = 7;

  tak_res_ape_FatalErrorFirst = tak_res_ape_None;



{--- Info -----------------------------------------------------------}

function tak_APE_Valid (ATag : TtakAPEv2Tag) : TtakBool;
                        cdecl; external 'tak_deco_lib.dll';


function tak_APE_State (ATag : TtakAPEv2Tag) : TtakResult;
                        cdecl; external 'tak_deco_lib.dll';

function tak_APE_GetErrorString (AError      : TtakResult;
                                 AString     : PChar;
                                 AStringSize : TtakInt32) : TtakResult;
                                 cdecl; external 'tak_deco_lib.dll';

function tak_APE_ReadOnly (ATag : TtakAPEv2Tag) : TtakBool;
                           cdecl; external 'tak_deco_lib.dll';

function tak_APE_GetDesc (    ATag  : TtakAPEv2Tag;
                          var ADesc : TtakAPEv2TagDesc) : TtakResult;
                          cdecl; external 'tak_deco_lib.dll';

function tak_APE_GetItemNum (ATag : TtakAPEv2Tag) : TtakInt32;
                             cdecl; external 'tak_deco_lib.dll';



{--- Items ----------------------------------------------------------}

function tak_APE_GetIndexOfKey (    ATag : TtakAPEv2Tag;
                                    AKey : PChar;
                                var AIdx : TtakInt32) : TtakResult;
                                cdecl; external 'tak_deco_lib.dll';

function tak_APE_GetItemDesc (    ATag : TtakAPEv2Tag;
                                  AIdx  : TtakInt32;
                              var ADesc : TtakAPEv2ItemDesc) : TtakResult;
                              cdecl; external 'tak_deco_lib.dll';

function tak_APE_GetItemKey (    ATag     : TtakAPEv2Tag;
                                 AIdx     : TtakInt32;
                                 AKey     : PChar;
                                 AMaxSize : TtakInt32;
                             var ASize    : TtakInt32) : TtakResult;
                             cdecl; external 'tak_deco_lib.dll';

function tak_APE_GetItemValue (    ATag     : TtakAPEv2Tag;
                                   AIdx     : TtakInt32;
                                   AValue   : Pointer;
                                   AMaxSize : TtakInt32;
                               var ASize    : TtakInt32) : TtakResult;
                               cdecl; external 'tak_deco_lib.dll';

function tak_APE_GetTextItemValueAsAnsi (    ATag            : TtakAPEv2Tag;
                                             AIdx            : TtakInt32;
                                             AValueIdx       : TtakInt32;
                                             AValueSeparator : Char;
                                             AValue          : PChar;
                                             AMaxSize        : TtakInt32;
                                         var ASize           : TtakInt32) : TtakResult;
                                         cdecl; external 'tak_deco_lib.dll';



{=== Seekable Stream Decoder (SSD) ==================================}

type
  TtakSeekableStreamDecoder = Pointer;

type
  PtakSSDDamageItem = ^TtakSSDDamageItem;
  TtakSSDDamageItem = packed record
    SamplePosition : TtakInt64;
    SampleSize     : TtakInt64;
  end;
  TSSDDamageCallback = procedure (AUser   : Pointer;
                                  ADamage : PtakSSDDamageItem); cdecl;

const
  tak_ssd_opt_OpenWriteable     = $00000001;
  tak_ssd_opt_BufferInput       = $00000002;
  tak_ssd_opt_SequentialRead    = $00000004;
  tak_ssd_opt_SkipDamagedFrames = $00000008;
  tak_ssd_opt_CheckMd5          = $00000010;

type
  TtakSSDOptions = packed record
    Cpu   : TtakCpuOptions;
    Flags : TtakInt32;
  end;


const

  //--- Warnings ---

  tak_res_ssd_MetaDataMissing = 1;

  //--- Errors ---

  tak_res_ssd_MetaDataDamaged = 2;
  tak_res_ssd_FrameDamaged    = 3;
  tak_res_ssd_ErrorFirst      = tak_res_ssd_MetaDataDamaged;

  //--- Fatal Errors ---

  tak_res_ssd_SourceIoError       = 4;
  tak_res_ssd_IncompatibleVersion = 5;
  tak_res_ssd_Undecodable         = 6;
  tak_res_ssd_FatalErrorFirst     = tak_res_ssd_SourceIoError;

type
  TtakSSDResult = packed record
    OpenResult          : TtakResult;
    SumResult           : TtakResult;
    StreamSampleNum     : TtakInt64;
    ReadSampleNum       : TtakInt64;
    DamagedSampleNum    : TtakInt64;
    SkippedDataBlockNum : TtakInt32;
  end;



{--- Create & Destroy -----------------------------------------------}

function tak_SSD_Create_FromFile (      ASourcePath     : PChar;
                                  const AOptions        : TtakSSDOptions;
                                        ADamageCallback : TSSDDamageCallback;
                                        ACallbackUser   : Pointer): TtakSeekableStreamDecoder;
                                  cdecl; external 'tak_deco_lib.dll';

function tak_SSD_Create_FromStream (      ASourceStream     : PtakStreamIoInterface;
                                          ASourceStreamUser : Pointer;
                                    const AOptions          : TtakSSDOptions;
                                          ADamageCallback   : TSSDDamageCallback;
                                          ACallbackUser     : Pointer) : TtakSeekableStreamDecoder;
                                    cdecl; external 'tak_deco_lib.dll';

procedure tak_SSD_Destroy (ADecoder : TtakSeekableStreamDecoder);
                           cdecl; external 'tak_deco_lib.dll';



{--- Info -----------------------------------------------------------}

function tak_SSD_Valid (ADecoder : TtakSeekableStreamDecoder) : TtakBool;
                        cdecl; external 'tak_deco_lib.dll';

function tak_SSD_State (ADecoder : TtakSeekableStreamDecoder) : TtakResult;
                        cdecl; external 'tak_deco_lib.dll';

function tak_SSD_GetStateInfo (    ADecoder : TtakSeekableStreamDecoder;
                               var AInfo    : TtakSSDResult) : TtakResult;
                               cdecl; external 'tak_deco_lib.dll';

function tak_SSD_GetErrorString (AError      : TtakResult;
                                 AString     : PChar;
                                 AStringSize : TtakInt32) : TtakResult;
                                 cdecl; external 'tak_deco_lib.dll';

function tak_SSD_GetStreamInfo (    ADecoder : TtakSeekableStreamDecoder;
                                var AInfo    : Ttak_str_StreamInfo) : TtakResult;
                                cdecl; external 'tak_deco_lib.dll';

function tak_SSD_GetFrameSize (ADecoder : TtakSeekableStreamDecoder) : TtakInt32;
                               cdecl; external 'tak_deco_lib.dll';



{--- IO -------------------------------------------------------------}

function tak_SSD_Seek (ADecoder   : TtakSeekableStreamDecoder;
                       ASamplePos : TtakInt64) : TtakResult;
                       cdecl; external 'tak_deco_lib.dll';

function tak_SSD_ReadAudio (    ADecoder   : TtakSeekableStreamDecoder;
                                ASamples   : Pointer;
                                ASampleNum : TtakInt32;
                            var AReadNum   : TtakInt32) : TtakResult;
                            cdecl; external 'tak_deco_lib.dll';

function tak_SSD_GetReadPos (ADecoder : TtakSeekableStreamDecoder) : TtakInt64;
                             cdecl; external 'tak_deco_lib.dll';

function tak_SSD_GetCurFrameBitRate (ADecoder : TtakSeekableStreamDecoder) : TtakInt32;
                                     cdecl; external 'tak_deco_lib.dll';

function tak_SSD_GetSimpleWaveDataDesc (    ADecoder : TtakSeekableStreamDecoder;
                                        var ADesc    : Ttak_str_SimpleWaveDataHeader) : TtakResult;
                                        cdecl; external 'tak_deco_lib.dll';

function tak_SSD_ReadSimpleWaveData (ADecoder : TtakSeekableStreamDecoder;
                                     ABuf     : Pointer;
                                     ABufSize : TtakInt32) : TtakResult;
                                     cdecl; external 'tak_deco_lib.dll';

function tak_SSD_GetEncoderInfo (    ADecoder : TtakSeekableStreamDecoder;
                                 var AInfo    : Ttak_str_MetaEncoderInfo) : TtakResult;
                                 cdecl; external 'tak_deco_lib.dll';

function tak_SSD_GetAPEv2Tag (ADecoder : TtakSeekableStreamDecoder) : TtakAPEv2Tag;
                              cdecl; external 'tak_deco_lib.dll';



implementation

end.
