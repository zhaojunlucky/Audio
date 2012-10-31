#include <windows.h>

#include "..\decoders\zlib\zlib.h"
#include "..\decoders\jpeg\jpeglib.h"
//#include "..\decoders\lpng\pngstruct.h"
#include "..\decoders\lpng\png.h"


struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct {
	unsigned char *start;
	unsigned char *pos;
	unsigned int size;
} PNG_STREAM;




typedef struct my_error_mgr * my_error_ptr;

typedef struct my_error_mgr * my_error_ptr;
static void png_cexcept_error(png_structp png_ptr, png_const_charp msg);


static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length);


METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


METHODDEF(void)
my_output_message (j_common_ptr cinfo)
{
  char buffer[JMSG_LENGTH_MAX];

  /* Create the message */
  (*cinfo->err->format_message) (cinfo, buffer);

#ifdef _DEBUG
  /* Send it to stderr, adding a newline */
  fprintf(stderr, "%s\n", buffer);
#endif

}

__declspec(dllexport) HBITMAP DecodePicture(char *sMIME, char *memory_block, unsigned int block_size, unsigned int *pnWidth, unsigned int *pnHeight);
unsigned int PngLoadImage(png_byte *memory_block, unsigned int block_size, png_byte **ppbImageData,
                   unsigned int *piWidth, unsigned int *piHeight, unsigned int *piChannels, png_color *pBkgColor);
HBITMAP CreateBitmapFromMemory1(HDC hdc, int nWidth, int nHeight, unsigned int nBitPerPixel,
			RGBQUAD *rgbColorTable, unsigned int nColorUsed,
			VOID *pBitmapBitsArray, unsigned int nSizeOfBitmapArray,
			unsigned int SwapRGB);
HBITMAP read_JPEG_file(char *memory_block, unsigned int block_size, unsigned int *width, unsigned int *height);

