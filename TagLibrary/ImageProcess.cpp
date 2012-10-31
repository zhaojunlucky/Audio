#include "ImageProcess.h"




HBITMAP DecodePicture(char *sMIME, char *memory_block, unsigned int block_size, unsigned int *pnWidth, unsigned int *pnHeight)
{
	HBITMAP out = NULL;

	if(_stricmp(sMIME, "image/png") == 0)
	{
		// draw png image
		png_byte *ppbImage = NULL;
		unsigned int pxImgSize;
		unsigned int pyImgSize;
		unsigned int piChannels;
		png_color pBkgColor = {127, 127, 127};

		int ret = PngLoadImage((unsigned char*) memory_block , block_size,
								&ppbImage, &pxImgSize, &pyImgSize, &piChannels, &pBkgColor); 


		if (ppbImage != NULL)
		{
					
			BYTE *bits = NULL;
			HDC hdc =  CreateCompatibleDC(NULL);
			if(hdc)
			{
				out = CreateBitmapFromMemory1(hdc, (int)pxImgSize, -(int)pyImgSize, 32, NULL, 0,
							ppbImage, pxImgSize * pyImgSize * 3, 1);

				if(out)
				{	
					*pnWidth = pxImgSize;
					*pnHeight = pyImgSize;
				}

				DeleteDC(hdc);
			}
					
			free(ppbImage);
		}
	}
	else if(_stricmp(sMIME, "image/jpeg") == 0 ||0 ==  _stricmp(sMIME,"image/jpg"))
	{

		out = read_JPEG_file(memory_block, block_size, pnWidth, pnHeight);
		if(out)
		{
			BITMAP b;
			if(GetObject(out, sizeof(BITMAP), &b))
			{
				*pnWidth = b.bmWidth;
				*pnHeight = b.bmHeight;
			}
		}

	}
	else if(_stricmp(sMIME, "image/bmp") == 0)
	{
			
		unsigned char *ptr = (unsigned char*) memory_block;
		BITMAPFILEHEADER *fh = (BITMAPFILEHEADER*) ptr;
		ptr += sizeof(BITMAPFILEHEADER);

		BITMAPINFOHEADER *ih = (BITMAPINFOHEADER*) ptr;
		ptr += sizeof(BITMAPINFOHEADER);


		int err = 0;
		// check if this is valid bitmap
		if(fh->bfType != 19778 || fh->bfSize != block_size || fh->bfReserved1 != 0 || fh->bfReserved2 != 0)
			err = 1;

		// check if this is valid bitmap
		if(ih->biSize != sizeof(BITMAPINFOHEADER) || ih->biPlanes != 1)
			 err = 1;

		// check if this is valid bitmap
		if(ih->biBitCount != 1 && ih->biBitCount != 4 && ih->biBitCount != 8 && ih->biBitCount != 16 && 
			ih->biBitCount != 24 && ih->biBitCount != 32)
		{
			err = 1;
		}

		if(ih->biCompression != BI_RGB)
			err = 1;


		RGBQUAD *rgb = NULL;
		if(ih->biClrUsed)
		{
			rgb = (RGBQUAD*) ptr;
			ptr += (ih->biClrUsed * sizeof(RGBQUAD));
		}


		char *bits = (char*) ptr;


		HDC hdc =  CreateCompatibleDC(NULL);
		if(hdc)
		{

			out = CreateBitmapFromMemory1(hdc, ih->biWidth, ih->biHeight, ih->biBitCount,
						rgb, ih->biClrUsed, bits, ih->biSizeImage, 0);

			DeleteDC(hdc);
		}

		if(out)
		{
			BITMAP b;
			if(GetObject(out, sizeof(BITMAP), &b))
			{
				*pnWidth = b.bmWidth;
				*pnHeight = b.bmHeight;
			}
		}
	}

	return out;
}


