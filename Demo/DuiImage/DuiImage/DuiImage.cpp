// DuiImage.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DuiImage.h"
#include "MainUI.h"

CMainUI theMainWindow;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DUIIMAGE));

    MSG msg;
    // 主消息循环: 
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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   WCHAR buf[MAX_PATH] = { 0 };
   ::GetModuleFileName(NULL, buf, MAX_PATH);
   std::wstring path = buf;
   path = path.substr(0, path.find_last_of(L"\\"));
   path += L"\\skin";
   CPaintManagerUI::SetResourcePath(path.c_str());

   theMainWindow.Create(NULL, L"DuiImageUI", WS_POPUP , 0);
   HWND hWnd = theMainWindow.GetHWND();
   if (!hWnd) {
      return FALSE;
   }

   int screen_height = GetSystemMetrics(SM_CYSCREEN);
   int screen_width = GetSystemMetrics(SM_CXSCREEN);
   RECT rect = { 0,0,0,0 };
   ::GetWindowRect(hWnd, &rect);
   int main_height = rect.bottom - rect.top;
   int main_width = rect.right - rect.left;
   int x = (screen_width - main_width) / 2;
   int y = (screen_height - main_height) / 2;
   ::SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
