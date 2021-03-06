//
// Capture screen and display it in child window
//

#include "stdafx.h"
#include "ScreenCapture.h"
#include "selectionrectangle.h"
#include "captureimage.h"
#include "monitor.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//
HANDLE hThread;
HANDLE hStopEvent;
DWORD dwThreadId;

//
CSelectionRectangle sel;
CCaptureImage cap;
CMonitor mon;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI		ThreadFunc(LPVOID lpParam);
INT_PTR CALLBACK	DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void OnPlay(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnStop(HWND hWnd, WPARAM wParam, LPARAM lParam);

void OnLButtonDown(HWND hWnd, int x, int y);
void OnLButtonUp(HWND hWnd, int x, int y);
void OnMouseMove(HWND hWnd, int x, int y);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);

void OnExit(HWND hWnd);

// main window
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SCREENCAPTURE, szWindowClass, MAX_LOADSTRING);

    //  Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCREENCAPTURE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    // Store instance handle in our global variable.
    hInst = hInstance; 

    // Create the main program window.
    HWND hWnd = CreateWindowEx(WS_EX_LAYERED, szWindowClass, szTitle, WS_POPUP,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

	// Make the window transparent
    ShowWindow(hWnd, SW_MAXIMIZE);
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    SetLayeredWindowAttributes(hWnd, RGB(255,255,255), 255, LWA_COLORKEY);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCREENCAPTURE));

    MSG msg;

    // Main message loop
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//  Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_PLAY: OnPlay(hWnd, wParam, lParam);					break;
	case WM_STOP: OnStop(hWnd, wParam, lParam);					break;

    case WM_LBUTTONDOWN:   OnLButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam));  break;
    case WM_LBUTTONUP:     OnLButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam));    break;
    case WM_MOUSEMOVE:     OnMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam));    break;

    case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_EXIT:  OnExit(hWnd);						break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
        break;
	case WM_PAINT:   OnPaint(hWnd);									break;
	case WM_CREATE:  OnCreate(hWnd);								break;
	case WM_DESTROY: OnDestroy(hWnd);								break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//
DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
	wchar_t szText[100];
	DWORD dwWaitResult, dwTimeOut;
	BYTE* buffer = NULL;

	// display thread status to output window
	swprintf_s(szText, 100, L"THE THREAD 0x%x HAS STARTED.\n", dwThreadId);
	OutputDebugString(szText);

	// set size of image to capture
	mon.SetSize(sel.width, sel.height);

	dwTimeOut = 40;
	while (true)
	{
		// capture and display
		cap.Capture(&buffer);
		mon.Invalidate(buffer);

		// exit loop if event handle is set
		dwWaitResult = WaitForSingleObject(hStopEvent, dwTimeOut);
		if (dwWaitResult == WAIT_OBJECT_0) break;
	}

	// display thread status to output window
	swprintf_s(szText, 100, L"THE THREAD 0x%x HAS STOPED.\n", dwThreadId);
	OutputDebugString(szText);

	return 0;
}

// Message handler for dialog box.
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return mon.DlgProc(hDlg, message, wParam, lParam);
}

// start capture
void OnPlay(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// reset event handle to prevent stopping the loop
	ResetEvent(hStopEvent);

	// close thread handle
	if(hThread != NULL) CloseHandle(hThread);

	// create thread
	hThread = CreateThread(NULL,         // no security attributes 
						   0,            // use default stack size 
						   ThreadFunc,   // thread function 
						   NULL,         // argument to thread function 
						   0,            // use default creation flags 
						   &dwThreadId); // returns the thread identifier 

	// error checking display in output window
   if (hThread == NULL) 
      OutputDebugString(L"Create thread failed.\n" ); 
}

// stop capture
void OnStop(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SetEvent(hStopEvent);
}

//
void OnLButtonDown(HWND hWnd, int x, int y)
{
	sel.OnLButtonDown(hWnd, x, y);
}

//
void OnLButtonUp(HWND hWnd, int x, int y)
{
	sel.OnLButtonUp(hWnd, x, y);
}

// select rectangular area on the screen to be captured
void OnMouseMove(HWND hWnd, int x, int y)
{
	if (sel.OnMouseMove(hWnd, x, y))
	{
		cap.Resize(sel.x, sel.y, sel.width, sel.height);
		mon.SetSize(sel.width, sel.height);
	}
}

// draw selection rectangle
void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC;

	hDC = BeginPaint(hWnd, &ps);
	sel.Draw(hDC);
	EndPaint(hWnd, &ps);
}

//
void OnCreate(HWND hWnd)
{
	// initialize handle
	hThread    = NULL;
	hStopEvent = NULL;

	// create event
	hStopEvent = CreateEvent(NULL,  // no security attributes
		TRUE,                       // manual-reset event(TRUE-manual FALSE-auto)
		FALSE,                      // initial state is non-signaled (TRUE-signal FALSE-nonsignal)
		L"screencapturestopevent"); // object name

	// error checking, display in output window
    if (hStopEvent == NULL) 
        OutputDebugString(L"Create event failed.\n");
	
	// set rectangular area on the screen to be captured
	cap.Create(sel.x, sel.y, sel.width, sel.height);

	// show dialog box
	mon.Show(hWnd, hInst, DlgProc);
}

//
void OnDestroy(HWND hWnd)
{
	// close all handles
	if(hThread != NULL)    CloseHandle(hThread);
	if(hStopEvent != NULL) CloseHandle(hStopEvent);

	//
	cap.Destroy();

	// terminate program
	PostQuitMessage(0);
}

//
void OnExit(HWND hWnd)
{
	DestroyWindow(hWnd);
}