unsigned int PngLoadImage(png_byte *memory_block, unsigned int block_size, png_byte **ppbImageData,
                   unsigned int *piWidth, unsigned int *piHeight, unsigned int *piChannels, png_color *pBkgColor)
{
 
    png_byte            *pbSig = memory_block;
    int                 iBitDepth;
    int                 iColorType;
    double              dGamma;
    png_color_16       *pBackground;
    png_uint_32         ulChannels;
    png_uint_32         ulRowBytes;
    png_byte           *pbImageData = *ppbImageData;
    static png_byte   **ppbRowPointers = NULL;
    //int                 i;


	if(block_size < 8)
		return 0;

    // first check the eight byte PNG signature
    if (png_sig_cmp(pbSig, 0, 8))
    {
        *ppbImageData = pbImageData = NULL;
        return 0;
    }

    // create the two png(-info) structures

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr) png_cexcept_error, (png_error_ptr) NULL);
    if (png_ptr == NULL)
    {
        *ppbImageData = pbImageData = NULL;
        return 0;
    }

	png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        *ppbImageData = pbImageData = NULL;
        return 0;
    }

    try
    {
        
		PNG_STREAM stream;
		stream.start = memory_block;
		stream.pos = memory_block;
		stream.size = block_size;
        // initialize the png structure
        

        png_set_read_fn(png_ptr, (png_voidp)&stream, png_read_data);

        
        // read all PNG info up to image data
        
        png_read_info(png_ptr, info_ptr);
        
        // get width, height, bit-depth and color-type
        
        png_get_IHDR(png_ptr, info_ptr, piWidth, piHeight, &iBitDepth, &iColorType, NULL, NULL, NULL);
        
        // expand images of all color-type and bit-depth to 3x8 bit RGB images
        // let the library process things like alpha, transparency, background
        
        if (iBitDepth == 16)
            png_set_strip_16(png_ptr);
        if (iColorType == PNG_COLOR_TYPE_PALETTE)
            png_set_expand(png_ptr);
        if (iBitDepth < 8)
            png_set_expand(png_ptr);
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
            png_set_expand(png_ptr);
        if (iColorType == PNG_COLOR_TYPE_GRAY ||
            iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png_ptr);
        
			
        // set the background color to draw transparent and alpha images over.
        if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
        {
            png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
            pBkgColor->red   = (BYTE) pBackground->red;
            pBkgColor->green = (BYTE) pBackground->green;
            pBkgColor->blue  = (BYTE) pBackground->blue;
        }
        else
        {
            pBkgColor = NULL;
        }
        
        // if required set gamma conversion
        if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
            png_set_gamma(png_ptr, (double) 2.2, dGamma);
        
        // after the transformations have been registered update info_ptr data
        
        png_read_update_info(png_ptr, info_ptr);
        
        // get again width, height and the new bit-depth and color-type
        
        png_get_IHDR(png_ptr, info_ptr, piWidth, piHeight, &iBitDepth,
            &iColorType, NULL, NULL, NULL);
        
        
        // row_bytes is the width x number of channels
        
        ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
        ulChannels = png_get_channels(png_ptr, info_ptr);
        
        *piChannels = ulChannels;
        
        // now we can allocate memory to store the image
        
        if (pbImageData)
        {
            free (pbImageData);
            pbImageData = NULL;
        }
        if ((pbImageData = (png_byte *) malloc(ulRowBytes * (*piHeight)
                            * sizeof(png_byte))) == NULL)
        {
            png_error(png_ptr, "Visual PNG: out of memory");
        }
        *ppbImageData = pbImageData;
        
        // and allocate memory for an array of row-pointers
        
        if ((ppbRowPointers = (png_bytepp) malloc((*piHeight)
                            * sizeof(png_bytep))) == NULL)
        {
            png_error(png_ptr, "Visual PNG: out of memory");
        }
        
        // set the individual row-pointers to point at the correct offsets
        
        for (unsigned int i = 0; i < (*piHeight); i++)
            ppbRowPointers[i] = pbImageData + i * ulRowBytes;
        
        // now we can go ahead and just read the whole image
        
        png_read_image(png_ptr, ppbRowPointers);
        
        // read the additional chunks in the PNG file (not really needed)
        
        png_read_end(png_ptr, NULL);
        
        // and we're done
        
        free (ppbRowPointers);
        ppbRowPointers = NULL;
        
        // yepp, done
    }

    catch (...)
    {

        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

        *ppbImageData = pbImageData = NULL;
        
        if(ppbRowPointers)
            free (ppbRowPointers);


        return 0;
    }



    return 1;
}

