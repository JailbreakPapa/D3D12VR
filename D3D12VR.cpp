#include "VRD3D12.h"

VRD3D12 dx;
HWND hwnd = nullptr;

HWND InitWindow(LPCWSTR name, HINSTANCE inst, int show_cmd, int width, int height, bool fullscreen)
{

	if (fullscreen) {
		HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hmon, &mi);

		width = mi.rcMonitor.right - mi.rcMonitor.left;
		height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = inst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = LPCWSTR(name);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
		throw("Failed to register class with error: " + GetLastError());

	hwnd = CreateWindowEx(NULL,
		LPCWSTR(name), LPCWSTR(name),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		inst,
		NULL);

	if (!hwnd)
		throw("Failed to create window with error: " + GetLastError());

	if (fullscreen) {
		SetWindowLong(hwnd, GWL_STYLE, 0);
	}

	ShowWindow(hwnd, show_cmd);
	UpdateWindow(hwnd);

	return hwnd;
}

LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		GameTimer::Stop();
		return 0;
	case WM_KEYDOWN:
		if (w_param == VK_ESCAPE)
			DestroyWindow(hWnd);
		return 0;
	}

	
	return DefWindowProc(hWnd, msg, w_param, l_param);
}

void StartLoop(std::function<void()> init, std::function<void()> render)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	init();

	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			render();
		}
	}
}

INT CALLBACK WinMain(HINSTANCE inst, HINSTANCE prev_inst, LPSTR arg, int show_cmd) {
	GameTimer::Start();
	//int fc{ 0 };
	//auto fps = fc / (int)dx.g.GameTime();
	InitWindow(L"VREngine - D3D12", prev_inst, show_cmd, 1024, 764, false);
	StartLoop(&Init, &D3D12Render);
	return 0;
}

void Init()
{
	dx.Width = 1024;
	dx.Height = 764;
	dx.Windowed = true;
	dx.CreateContexts(hwnd);
}

void D3D12Render()
{
	dx.Render();
}