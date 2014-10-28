// RegWorker.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "RegWorker.h"

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

bool WriteAutostart();  //写入注册表开机自启
bool DeleteAutostart();  //删除开机自启
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
	LoadString(hInstance, IDC_REGWORKER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REGWORKER));

	CString cmdLine( lpCmdLine );
	if ( cmdLine.Find( L"write" ) != -1 )
	{
		WriteAutostart();
	}
	else if ( cmdLine.Find( L"delete" ) != -1 )
	{
		DeleteAutostart();
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

bool WriteAutostart()
{
	bool flag;

	HKEY hKEY;//定义有关的hKEY,在查询结束时要关闭
	
	//打开与路径 data_Set相关的hKEY
	CString tmp("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	
	//访问注册表，hKEY则保存此函数所打开的键的句柄
	long ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, tmp,0, KEY_READ|KEY_WRITE|KEY_ALL_ACCESS, &hKEY);
	
	if(ret!=ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
	{
		MessageBox( NULL, L"无法打开注册表！", L"错误", MB_OK );
		return false; //返回false，不用关闭注册表
	}

	CString StartCmd = L"\"" + GetFolderPath() + L"SpiritClient.exe\" /min";

	DWORD length = lstrlen( StartCmd );
	DWORD hkey_type=REG_SZ;  //定义数据类型
	
	ret=::RegSetValueEx(hKEY, L"SpiritClient", NULL,hkey_type,(const BYTE *)(LPCTSTR)StartCmd, length*sizeof(TCHAR) );
	if(ret!=ERROR_SUCCESS)
	{
		CString temp;
		temp.Format( L"无法写入注册表信息 错误代码：%d", ret);
		temp += L"\t\n 命令行：" + StartCmd;
		MessageBox( NULL, temp, L"错误", MB_OK );
		flag = false;
	}
	else
	{
		flag = true;
	}

	::RegCloseKey(hKEY);

	return flag;
}

bool DeleteAutostart()
{
	bool flag;

	HKEY hKEY;//定义有关的hKEY,在查询结束时要关闭
	
	//打开与路径 data_Set相关的hKEY
	CString tmp("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	
	//访问注册表，hKEY则保存此函数所打开的键的句柄
	long ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, tmp,0,KEY_READ|KEY_WRITE,&hKEY);
	
	if(ret!=ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
	{
		MessageBox( NULL, L"无法打开注册表！", L"错误", MB_OK );
		return false; //返回false，不用关闭注册表
	}

	//ret = ::RegDeleteKey(hKEY, "SecClient" );
	ret = ::RegDeleteValue(hKEY, L"SpiritClient" );  //注意Key和Value，打开注册表，左边树形结构是key，右边list结构是value
	if(ret!=ERROR_SUCCESS)
	{
		MessageBox( NULL, L"无法删除注册表项！", L"错误", MB_OK );
		flag = false;
	}
	else
	{
		flag = true;
	}

	return flag;
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REGWORKER));
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