// AdminLauncher.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "AdminLauncher.h"

#include "shellapi.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

bool RunAdminProcess(CString sFilePath, CString sCmdLine);
CString GetFilePath();  //获得当前exe路径
CString GetFolderPath();  //获得当前文件夹路径

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ADMINLAUNCHER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ADMINLAUNCHER));

	CString cmdLine( lpCmdLine );
	//CString path = GetFolderPath() + L"SpiritClinet.exe";
	if ( cmdLine.Find( L"write" ) != -1 )
	{
		RunAdminProcess( L"RegWorker.exe" , L"write" );
	}
	else if ( cmdLine.Find( L"delete" ) != -1 )
	{
		RunAdminProcess( L"RegWorker.exe" , L"delete" );
	}
	else
	{
		MessageBox( NULL, L"请传递参数！", L"错误", MB_OK );
	}

	PostQuitMessage(0);

	// 主消息循环:
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

bool RunAdminProcess(CString sFilePath, CString sCmdLine)
{
	OSVERSIONINFOEX OSVerInfo;
	OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if( !GetVersionEx( (OSVERSIONINFO *)&OSVerInfo ) )
	{
			OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx((OSVERSIONINFO *)&OSVerInfo);
	}

	TCHAR atszVerb[16];
	if(OSVerInfo.dwMajorVersion >= 6) // Vista 以上
	{
			_tcscpy_s(atszVerb,_T("runas"));
	}
	else
	{
			_tcscpy_s(atszVerb,_T(""));
	}

	// Launch itself as administrator.
	SHELLEXECUTEINFO sei;
	memset( &sei, 0, sizeof(SHELLEXECUTEINFO) );
	sei.cbSize = sizeof(SHELLEXECUTEINFO); 
	sei.lpVerb = atszVerb;
	//sei.lpFile = szPath;
	sei.lpFile = (LPCTSTR)sFilePath;
	sei.lpParameters = (LPCTSTR)sCmdLine;
	//sei.hwnd = hWnd;
	sei.nShow = SW_SHOWNORMAL;
		
	if ( !ShellExecuteEx(&sei) )
	{
		DWORD dwStatus = GetLastError();
		if (dwStatus == ERROR_CANCELLED)
		{
			// The user refused to allow privileges elevation.
			return false;
		}
		else
			if (dwStatus == ERROR_FILE_NOT_FOUND)
			{
				// The file defined by lpFile was not found and
				// an error message popped up.
				return FALSE;
			}
				
		return false;
	}    
		
	return true;
}

//获得当前文件的路径
CString GetFilePath()
{
	const int pathLen = 10240;
	TCHAR filePath[pathLen];
	GetModuleFileName( NULL, filePath, pathLen );

	CString path( filePath );

	return path;
}

//获得当前文件夹
CString GetFolderPath()
{
	CString path = GetFilePath();
	for (int i=path.GetLength()-1; i>=0; i--)
	{
		if ( path[i] == '\\' )
		{
			path = path.Left(i+1);
			break;
		}
	}

	return path;
}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ADMINLAUNCHER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 300, 200, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   //UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
