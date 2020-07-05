#include "stdafx.h"

#include "JPEGIJG.h"

#include <setjmp.h>

#define	IJGBLOCKSIZE 16384

extern "C"
{

#ifdef FAR
#undef FAR
#endif

#define boolean ijg_boolean

#include "dcmtk/dcmjpeg/libijg8/jpeglib8.h"
#include "dcmtk/dcmjpeg/libijg8/jerror8.h"

#undef boolean

// disable any preprocessor magic the IJG library might be doing with the "const" keyword
#ifdef const
#undef const
#endif



void my_output_message(j_common_ptr cinfo) 
{
	char buffer[JMSG_LENGTH_MAX];

	// Create the message 
	(*cinfo->err->format_message)(cinfo, buffer);

	//i_push_error(0, buffer);

	/* Send it to stderr, adding a newline */
}


struct my_error_mgr 
{
	struct jpeg_error_mgr pub;	// "public" fields 
	jmp_buf setjmp_buffer;		// for return to caller 
};

typedef struct my_error_mgr * my_error_ptr;



void my_error_exit (j_common_ptr cinfo) 
{
	// cinfo->err really points to a my_error_mgr struct, so coerce pointer 
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
  
	// Always display the message. 
	// We could postpone this until after returning, if we chose. 
	(*cinfo->err->output_message)(cinfo);

	// Return control to the setjmp point 
	longjmp(myerr->setjmp_buffer, 1);
}


// private error handler struct
struct DJDIJGErrorStruct
{
  // the standard IJG error handler object
  struct jpeg_error_mgr pub;

  // our jump buffer
  jmp_buf setjmp_buffer;
};



// private error handler struct
struct DJEIJGErrorStruct
{
  // the standard IJG error handler object
  struct jpeg_error_mgr pub;

  // our jump buffer
  jmp_buf setjmp_buffer;
};


// private source manager struct
struct DJDIJGSourceManagerStruct
{
  // the standard IJG source manager object
  struct jpeg_source_mgr pub;

  // number of bytes to skip at start of buffer
  long skip_bytes;

  // buffer from which reading will continue as soon as the current buffer is empty
  unsigned char *next_buffer;

  // buffer size
  unsigned int  next_buffer_size;
};

struct wiol_destination_mgr
{
	struct jpeg_destination_mgr pub; /* public fields */
	unsigned char *jpegdata;
	JOCTET *buffer;		/* start of buffer */
	unsigned long jpeglen;
	bool start_of_file;	/* have we gotten any data yet? */
};

typedef wiol_destination_mgr *wiol_dest_ptr;



void wiol_init_destination(j_compress_ptr cinfo);
BOOL wiol_empty_output_buffer(j_compress_ptr cinfo);
void wiol_term_destination(j_compress_ptr cinfo);




// callback forward declarations
void DJDIJGErrorExit(j_common_ptr);
void DJDIJGOutputMessage(j_common_ptr cinfo);
void DJDIJGinitSource(j_decompress_ptr);
ijg_boolean DJDIJGfillInputBuffer(j_decompress_ptr);
void DJDIJGskipInputData(j_decompress_ptr, long);
void DJDIJGtermSource(j_decompress_ptr);

}


/*

static void wiol_init_source(j_decompress_ptr cinfo) 
{
	wiol_src_ptr src = (wiol_src_ptr) cinfo->src;
  
	// We reset the empty-input-file flag for each image, but we don't clear
	// the input buffer.   This is correct behavior for reading a series of
	// images from one source.
	//
	src->start_of_file = true;
	src->jpeglen = 0;
}

*/



// error handler, executes longjmp
void DJDIJGErrorExit(j_common_ptr cinfo)
{
  DJDIJGErrorStruct *myerr = (DJDIJGErrorStruct *)cinfo->err;
  longjmp(myerr->setjmp_buffer, 1);
}

// message handler for warning messages and the like
void DJDIJGOutputMessage(j_common_ptr cinfo)
{
  DJDIJGErrorStruct *myerr = (DJDIJGErrorStruct *)cinfo->err;
}


// methods for decompress-source-manager

void DJDIJGinitSource(j_decompress_ptr cinfo )
{

}



