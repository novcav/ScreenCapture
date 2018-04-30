//
// capture image
//

#pragma once

class CCaptureImage
{
private:
	HANDLE hHeap;
	BITMAPINFOHEADER bih;
	int x, y, width, height;
	BYTE* buffer;
	DWORD buffersize;

private:
	void ComputeBuffersize(DWORD *buffersize, WORD bitcount, int width, int height);

public:
	CCaptureImage();
	~CCaptureImage();

	void Create(int x, int y, int width, int height);
	void Resize(int x, int y, int width, int height);
	void Capture(BYTE **image);
	void Destroy();
};

