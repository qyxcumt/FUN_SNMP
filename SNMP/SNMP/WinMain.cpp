//                            _ooOoo_  
//                           o8888888o  
//                           88" . "88  
//                           (| -_- |)  
//                            O\ = /O  
//                        ____/`---'\____  
//                      .   ' \\| |// `.  
//                       / \\||| : |||// \  
//                     / _||||| -:- |||||- \  
//                       | | \\\ - /// | |  
//                     | \_| ''\---/'' | |  
//                      \ .-\__ `-` ___/-. /  
//                   ___`. .' /--.--\ `. . __  
//                ."" '< `.___\_<|>_/___.' >'"".  
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
//                 \ \ `-. \_ __\ /__ _/ .-` / /  
//         ======`-.____`-.___\_____/___.-`____.-'======  
//                            `=---='  
//  
//         .............................................  
//                  佛祖保佑             永无BUG 
//          佛曰:  
//                  写字楼里写字间，写字间里程序员；  
//                  程序人员写程序，又拿程序换酒钱。  
//                  酒醒只在网上坐，酒醉还来网下眠；  
//                  酒醉酒醒日复日，网上网下年复年。  
//                  但愿老死电脑间，不愿鞠躬老板前；  
//                  奔驰宝马贵者趣，公交自行程序员。  
//                  别人笑我忒疯癫，我笑自己命太贱；  
//                  不见满街漂亮妹，哪个归得程序员？ 


#include <Windows.h>

//声明回调函数
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//WinMain函数，程序入口点，
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR CmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Hello, world!");							//程序名
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;									//窗口风格
	wndclass.lpfnWndProc = WndProc;												//回调函数
	wndclass.cbClsExtra = 0;													//在wndclass结构体后额外分配字节数
	wndclass.cbWndExtra = 0;													//窗体实例后分配字节数（这是什么鬼？）
	wndclass.hInstance = hInstance;												//句柄
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);							//图标
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);								//鼠标形状	
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);				//背景颜色	
	wndclass.lpszMenuName = NULL;												//菜单名
	wndclass.lpszClassName = szAppName;											//窗体类名

	if (!RegisterClass(&wndclass)){
		MessageBox(NULL, TEXT("Error"), TEXT("无法注册窗口"), MB_OK);
		return -1;
	}

	//注册窗体
	hwnd = CreateWindow(szAppName,												//类名
		TEXT("你好程序"),														//标题
		WS_OVERLAPPEDWINDOW,													//窗口风格，Creates an overlapped window  
		CW_USEDEFAULT,															//x
		CW_USEDEFAULT,															//y
		CW_USEDEFAULT,															//width
		CW_USEDEFAULT,															//high
		NULL,																	//父窗口
		NULL,																	//菜单句柄
		hInstance,																//窗口句柄
		NULL);

	ShowWindow(hwnd, iCmdShow);													//显示窗口
	UpdateWindow(hwnd);															//更新窗口……显示完之后就要更新…………

	while (GetMessage(&msg, NULL, 0, 0)){											//消息循环…………
		TranslateMessage(&msg);													//翻译
		DispatchMessage(&msg);													//分发
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)			//这是回调函数……
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

	//处理消息
	switch (message){
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);

		DrawText(hdc, TEXT("你好GUI"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}