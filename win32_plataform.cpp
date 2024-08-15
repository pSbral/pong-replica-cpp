#include <Windows.h>

bool window_exec = true;

struct RenderState
{
	int height, width;
	void* memory;

	BITMAPINFO bitmap_info;
};

RenderState render_state;

#include "renderer.cpp"

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
		} break;
		case WM_SIZE: {
			RECT rect;
			GetClientRect(hWnd, &rect);

			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			int size = render_state.width * render_state.height * sizeof(unsigned int);

			if (render_state.height) VirtualFree(render_state.memory, 0, MEM_RELEASE);
			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;
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
	HDC hdc = GetDC(window);

	if (!window) {
		MessageBox(NULL, L"Falha ao criar a janela.", L"Erro", MB_OK | MB_ICONERROR);
		return -1;
	}

	// Loop de mensagens
	while (window_exec) {
		//Input
		MSG msg = {};
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//Simulate
		render_background();

		//Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}

	return 0;
}