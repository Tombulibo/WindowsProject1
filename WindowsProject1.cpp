// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WindowsProject1.h"

#include <commdlg.h>
#include "SimpleFile.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

vecLinesContainer fileLines; // 读取的文件内容
HWND hWndEdit; // 文本框句柄
WCHAR currentFile[260]; // 文件名

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

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



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   RECT rect;
   GetClientRect(hWnd, &rect);
   hWndEdit = CreateWindowW(
       L"EDIT", L"简单文本编辑器：理解Win32 API方式下的消息机制",
       WS_VISIBLE | WS_CHILD | ES_LEFT | ES_MULTILINE |
       ES_WANTRETURN | WS_VSCROLL | ES_AUTOVSCROLL,
       rect.left + 5, rect.top + 5,
       rect.right - 10, rect.bottom - 10,
       hWnd, nullptr, hInstance, nullptr);
   HFONT hFont = CreateFont(
       24, 0, 0, 0, 300, false, false, false,
       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
       CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
       DEFAULT_PITCH,
       L"微软雅黑"
   );
   SendMessage(hWndEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    OPENFILENAME ofn; // OpenFile通用结构体
    WCHAR szFile[260] = { 0 }; // 文件名
    int cTxtLen; // 文本长度
    WCHAR* allFileLines; // 文本内容

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_OPEN :
                //初始化OPENFILENAME结构体
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.lpstrFile = currentFile;
                ofn.nMaxFile = sizeof(currentFile);
                ofn.lpstrFilter =_T("All\0*.*\0Text\0*.TXT\0");
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                
                if (GetOpenFileName(&ofn) == TRUE)
                {
                    SetWindowText(hWnd, ofn.lpstrFile);
                    SimpleFile simplefile;
                    fileLines = simplefile.getFileLines(ofn.lpstrFile);

                    vecLinesContainer::iterator it;
                    std::wstring allLines = L"";
                    for (it = fileLines.begin(); it != fileLines.end(); it++)
                    {
                        allLines.append(*it);
                        allLines.append(L"\r\n");
                    }

                    SetWindowText(hWndEdit, allLines.c_str());

                }       
                break;
            case IDM_SAVE:
                SetWindowText(hWnd, currentFile);
                cTxtLen = GetWindowTextLength(hWndEdit);
                allFileLines =
                    (LPWSTR)VirtualAlloc((LPVOID)NULL,
                                         (DWORD)(cTxtLen + 1),
                                         MEM_COMMIT,
                                         PAGE_READWRITE);
                GetWindowText(hWndEdit, allFileLines, cTxtLen + 1);
                if (allFileLines != NULL)
                {
                    SimpleFile simplefile;
                    simplefile.WriteFile(currentFile, allFileLines);
                }
                break;
            case IDM_SAVEAS :
                //初始化OPENFILENAME结构体
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = _T("All\0*.*\0Text\0*.TXT\0");
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                if (GetSaveFileName(&ofn) == TRUE)
                {
                    SetWindowText(hWnd, ofn.lpstrFile);
                    cTxtLen = GetWindowTextLength(hWndEdit);
                    allFileLines =
                        (LPWSTR)VirtualAlloc((LPVOID)NULL,
                                             (DWORD)(cTxtLen + 1),
                                             MEM_COMMIT,
                                             PAGE_READWRITE);
                    GetWindowText(hWndEdit, allFileLines, cTxtLen + 1);
                    if (allFileLines != NULL)
                    {
                        SimpleFile simplefile;
                        simplefile.WriteFile(ofn.lpstrFile, allFileLines);
                    }
                }
                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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
