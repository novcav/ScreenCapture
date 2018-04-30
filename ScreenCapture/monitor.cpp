//
// display captured image
//

#include "stdafx.h"
#include "monitor.h"

// construction
CMonitor::CMonitor()
{
	hDialog = NULL;
}

// destruction
CMonitor::~CMonitor()
{
}

// initalize dialog
void CMonitor::OnInitDialog(HWND hDlg)
{
	HWND hButton1, hButton2;
	int x, y;
	RECT rect;

	// position the dialog to center
	GetWindowRect(hDlg, &rect);

	x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
	y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;

	SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

	// get button handle
	hButton1 = GetDlgItem(hDlg, IDC_BUTTON1);
	hButton2 = GetDlgItem(hDlg, IDC_BUTTON2);

	// set play button bitmap
	HBITMAP hBitmap1 = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	SendMessage(hButton1, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hBitmap1);

	// set stop button bitmap
	HBITMAP hBitmap2 = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	SendMessage(hButton2, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hBitmap2);

	// enable play button, disable stop button
	EnableWindow(hButton1, TRUE);
	EnableWindow(hButton2, FALSE);

	// update dialog window
	BYTE* buffer = NULL;
	PostMessage(hDlg, WM_DRAW, 0, (LPARAM) buffer);
}

// start capture
void CMonitor::OnPlay(HWND hDlg)
{
	HWND hWnd, hButton1, hButton2;

	// get button handle
	hButton1 = GetDlgItem(hDlg, IDC_BUTTON1);
	hButton2 = GetDlgItem(hDlg, IDC_BUTTON2);

	// enable play button, disable stop button
	EnableWindow(hButton1, FALSE);
	EnableWindow(hButton2, TRUE);

	// send message to parent window to start capture
	hWnd = GetParent(hDlg);
	SendMessage(hWnd, WM_PLAY, 0, 0);
}

// stop capture
void CMonitor::OnStop(HWND hDlg)
{
	HWND hWnd, hButton1, hButton2;

	// get button handle
	hButton1 = GetDlgItem(hDlg, IDC_BUTTON1);
	hButton2 = GetDlgItem(hDlg, IDC_BUTTON2);

	// enable stop button, disable play button
	EnableWindow(hButton1, TRUE);
	EnableWindow(hButton2, FALSE);

	// send message to parent window to stop capture
	hWnd = GetParent(hDlg);
	SendMessage(hWnd, WM_STOP, 0, 0);
}

// exit program
void CMonitor::OnClose(HWND hDlg)
{
	// send message to parent window to stop thread and terminate program
	HWND hWnd = GetParent(hDlg);
	PostMessage(hWnd, WM_STOP, 0, 0);
	PostMessage(hWnd, WM_COMMAND, IDM_EXIT, 0);
}

// draw image capture
void CMonitor::OnDraw(HWND hDlg, LPARAM lParam)
{
	BYTE* buffer = (BYTE*) lParam;

	HDC hDC, hDC1, hDC2;
	HBITMAP hBitmap1, hBitmap2;

	hDC = GetDC(hDlg);

	// create memory device context of size WIDTH and HEIGHT
	hDC1 = CreateCompatibleDC(hDC);
	hBitmap1 = CreateCompatibleBitmap(hDC, WIDTH, HEIGHT);
	SelectObject(hDC1, hBitmap1);

	// create second memory device context of size width and height
	// set the pixel in a bitmap and scale it to fit in first device context
	hDC2 = CreateCompatibleDC(hDC);
	hBitmap2 = CreateCompatibleBitmap(hDC, width, height);
	SetDIBits(hDC, hBitmap2, 0, (UINT)height, buffer, (BITMAPINFO *)&bih, DIB_RGB_COLORS);
	SelectObject(hDC2, hBitmap2);
	SetStretchBltMode(hDC2, HALFTONE);
	StretchBlt(hDC1, x, y, scaledwidth, scaledheight, hDC2, 0, 0,  width,  height, SRCCOPY);

	// copy the bitmap from first memory device context to display device context
	BitBlt(hDC, 0, 0, WIDTH, HEIGHT, hDC1, 0, 0, SRCCOPY);

	// clean up
	DeleteObject(hBitmap1);
	DeleteObject(hBitmap2);
	DeleteObject(hDC1);
	DeleteObject(hDC2);
	ReleaseDC(hDlg, hDC); 
}

// display dialog
void CMonitor::Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pDlgProc)
{
	// if window already, don't show dialog box
	if (IsWindow(hDialog)) return;

	// copy to lcal variable
	this->hInstance = hInstance;

	// Creates a modeless dialog box from a dialog box template IDD_DIALOG1
	hDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, pDlgProc);

	// show dialog box
	ShowWindow(hDialog, SW_SHOW);
}

// dialog procedure
INT_PTR CMonitor::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		switch (LOWORD(wParam)) 
		{
		case IDC_BUTTON1:
			OnPlay(hDlg);
 			return (INT_PTR)TRUE;

		case IDC_BUTTON2:
			OnStop(hDlg);
			return (INT_PTR)TRUE;
		}

	case WM_DRAW:
		OnDraw(hDlg, lParam);
		return (INT_PTR)TRUE;

	case WM_CLOSE:
		OnClose(hDlg);
		return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

// fit the image into a dialog client area
void CMonitor::SetSize(DWORD width, DWORD height)
{
	this->width  = width;
	this->height = height;

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

	scaledheight = HEIGHT;
	scaledwidth  = DWORD((float)scaledheight * ((float)width / (float)height));

	if(scaledwidth > WIDTH)
	{
		scaledwidth  = WIDTH;
		scaledheight = DWORD((float)scaledwidth * ((float)height / (float)width));
	}

	x = (WIDTH  - scaledwidth)  / 2;
	y = (HEIGHT - scaledheight) / 2;
}

// update display
void CMonitor::Invalidate(BYTE* buffer)
{
	SendMessage(hDialog, WM_DRAW, 0, (LPARAM) buffer);
}
