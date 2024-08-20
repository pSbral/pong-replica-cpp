#include <Windows.h>
#include "utils.cpp"

global_variable bool window_exec = true;

struct RenderState
{
	int height, width;
	void* memory;

	BITMAPINFO bitmap_info;
};

global_variable RenderState render_state;

#include "plataform_common.cpp"
#include "renderer.cpp"
#include "game.cpp"

LRESULT CALLBACK window_callback(
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

			int size = render_state.width * render_state.height * sizeof(u32);

			if (render_state.height) VirtualFree(render_state.memory, 0, MEM_RELEASE); // Liberar memória antiga, se existir
			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height; // Valor negativo inverte os eixos verticais
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
	// Inicializa a memória
	render_state.memory = NULL;

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
		hInstance, 0
	);
	
	if (!window) {
		MessageBox(NULL, L"Falha ao criar a janela.", L"Erro", MB_OK | MB_ICONERROR);
		return -1;
	}

	HDC hdc = GetDC(window);

	Input input = {};

	float delta_time = 0.016666f;
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);

	float performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (float)perf.QuadPart;
	}

	// Loop de mensagens
	while (window_exec) {
		//Input
		MSG msg = {};
		
		for (int i = 0; i < BUTTON_COUNT; i++) {
			input.buttons[i].changed = false;
		}

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			switch (msg.message) {
				case WM_KEYUP:
				case WM_KEYDOWN: {
					u32 vk_code = (u32)msg.wParam;
					bool is_down = ((msg.lParam & (1 << 31)) == 0);

					switch (vk_code)
						{
					case VK_UP: {
						input.buttons[BUTTON_UP].is_down = is_down;
						input.buttons[BUTTON_UP].changed = true;
					} break;
					case VK_DOWN: {
						input.buttons[BUTTON_DOWN].is_down = is_down;
						input.buttons[BUTTON_DOWN].changed = true;
					} break;
					case VK_LEFT: {
						input.buttons[BUTTON_LEFT].is_down = is_down;
						input.buttons[BUTTON_LEFT].changed = true;
					} break;
					case VK_RIGHT: {
						input.buttons[BUTTON_RIGHT].is_down = is_down;
						input.buttons[BUTTON_RIGHT].changed = true;
					} break;
					default:
						break;
					}
				} break;

				default: {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

		//Simulate
		simulate_game(&input, delta_time);

		//Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		
		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		frame_begin_time = frame_end_time;
	}

	// Libera o HDC
	ReleaseDC(window, hdc);

	// Libera a memória alocada
	if (render_state.memory) {
		VirtualFree(render_state.memory, 0, MEM_RELEASE);
	}

	return 0;
}