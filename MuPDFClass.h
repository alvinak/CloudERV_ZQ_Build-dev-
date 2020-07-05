#ifndef __MuPDFClass_h__
#define __MuPDFClass_h__

extern "C" {
#include "mupdf/fitz.h"
}

class MuPDFClass
{
private:
        fz_stream* _source;
        fz_document* _doc;
        fz_context* _ctx;
        fz_page* _page;

        bool _FromStream;
        int _CurrentPage;
        float _CurrentPageWidth;
        float _CurrentPageHeight;
        unsigned char* _BitmapBuffer;
        void CleanUp();

public:
        int _PageCount;
        MuPDFClass()
        {
                _source = NULL;
                _FromStream = false;
                _doc = NULL;
                _ctx = NULL;
                _page = NULL;
                _PageCount = -1;
                _CurrentPage = -1;
                _BitmapBuffer = NULL;
                
                fz_var(_doc);

                _ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
                if(!_ctx)
                {
                        //error here
                        fprintf(stderr, "cannot initialise context\n");
                        exit(1);
                }
        };
        ~MuPDFClass()
        {
                CleanUp();
        };      
        int LoadPdf(char* filename, char* password);
        int LoadPdfFromStream(unsigned char* buffer, int bufferSize, char* password);
        int LoadPdfInit(char* password);
        int LoadPage(int pageNumber);
        float GetWidth();
        float GetHeight();
        void SetAlphaBits(int alphaBits);
        unsigned char* GetBitmap(int & width, int & height, float dpiX, float dpiY, int rotation, int colorspace, bool rotateLandscapePages, bool convertToLetter, int & pnLength, int maxSize);
        void FreeRenderedPage();
};

#endif  // __MuPDFClass_h__