ijg_boolean DJDIJGfillInputBuffer(j_decompress_ptr cinfo)
{
  DJDIJGSourceManagerStruct *src = (DJDIJGSourceManagerStruct *)(cinfo->src);

  // if we already have the next buffer, switch buffers
  if (src->next_buffer)
  {
    src->pub.next_input_byte    = src->next_buffer;
    src->pub.bytes_in_buffer    = (unsigned int) src->next_buffer_size;
    src->next_buffer            = NULL;
    src->next_buffer_size       = 0;

    // The suspension was caused by DJDIJG8skipInputData iff src->skip_bytes > 0.
    // In this case we must skip the remaining number of bytes here.
    if (src->skip_bytes > 0)
    {
      if (src->pub.bytes_in_buffer < (unsigned long) src->skip_bytes)
      {
        src->skip_bytes            -= src->pub.bytes_in_buffer;
        src->pub.next_input_byte   += src->pub.bytes_in_buffer;
        src->pub.bytes_in_buffer    = 0;
        // cause a suspension return
        return FALSE;
      }
      else
      {
        src->pub.bytes_in_buffer   -= (unsigned int) src->skip_bytes;
        src->pub.next_input_byte   += src->skip_bytes;
        src->skip_bytes             = 0;
      }
    }
    return TRUE;
  }

  // otherwise cause a suspension return
  return FALSE;
}

void DJDIJGskipInputData(
  j_decompress_ptr cinfo,
  long num_bytes)
{
  DJDIJGSourceManagerStruct *src = (DJDIJGSourceManagerStruct *)(cinfo->src);

  if (src->pub.bytes_in_buffer < (size_t)num_bytes)
  {
    src->skip_bytes             = num_bytes - src->pub.bytes_in_buffer;
    src->pub.next_input_byte   += src->pub.bytes_in_buffer;
    src->pub.bytes_in_buffer    = 0; // causes a suspension return
  }
  else
  {
    src->pub.bytes_in_buffer   -= (unsigned int) num_bytes;
    src->pub.next_input_byte   += num_bytes;
    src->skip_bytes             = 0;
  }
}


void DJDIJGtermSource(j_decompress_ptr  cinfo)
{

}

static void wiol_init_destination (j_compress_ptr cinfo) 
{
	wiol_dest_ptr dest = (wiol_dest_ptr) cinfo->dest;
  
	// We reset the empty-input-file flag for each image, but we don't clear
	//the input buffer.   This is correct behavior for reading a series of
	//images from one source.
	//

	dest->start_of_file = true;
	dest->jpeglen = 0;
	
}



BOOL wiol_empty_output_buffer(j_compress_ptr cinfo) 
{
	wiol_dest_ptr dest = (wiol_dest_ptr) cinfo->dest;

	memcpy(dest->jpegdata + dest->jpeglen, dest->buffer,IJGBLOCKSIZE);

	dest->jpeglen += IJGBLOCKSIZE;
	dest->pub.free_in_buffer = IJGBLOCKSIZE;
	dest->pub.next_output_byte = dest->buffer;
	
	return TRUE;
}

static void wiol_term_destination (j_compress_ptr cinfo) 
{
	wiol_dest_ptr dest = (wiol_dest_ptr) cinfo->dest;
	int nbytes;
	
	
	// yes, this needs to flush the buffer 
	// needs error handling */

	if (dest != NULL)
	{
		nbytes = IJGBLOCKSIZE - dest->pub.free_in_buffer;
		
		if (dest->jpegdata != NULL && nbytes > 0)
		{
			memcpy(dest->jpegdata + dest->jpeglen,dest->buffer,nbytes);
			dest->jpeglen += nbytes;
		}

		if (dest->buffer != NULL)
			delete [] dest->buffer;
	}

	return;
}



BOOL ReadJpegParams(void *pJpegData,int nJpegLen,int &nWidth,int &nHeight,int &nBPP,int &nColorMode)
{
	jpeg_decompress_struct cinfo;
	DJDIJGErrorStruct jerr;
	DJDIJGSourceManagerStruct src;

    // Specify the source of the compressed data
    src.pub.init_source       = DJDIJGinitSource;
    src.pub.fill_input_buffer = DJDIJGfillInputBuffer;
    src.pub.skip_input_data   = DJDIJGskipInputData;
    src.pub.resync_to_restart = jpeg_resync_to_restart;
    src.pub.term_source       = DJDIJGtermSource;
    src.pub.bytes_in_buffer   = 0;
    src.pub.next_input_byte   = NULL;
    src.skip_bytes             = 0;
    src.next_buffer            = NULL;
    src.next_buffer_size       = 0;

	cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = DJDIJGErrorExit;
    jerr.pub.output_message = DJDIJGOutputMessage;
    
	if (setjmp(jerr.setjmp_buffer))
    {
        // the IJG error handler will cause the following code to be executed
        char buffer[JMSG_LENGTH_MAX];
        (cinfo.err->format_message)((jpeg_common_struct *)&cinfo, buffer); 

	    jpeg_destroy_decompress(&cinfo);

///		delete (DJDIJGErrorStruct *)(&cinfo.err);
///		delete (DJDIJGSourceManagerStruct *)(&cinfo.src);
		return FALSE;
    }

    jpeg_create_decompress(&cinfo);

    cinfo.src = &src.pub;

	src.next_buffer            = (unsigned char *) pJpegData;
	src.next_buffer_size       = nJpegLen;
	
	jpeg_read_header(&cinfo, FALSE);

	nWidth = cinfo.image_width ;
	nHeight = cinfo.image_height;
	nBPP = cinfo.num_components;

	switch(cinfo.jpeg_color_space)
	{
	case JCS_GRAYSCALE:
		nColorMode = 1;
		break;
	case JCS_YCbCr: 
		nColorMode = 2;
		break;
	case JCS_RGB:
		nColorMode = 3;
	default:
		nColorMode = 3;
		break;
	}

	jpeg_destroy_decompress(&cinfo);

///	delete (DJDIJGErrorStruct *)(&cinfo.err);
///	delete (DJDIJGSourceManagerStruct *)(&cinfo.src);	

	return TRUE;
}