HBITMAP CreateBitmapFromMemory1(HDC hdc, int nWidth, int nHeight, unsigned int nBitPerPixel,
			RGBQUAD *rgbColorTable, unsigned int nColorUsed,
			VOID *pBitmapBitsArray, unsigned int nSizeOfBitmapArray,
			unsigned int SwapRGB)
{
	unsigned int fDeleteDC = 0;
	if(hdc == 0) {
		hdc = CreateCompatibleDC(NULL);
		if(hdc == 0) return 0;
		fDeleteDC = 1;
	}			
	if(nWidth == 0 ||nHeight == 0 || nBitPerPixel == 0) return 0;
	if(nColorUsed != 0 && rgbColorTable == 0) return 0;
	// allocate memory for BITMAPINFO + memory for RGBQUAD color table
	BITMAPINFO *pBitmapInfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO) + nColorUsed * sizeof(RGBQUAD));
	if(pBitmapInfo == 0) return 0;
	// set BITMAPINFO VALUES
	pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBitmapInfo->bmiHeader.biWidth = nWidth;
	pBitmapInfo->bmiHeader.biHeight = nHeight;
	pBitmapInfo->bmiHeader.biPlanes = 1;
	pBitmapInfo->bmiHeader.biBitCount = nBitPerPixel; 
	pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	pBitmapInfo->bmiHeader.biSizeImage = nSizeOfBitmapArray;
	pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	pBitmapInfo->bmiHeader.biClrUsed = nColorUsed;
	pBitmapInfo->bmiHeader.biClrImportant = 0;
	unsigned int i;
	for(i = 0; i < nColorUsed; i++) {
		pBitmapInfo->bmiColors[i].rgbBlue = rgbColorTable[i].rgbBlue;
		pBitmapInfo->bmiColors[i].rgbGreen = rgbColorTable[i].rgbGreen;
		pBitmapInfo->bmiColors[i].rgbRed = rgbColorTable[i].rgbRed;
		pBitmapInfo->bmiColors[i].rgbReserved = 0;
	}
	// create bitmap
	VOID *pBits;
	HBITMAP hbm = CreateDIBSection(hdc, pBitmapInfo, DIB_RGB_COLORS, &pBits, NULL, 0); 
	if(fDeleteDC)
		DeleteDC(hdc);
	free(pBitmapInfo);
	if(hbm == 0) return 0;


	if(SwapRGB)
	{
		unsigned char *dest = (unsigned char*) pBits;
		unsigned char *src = (unsigned char*) pBitmapBitsArray;

		if(nBitPerPixel == 32)
		{
			unsigned int j = 0;
			for(i = 0; i < nSizeOfBitmapArray; i += 3)
			{
				dest[j] = src[i + 2];
				dest[j + 1] = src[i + 1];
				dest[j + 2] = src[i];
				dest[j + 3] = 0;
				j += 4;

			}
		}
		else
		{
			for(i = 0; i < nSizeOfBitmapArray; i += 3)
			{
				dest[i] = src[i + 2];
				dest[i + 1] = src[i + 1];
				dest[i + 2] = src[i];

			}
		}
	}
	else
	{
		memcpy(pBits, pBitmapBitsArray, nSizeOfBitmapArray);
	}


	return hbm;
}

HBITMAP read_JPEG_file(char *memory_block, unsigned int block_size, unsigned int *width, unsigned int *height)
{

	HBITMAP hbm = NULL;

  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr jerr;
  /* More stuff */
  
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */



  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  jerr.pub.output_message = my_output_message;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
	if(hbm)
		DeleteObject(hbm);
    return NULL;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_mem_src(&cinfo, (unsigned char*)  memory_block, block_size);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.txt for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

		


  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);





	
	HDC hdc = CreateCompatibleDC(NULL);
	if(hdc == 0)
	{
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}


	// allocate memory for BITMAPINFO + memory for RGBQUAD color table
	BITMAPINFO *pBitmapInfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO));
	if(pBitmapInfo == 0)
	{
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	// set BITMAPINFO VALUES
	pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBitmapInfo->bmiHeader.biWidth = cinfo.output_width;
	pBitmapInfo->bmiHeader.biHeight = - (long)cinfo.output_height;
	pBitmapInfo->bmiHeader.biPlanes = 1;
	pBitmapInfo->bmiHeader.biBitCount = 32; 
	pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	pBitmapInfo->bmiHeader.biSizeImage = 0 /*row_stride * cinfo.output_height*/;
	pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	pBitmapInfo->bmiHeader.biClrUsed = 0;
	pBitmapInfo->bmiHeader.biClrImportant = 0;

	unsigned int i;
	
	// create bitmap
	VOID *pBits;
	hbm = CreateDIBSection(hdc, pBitmapInfo, DIB_RGB_COLORS, &pBits, NULL, 0); 

	DeleteDC(hdc);
	if(hbm == 0)
	{
		
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	free(pBitmapInfo);//my

	unsigned char *dest = (unsigned char*) pBits;


  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
   
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */


	switch(cinfo.output_components)
	{
		case 3:
		{

			unsigned char *src = (unsigned char*) buffer[0];
			unsigned int j = 0;
			for(i = 0; i < (unsigned int)row_stride; i += 3)
			{
			
				dest[j] = src[i + 2];
				dest[j + 1] = src[i + 1];
				dest[j + 2] = src[i];
				dest[j + 3] = 0;
				j += 4;

			}

			dest += cinfo.output_width * 4;
		

		}
		break;


		case 1:
		{
			unsigned char *src = (unsigned char*) buffer[0];
			unsigned int j = 0;
			for(i = 0; i < (unsigned int)row_stride; i++)
			{
				dest[j] = src[i];
				dest[j + 1] = src[i];
				dest[j + 2] = src[i];
				dest[j + 3] = src[i];
				j+= 4;
			}

			dest += row_stride * 4;
		}
		break;

		default:
			jpeg_destroy_decompress(&cinfo);
			if(hbm)
				DeleteObject(hbm);
		return NULL;
	}

	

	
  }

  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */
  *width = cinfo.output_width;
  *height = cinfo.output_height;
  return hbm;
}

static void png_cexcept_error(png_structp png_ptr, png_const_charp msg)
{
    throw msg;
}


static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{

   PNG_STREAM *stream = (PNG_STREAM*) (png_ptr->io_ptr);

   if(stream->size == 0)
   {
		png_error(png_ptr, "Read Error");
		return;
	}

   if(length > stream->size)
	length = stream->size;

	memcpy(data, stream->pos, length);
	stream->pos += length;
	stream->size -= length;

}