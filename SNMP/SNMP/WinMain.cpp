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
//                  ���汣��             ����BUG 
//          ��Ի:  
//                  д��¥��д�ּ䣬д�ּ������Ա��  
//                  ������Աд�������ó��򻻾�Ǯ��  
//                  ����ֻ���������������������ߣ�  
//                  ��������ո��գ����������긴�ꡣ  
//                  ��Ը�������Լ䣬��Ը�Ϲ��ϰ�ǰ��  
//                  ���۱������Ȥ���������г���Ա��  
//                  ����Ц��߯��񲣬��Ц�Լ���̫����  
//                  ��������Ư���ã��ĸ���ó���Ա�� 


#include <Windows.h>

//�����ص�����
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//WinMain������������ڵ㣬
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR CmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Hello, world!");							//������
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;									//���ڷ��
	wndclass.lpfnWndProc = WndProc;												//�ص�����
	wndclass.cbClsExtra = 0;													//��wndclass�ṹ����������ֽ���
	wndclass.cbWndExtra = 0;													//����ʵ��������ֽ���������ʲô����
	wndclass.hInstance = hInstance;												//���
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);							//ͼ��
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);								//�����״	
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);				//������ɫ	
	wndclass.lpszMenuName = NULL;												//�˵���
	wndclass.lpszClassName = szAppName;											//��������

	if (!RegisterClass(&wndclass)){
		MessageBox(NULL, TEXT("Error"), TEXT("�޷�ע�ᴰ��"), MB_OK);
		return -1;
	}

	//ע�ᴰ��
	hwnd = CreateWindow(szAppName,												//����
		TEXT("��ó���"),														//����
		WS_OVERLAPPEDWINDOW,													//���ڷ��Creates an overlapped window  
		CW_USEDEFAULT,															//x
		CW_USEDEFAULT,															//y
		CW_USEDEFAULT,															//width
		CW_USEDEFAULT,															//high
		NULL,																	//������
		NULL,																	//�˵����
		hInstance,																//���ھ��
		NULL);

	ShowWindow(hwnd, iCmdShow);													//��ʾ����
	UpdateWindow(hwnd);															//���´��ڡ�����ʾ��֮���Ҫ���¡�������

	while (GetMessage(&msg, NULL, 0, 0)){											//��Ϣѭ����������
		TranslateMessage(&msg);													//����
		DispatchMessage(&msg);													//�ַ�
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)			//���ǻص���������
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

	//������Ϣ
	switch (message){
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);

		DrawText(hdc, TEXT("���GUI"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}