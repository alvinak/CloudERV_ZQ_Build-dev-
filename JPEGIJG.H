#if !defined(__JPEGIJG_H__)
#define __JPEGIJG_H__

BOOL ReadJpegParams(void *pJpegData,int nJpegLen,int &nWidth,int &nHeight,int &nBPP,int &nColorMode);
BOOL JpegIJGCompress(unsigned char *pDataBuf,int nImageWidth,int nImageHeight,int nSamplesPerPixel,unsigned char *pJpegData,unsigned long &nJpegLen);
BOOL JpegIJGDeompress(void *pJpegData,int nJpegLen,int &nImageWidth,int &nImageHeight,unsigned char **lpImageData);


#endif