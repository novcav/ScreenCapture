//
// display captured image
//

#pragma once

#include "resource.h"

#define WIDTH  600
#define HEIGHT 356

class CMonitor
{
private:
	HWND hDialog;
	HINSTANCE hInstance;
	int width, height, x, y, scaledwidth, scaledheight;
	BITMAPINFOHEADER bih;

private:
	void OnInitDialog(HWND hDlg);
	void OnPlay(HWND hDlg);
	void OnStop(HWND hDlg);
	void OnClose(HWND hDlg);
	void OnDraw(HWND hDlg, LPARAM lParam);

public:
	CMonitor();
	~CMonitor();

	void Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pDlgProc);
	INT_PTR DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	void SetSize(DWORD width, DWORD height);
	void Invalidate(BYTE* buffer);
};

