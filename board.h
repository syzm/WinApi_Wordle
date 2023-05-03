#pragma once

#include <string>
#include "game.h"
#include <Windows.h>

class board
{
private:
	bool register_class();

	static std::wstring const s_class_name;

	static LRESULT CALLBACK window_proc_static(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);

	LRESULT window_proc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);

	HWND create_window(DWORD style, HWND parent);
	static constexpr LONG columns = 5;
	static constexpr LONG margin = 6;
	static constexpr LONG field_size = 55;
	static constexpr LONG border = 4;
	static constexpr LONG width = columns * (field_size + margin) + margin + 2 * border;

	// need to have rows like that in order to create different boards for different levels
	LONG rows;
	LONG height()
	{
		return rows * (field_size + margin) + margin + 2 * border;
	};

	HINSTANCE m_instance;
	DWORD style;
	HWND board_main;

public:
	board(HWND parent, LONG rows);
	friend class game;
	int run(int show_command);
};