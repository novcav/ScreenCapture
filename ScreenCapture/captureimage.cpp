//
// capture image
//

#include "stdafx.h"
#include "captureimage.h"

// constructor
CCaptureImage::CCaptureImage()
{
	hHeap  = NULL;
	buffer = NULL;
}

// destructor
CCaptureImage::~CCaptureImage()
{
}

// compute buffer size
void CCaptureImage::ComputeBuffersize(DWORD *buffersize, WORD bitcount, int width, int height)
{
	int rem, widthbytes;

	// make width 4-byte aligned
	widthbytes = width * bitcount / 8;
	rem = widthbytes % 4;
	widthbytes += (rem > 0 ? (4 - rem) : 0);

	// compute buffer size needed for an image size (wudth, height)
	*buffersize = widthbytes * height;
}

// create a rectangular area on the screen to be captured
// this area is defined by x, y, width, height
void CCaptureImage::Create(int x, int y, int width, int height)
{
	wchar_t str[100];

	// copy to local variables
	this->x = x;
	this->y = y;
	this->width  = width;
	this->height = height;

	// Initialize the fields in the BITMAPINFO structure.
    bih.biSize          = sizeof(BITMAPINFOHEADER);
    bih.biWidth         = width;
    bih.biHeight        = height;
    bih.biPlanes        = 1;
    bih.biBitCount      = 24;
    bih.biCompression   = BI_RGB;
    bih.biSizeImage     = 0;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed       = 0;
    bih.biClrImportant  = 0;

	// compute the buffer size for captured image
	ComputeBuffersize(&buffersize, bih.biBitCount, width, height);

	// Retrieves a handle to the default heap of the calling process.
	hHeap = GetProcessHeap();
	if(hHeap == NULL)
	{
		swprintf_s(str, 100, L"GetProcessHeap Error Code: %d\n", GetLastError());
		OutputDebugString(str);
	}

	// Allocates a block of memory from a heap.
	buffer = (BYTE*)HeapAlloc(hHeap, 0, buffersize);
	if(buffer == NULL)
	{
		swprintf_s(str, 100, L"GetProcessHeap Error Code: %d\n", GetLastError());
		OutputDebugString(str);
	}
}

// resize a rectangular area on the screen to be captured
void CCaptureImage::Resize(int x, int y, int width, int height)
{
	if(hHeap == NULL) return;

	wchar_t str[100];

	this->x = x;
	this->y = y;
	this->width  = width;
	this->height = height;

    bih.biWidth  = width;
    bih.biHeight = height;

	// compute the buffer size for captured image
	ComputeBuffersize(&buffersize, bih.biBitCount, width, height);

	// Reallocates a block of memory from a heap.
	BYTE* temp;

	temp = (BYTE*)HeapReAlloc(hHeap, 0, buffer, buffersize);
	if(temp == NULL)
	{
		swprintf_s(str, 100, L"HeapReAlloc Error Code: %d\n", GetLastError());
		OutputDebugString(str);
		return;
	}

	buffer = temp;
}

// capture image
void CCaptureImage::Capture(BYTE **image)
{
	wchar_t str[100];
	HDC hDC1, hDC2;
	HBITMAP hBitmap;

	// retrieves the DC for the entire screen
	hDC1 = GetDC(NULL);

	// create memory device context of size width and height
	hDC2 = CreateCompatibleDC(hDC1);
	hBitmap = CreateCompatibleBitmap(hDC1, width, height);
	SelectObject(hDC2, hBitmap);

	// copy bitmap data from display device context (hDC1) to memory device context (hDC2)
	BitBlt(hDC2, 0, 0, width, height,  hDC1, x, y,  SRCCOPY);

	// lock memory
	if(!HeapLock(hHeap))
	{
		swprintf_s(str, 100, L"HeapLock Error Code: %d\n", GetLastError());
		OutputDebugString(str);
	}

	// copy bits into a buffer
 	GetDIBits(hDC1, hBitmap, 0, (UINT)height, buffer, (BITMAPINFO *)&bih, DIB_RGB_COLORS);

	// unlock memory
	if(!HeapUnlock(hHeap))
	{
		swprintf_s(str, 100, L"HeapUnlock Error Code: %d\n", GetLastError());
		OutputDebugString(str);
	}

	*image = buffer;

	// clean up
	DeleteObject(hBitmap);
	DeleteObject(hDC2);
	ReleaseDC(NULL, hDC1);
}

// clean up
void CCaptureImage::Destroy()
{
	wchar_t str[100];

	// Frees a memory block allocated from a heap
	if(!HeapFree(hHeap, 0, buffer))
	{
		swprintf_s(str, 100, L"HeapFree Error Code: %d\n", GetLastError());
		OutputDebugString(str);
	}
}