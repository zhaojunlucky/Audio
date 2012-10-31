#pragma once

#include <Windows.h>

#define ADIF_MAX_SIZE 30 /* Should be enough */
#define ADTS_MAX_SIZE 10 /* Should be enough */

#define local_buffer_size_   64

typedef struct {
    int version;
    int channels;
    int sampling_rate;
    int bitrate;
    int length;
    int object_type;
    int headertype;
} faadAACInfo;

typedef struct {
    HANDLE stream;
    unsigned short inetStream;
    unsigned char *data;
    int http;
    int buffer_offset;
    int buffer_length;
    int file_offset;
    int http_file_length;
} FILE_STREAM;

static int sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000};

FILE_STREAM *open_filestream(char *filename);
unsigned long filelength_filestream(FILE_STREAM *fs);
int read_buffer_filestream(FILE_STREAM *fs, void *data, int length);
int read_byte_filestream(FILE_STREAM *fs);
void close_filestream(FILE_STREAM *fs);
unsigned long tell_filestream(FILE_STREAM *fs);

int read_ADIF_header(FILE_STREAM *file, faadAACInfo *info);
static int read_ADTS_header(FILE_STREAM *file, faadAACInfo *info,
                            unsigned long **seek_table, int *seek_table_len,
                            int tagsize, int no_seek_table);
int get_AAC_format(char *filename, faadAACInfo *info,
                   unsigned long **seek_table, int *seek_table_len,
                   int no_seek_table);
int StringComp(char const *str1, char const *str2, unsigned long len);