BOOL JpegIJGDeompress(void *pJpegData,int nJpegLen,int &nImageWidth,int &nImageHeight,unsigned char **lpImageData)
{
	jpeg_decompress_struct cinfo;
	DJDIJGErrorStruct jerr;
	DJDIJGSourceManagerStruct src;
	int nBytesWide,nUnused, q, nRow, nPixel;

	nImageWidth = 0;
	nImageHeight = 0;

    // Specify the source of the compressed data
    src.pub.init_source       = DJDIJGinitSource;
    src.pub.fill_input_buffer = DJDIJGfillInputBuffer;
    src.pub.skip_input_data   = DJDIJGskipInputData;
    src.pub.resync_to_restart = jpeg_resync_to_restart;
    src.pub.term_source       = DJDIJGtermSource;
    src.pub.bytes_in_buffer   = 0;
    src.pub.next_input_byte   = NULL;
    src.skip_bytes             = 0;
    src.next_buffer            = NULL;
    src.next_buffer_size       = 0;

	cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = DJDIJGErrorExit;
    jerr.pub.output_message = DJDIJGOutputMessage;
    
	if (setjmp(jerr.setjmp_buffer))
    {
        // the IJG error handler will cause the following code to be executed
        char buffer[JMSG_LENGTH_MAX];
        (cinfo.err->format_message)((jpeg_common_struct *)&cinfo, buffer); 

	    jpeg_destroy_decompress(&cinfo);

		return FALSE;
    }


    jpeg_create_decompress(&cinfo);

    cinfo.src = &src.pub;

	src.next_buffer            = (unsigned char *) pJpegData;
	src.next_buffer_size       = nJpegLen;
	
	jpeg_read_header(&cinfo, TRUE);

	nImageWidth = cinfo.image_width ;
	nImageHeight = cinfo.image_height;
	
	// Êä³öRGB 24Î»
	cinfo.out_color_space = JCS_RGB ; // set the output color space to RGB

	// By calling jpeg_start_decompress, you populate cinfo
	// and can then allocate your output bitmap buffers for
	// each scanline.
	jpeg_start_decompress(&cinfo);


	nBytesWide = (nImageWidth * 3);
	nUnused = (((nBytesWide + 3) / 4) * 4) - nBytesWide;
	nBytesWide += nUnused;

	if (*lpImageData != NULL) {
		free(*lpImageData);
		*lpImageData = NULL;
	}

	*lpImageData = (unsigned char *) malloc( nBytesWide * nImageHeight);
	
	//Allocate single row of JSAMPs
	JSAMPARRAY jsmpPixels = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo,
		JPOOL_IMAGE, 
		cinfo.image_width * cinfo.output_components, 1);

	LPBYTE lpPixels = (LPBYTE) *lpImageData;

	while (cinfo.output_scanline < cinfo.output_height) {

		jpeg_read_scanlines(&cinfo, jsmpPixels, 1);
		
		for (q= 0; q < (nBytesWide - nUnused); q += 3) {	 
			//nRow = (nImageHeight - cinfo.output_scanline) * nBytesWide;
			nRow = (cinfo.output_scanline - 1) * nBytesWide;

			nPixel = nRow + q;
			lpPixels[nPixel+2] = jsmpPixels[0][q+0]; //Red
			lpPixels[nPixel+1] = jsmpPixels[0][q+1]; //Green
			lpPixels[nPixel+0] = jsmpPixels[0][q+2]; //Blue
		}
	}
	
	// Once done reading *all* scanlines, release all internal buffers,
	// etc by calling jpeg_finish_decompress. This lets you go back and
	// reuse the same cinfo object with the same settings, if you
	// want to decompress several jpegs in a row.
	//
	// If you didn't read all the scanlines, but want to stop early,
	// you instead need to call jpeg_abort_decompress(&cinfo)
	jpeg_finish_decompress(&cinfo);
 
	// At this point, optionally go back and either load a new jpg into
	// the jpg_buffer, or define a new jpeg_mem_src, and then start 
	// another decompress operation.
	
	// Once you're really really done, destroy the object to free everything
	jpeg_destroy_decompress(&cinfo);
	
	return TRUE;
}


