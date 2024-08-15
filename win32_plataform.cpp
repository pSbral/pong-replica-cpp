#include <Windows.h>

bool window_exec = true;

LRESULT window_callback(
	HWND    hWnd,
	UINT    Msg,
	WPARAM  wParam,
	LPARAM  lParam
) {
	LRESULT result = 0;

	switch (Msg) {
		case WM_CLOSE:
		case WM_DESTROY: {
			window_exec = false;
		}
		default: {
			result = DefWindowProc(hWnd, Msg, wParam, lParam);
		}
	}
	
	
	return result;
}

int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
) {
	//Criando a classe da janela de execução
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Game Window Class";
	window_class.lpfnWndProc = window_callback;
	window_class.hInstance = hInstance;
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);

	//Registrar a Janela
	if (!RegisterClass(&window_class)) {
		MessageBox(NULL, L"Falha ao registrar a classe da janela.", L"Erro", MB_OK | MB_ICONERROR);
		return -1;
	}

	//Criar a Janela
	HWND window = CreateWindowW(
		window_class.lpszClassName, 
		L"Game View", 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		1280, 720, 
		0, 0, 
		hInstance, 0);

	if (!window) {
		MessageBox(NULL, L"Falha ao criar a janela.", L"Erro", MB_OK | MB_ICONERROR);
		return -1;
	}

	// Loop de mensagens
	while (window_exec) {
		MSG msg = {};
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}