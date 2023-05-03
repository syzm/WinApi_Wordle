#include "board.h"
#include <stdexcept>


std::wstring const board::s_class_name{ L"WORDLE - PUZZLE" };

bool board::register_class()
{
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_class_name.c_str(),
		&desc) != 0)
		return true;

	desc =
	{
		.cbSize = sizeof(WNDCLASSEXW),
		.lpfnWndProc = window_proc_static,
		.hInstance = m_instance,
		.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
		.hbrBackground = CreateSolidBrush(RGB(255, 255, 255)),
		.lpszClassName = s_class_name.c_str(),
	};
	return RegisterClassExW(&desc) != 0;
}

LRESULT board::window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	board* app = nullptr;
	if (message == WM_NCCREATE)
	{
		app = static_cast<board*>(
			reinterpret_cast<LPCREATESTRUCTW>(lparam)
			->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app));
	}
	else
		app = reinterpret_cast<board*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	LRESULT res = app ?
		app->window_proc(window, message, wparam, lparam) :
		DefWindowProcW(window, message, wparam, lparam);
	if (message == WM_NCDESTROY)
		SetWindowLongPtrW(window, GWLP_USERDATA, 0);
	return res;
}

LRESULT board::window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_NCHITTEST:
		return HTCAPTION;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(window, &ps);
		HPEN pen = CreatePen(PS_SOLID, 2, RGB(222, 225, 233));
		HPEN oldPen = (HPEN)SelectObject(hdc, pen);
		HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
		RECT size{ 0, 0, 0, 0 };
		for (LONG row = 0; row < rows; ++row)
			for (LONG column = 0; column < columns; ++column)
			{
				size.top = border + row * (field_size + margin) + margin;
				size.left = border + column * (field_size + margin) + margin;
				size.bottom = size.top + field_size;
				size.right = size.left + field_size;
				RoundRect(hdc, size.left, size.top, size.right, size.bottom, 6, 6);
			}
		SelectObject(hdc, oldBrush);
		DeleteObject(pen);
		DeleteObject(brush);
		EndPaint(window, &ps);
		break;
	}
	case WM_CTLCOLORSTATIC:
		return reinterpret_cast<INT_PTR>(CreateSolidBrush(RGB(164, 174, 196)));
	case WM_CLOSE:
		DestroyWindow(board_main);
		return 0;
	case WM_DESTROY:
		DestroyWindow(board_main);
		return 0;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

HWND board::create_window(DWORD style, HWND parent)
{
	RECT size{ 0, 0, width, height() };
	AdjustWindowRectEx(&size, style, false, 0);
	int board_width = size.right - size.left;
	int board_height = size.bottom - size.top;
	int centerX = (GetSystemMetrics(SM_CXSCREEN) - board_width) / 2 ;
	int centerY = (GetSystemMetrics(SM_CYSCREEN) - board_height) / 2;
	HWND window = CreateWindowExW(
		0,
		s_class_name.c_str(),
		L"WORDLE - PUZZLE",
		WS_OVERLAPPED | WS_CAPTION |
		WS_BORDER | WS_MINIMIZEBOX,
		centerX,
		centerY,
		board_width,
		board_height,
		parent,
		nullptr,
		m_instance,
		this);
	return window;
}


board::board(HWND parent, LONG rows) : rows {rows}
{
	register_class();
	style = WS_OVERLAPPED | WS_CAPTION;
	board_main = create_window(style, parent);
}

int board::run(int show_command)
{
	ShowWindow(board_main, show_command);
	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return EXIT_SUCCESS;
}






