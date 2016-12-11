// P2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "P2.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
POINT mouse;
bool pixels = false;
bool rectangle = false;
int X1 = 0;
int X2 = 0;
int Y1 = 0;
int Y2 = 0;
RECT Rec;
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void drawPixels(HWND hWnd, WPARAM wParam, LPARAM lParam);
void ClipBitmap(HWND hWnd, UINT nFormat);
void OnRenderAllFormats(HWND hWnd);
void OnEditCopy(HWND hWnd);
void OnMouseMoveRectangle(HWND hWnd, WPARAM wParam, LPARAM lParam, int x1, int y1, int &x2, int &y2, DWORD rgb, bool enable);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_P2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_P2));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_P2));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_P2);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc, dc, hdcScreen;
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	int i;
	HBITMAP hbmp;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_TOOL_DRAWPIXELS:
			pixels = true;
			break;
		case ID_TOOL_CLIP:
			pixels = false;
			OnEditCopy(hWnd);
			break;
		case ID_TOOL_SELECTAREA:
			pixels = false;
			rectangle = true;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		X2 = mouse.x = LOWORD(lParam);
		Y2 = mouse.y = HIWORD(lParam);
		break; 
	case WM_MOUSEMOVE:
		drawPixels(hWnd, wParam, lParam);
		OnMouseMoveRectangle(hWnd, wParam, lParam, mouse.x, mouse.y, X2, Y2, RGB(0, 0, 0), rectangle);
		break;
	case WM_RENDERFORMAT:
		ClipBitmap(hWnd, (UINT)wParam);
		break;
	case WM_RENDERALLFORMATS:
		OnRenderAllFormats(hWnd);
		break;
	case WM_DESTROYCLIPBOARD:

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void drawPixels(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (wParam & MK_LBUTTON == MK_LBUTTON && pixels)
	{
		HDC dc = GetDC(hWnd);
		SetDCPenColor(dc, RGB(255, 0, 0));
		MoveToEx(dc, mouse.x, mouse.y, NULL);
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
		LineTo(dc, mouse.x, mouse.y);
		//SetPixel(dc, mouse.x, mouse.y, RGB(255, 0, 0));
		ReleaseDC(hWnd, dc);
	}
}

void drawRectangle(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (wParam & MK_LBUTTON == MK_LBUTTON)
	{
		HDC dc = GetDC(hWnd);
		SetDCPenColor(dc, RGB(255, 0, 0));
		MoveToEx(dc, mouse.x, mouse.y, NULL);
		LineTo(dc, mouse.x, mouse.y);
		//SetPixel(dc, mouse.x, mouse.y, RGB(255, 0, 0));
		ReleaseDC(hWnd, dc);
	}
}

void ClipBitmap(HWND hWnd, UINT nFormat)
{
	if (nFormat == CF_BITMAP)
	{
		HDC hdcWindow, hdcScreen;
		HDC hdcMemDC = NULL;
		HBITMAP hbmScreen = NULL;
		BITMAP bmpScreen;
		hdcScreen = GetDC(NULL);
		hdcWindow = GetDC(hWnd);

		hdcMemDC = CreateCompatibleDC(hdcWindow);

		RECT rcClient;
		GetClientRect(hWnd, &rcClient);

		hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
		SelectObject(hdcMemDC, hbmScreen);
		BitBlt(hdcMemDC,
			mouse.x, mouse.y,
			X2 - mouse.x, Y2 - mouse.y,
			hdcWindow,
			0, 0,
			SRCCOPY);
		//OpenClipboard(hWnd);
		//EmptyClipboard();
		SetClipboardData(CF_BITMAP, hbmScreen);
		CloseClipboard();
	}
}

// Chép handle NULL vào Clipboard khi user Copy
void OnEditCopy(HWND hWnd)
{
	OpenClipboard(hWnd);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, NULL);
	CloseClipboard();
}

// X? lý WM_RENDERALLFORMATS
void OnRenderAllFormats(HWND hWnd)
{
	OpenClipboard(hWnd);
	EmptyClipboard();
	ClipBitmap(hWnd, CF_BITMAP);
	CloseClipboard();
}

void OnMouseMoveRectangle(HWND hWnd, WPARAM wParam, LPARAM lParam, int x1, int y1, int &x2, int &y2, DWORD rgb, bool enable)
{
	if (!enable) return;
	if (!(wParam & MK_LBUTTON)) return;
	HDC dc = GetDC(hWnd);
	SelectObject(dc, GetStockObject(DC_PEN));
	SetDCPenColor(dc, rgb);
	// xóa đường cũ
	SetROP2(dc, R2_NOTXORPEN);

	MoveToEx(dc, x1, y1, NULL);
	Rectangle(dc, x1, y1, x2, y2);
	// vẽ đường mới
	x2 = LOWORD(lParam);
	y2 = HIWORD(lParam);

	MoveToEx(dc, x1, y1, NULL);
	Rectangle(dc, x1, y1, x2, y2);
	ReleaseDC(hWnd, dc);
}