BOOL JpegIJGCompress(unsigned char *pDataBuf,int nImageWidth,int nImageHeight,int nSamplesPerPixel,unsigned char *pJpegData,unsigned long &nJpegLen)
{
	int quality;
	jpeg_compress_struct cinfo;
	my_error_mgr jerr;
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int ni,row_stride;		/* physical row width in image buffer */
	wiol_destination_mgr mgr;
	wiol_dest_ptr dest;
	JSAMPARRAY buffer;		/* Output row buffer */
	unsigned char temp;

	nJpegLen = 0;

	if (pDataBuf == NULL || pJpegData == NULL)
	{
		return FALSE;
	}
	
	if (!(nSamplesPerPixel == 1 || nSamplesPerPixel == 3))
	{
		return FALSE;
	}

	quality = 95;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	jerr.pub.output_message = my_output_message;
  
	if (setjmp(jerr.setjmp_buffer)) 
	{
		jpeg_destroy_compress(&cinfo);
	    return FALSE;
	}

	jpeg_create_compress(&cinfo);
	
	cinfo.image_width  = nImageWidth; 	
	cinfo.image_height = nImageHeight;

	if (nSamplesPerPixel == 3)
	{
		cinfo.input_components = 3;		// # of color components per pixel 
		cinfo.in_color_space = JCS_RGB; 	// colorspace of input image 
		cinfo.jpeg_color_space = JCS_RGB;//YCbCr;
	}
	else
	{
	    cinfo.input_components = 1;		// # of color components per pixel 
		cinfo.in_color_space = JCS_GRAYSCALE; 	// colorspace of input image 
	}

	jpeg_set_defaults(&cinfo);

	jpeg_set_quality(&cinfo, quality, TRUE);  // limit to baseline-JPEG values 

	cinfo.density_unit=1;

	cinfo.X_density=(unsigned short)0;//GetXDPI();
	cinfo.Y_density=(unsigned short)0;//GetYDPI();

	cinfo.dest = (struct jpeg_destination_mgr *) &mgr;

	dest = (wiol_dest_ptr) cinfo.dest;
	
	dest->jpegdata   = pJpegData;
	dest->buffer     = new unsigned char[IJGBLOCKSIZE];
	dest->jpeglen    = 0;

	dest->pub.init_destination	  = wiol_init_destination;
	dest->pub.empty_output_buffer = wiol_empty_output_buffer;
	dest->pub.term_destination	  = wiol_term_destination;
	dest->pub.free_in_buffer      = IJGBLOCKSIZE;
	dest->pub.next_output_byte    = dest->buffer;
			
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = nImageWidth * nSamplesPerPixel;	// JSAMPLEs per row in image_buffer 
	
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, 8 + row_stride, 1);

    while (cinfo.next_scanline < cinfo.image_height) 
	{
		// jpeg_write_scanlines expects an array of pointers to scanlines.
		// Here the array is only one element long, but you could pass
		// more than one scanline at a time if that's more convenient.

		row_pointer[0] = pDataBuf + (cinfo.next_scanline * row_stride);

		if ((cinfo.next_scanline % 2) == 0)
		{
			memcpy(buffer[0],row_pointer[0],row_stride);	
		}
			
		if (nSamplesPerPixel == 3)
		{
			for (ni = 0; ni < nImageWidth; ni ++)
			{
				temp = buffer[0][ni];
				//buffer[0][ni] = 0;//buffer[0][ni + 2];
				//buffer[0][ni + 2] = 0;//temp;
			}
		}

		jpeg_write_scanlines(&cinfo, buffer, 1);
	}

	/* Step 6: Finish compression */

	jpeg_finish_compress(&cinfo);

	nJpegLen = dest->jpeglen;

	jpeg_destroy_compress(&cinfo);

	return TRUE;	
}



