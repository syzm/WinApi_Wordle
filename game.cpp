#include "game.h"
#include "resource.h"
#include <cctype>
#include <iostream>
#include <fstream>
#include <cstdlib> 
#include <algorithm>
#include <stdexcept>
#include <ctime>
#include <wingdi.h>

#pragma comment(lib, "Msimg32.lib")


std::wstring const game::s_class_name{ L"WORDLE - KEYBOARD" };

double add[3] = { 0, 0.5, 1.5 };

struct leter
{
	char let;
	int s_top;
	int s_left;
	int s_bot;
	int s_right;
	// Z - Zielone, S - szare, Y - zolty, N - niesprawdzone
	std::string colors = "NNNN";
};

struct leter litery[26];

bool game::register_class()
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
		.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1),
		.lpszClassName = s_class_name.c_str(),
		.hIconSm = LoadIcon(m_instance, MAKEINTRESOURCE(IDI_ICON1))
	};
	return RegisterClassExW(&desc) != 0;
}

void game::get_words(const char* filename)
{
	std::string line;
	std::ifstream Myfile;
	Myfile.open(filename);

	if (Myfile.is_open())
	{
		while (getline(Myfile, line))
			words.push_back(line);

		Myfile.close();
	}
}

bool game::CheckWord(std::string search)
{
	if (std::binary_search(words.begin(), words.end(), search)) {
		return true;
	}
	else
		return false;
}

LRESULT game::window_proc_static(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	game* app = nullptr;
	if (message == WM_NCCREATE)
	{
		app = static_cast<game*>(
			reinterpret_cast<LPCREATESTRUCTW>(lparam)
			->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app));
	}
	else
		app = reinterpret_cast<game*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	LRESULT res = app ?
		app->window_proc(window, message, wparam, lparam) :
		DefWindowProcW(window, message, wparam, lparam);
	if (message == WM_NCDESTROY)
		SetWindowLongPtrW(window, GWLP_USERDATA, 0);
	return res;
}

void game::draw_keyboard()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_main, &ps);
	RECT key{ 0,0, 0, 0 };
	GetWindowRect(m_main, &key);
	InvalidateRect(m_main, &key, true);
	for (int i = 0; i < 26; i++)
	{
		litery[i].colors = "NNNN";
	}
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(222, 225, 233));
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);
	HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	RECT size{ 0, 0, 0, 0 };

	int length[3] = { 10, 9, 7 };
	const wchar_t* letters = L"QWERTYUIOPASDFGHJKLZXCVBNM";
	int i = 0;

	for (LONG row = 0; row < 3; ++row)
		for (LONG column = 0; column < length[row]; ++column)
		{
			size.top = games[0]->border + row * (games[0]->field_size + games[0]->margin) + games[0]->margin;
			size.left = add[row] * games[0]->field_size + games[0]->border + column * (games[0]->field_size + games[0]->margin) + games[0]->margin;
			size.bottom = size.top + games[0]->field_size;
			size.right = size.left + games[0]->field_size;
			RoundRect(hdc, size.left, size.top, size.right, size.bottom, 6, 6);

			DrawText(hdc, &letters[i], 1, &size, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			i++;
		}
	SelectObject(hdc, oldPen);
	DeleteObject(pen);
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
	EndPaint(m_main, &ps);
}


LRESULT game::window_proc(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		SetWindowLong(window, GWL_EXSTYLE,
			GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(window, 0, 170, LWA_ALPHA);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(window, &ps);

		// pen for drawing the rectangles in keyboard
		HPEN pen = CreatePen(PS_SOLID, 2, RGB(222, 225, 233));
		HPEN oldPen = (HPEN)SelectObject(hdc, pen);

		// pen for filling the rectangles in keyboard
		HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

		RECT size{ 0, 0, 0, 0 };

		// amount of letters in each line
		int length[3] = { 10, 9, 7 };
		const wchar_t* letters = L"QWERTYUIOPASDFGHJKLZXCVBNM";
		int i = 0;

		for (LONG row = 0; row < 3; ++row)
			for (LONG column = 0; column < length[row]; ++column)
			{
				size.top = games[0]->border + row * (games[0]->field_size + games[0]->margin) + games[0]->margin;
				size.left = add[row] * games[0]->field_size + games[0]->border + column * (games[0]->field_size + games[0]->margin) + games[0]->margin;
				size.bottom = size.top + games[0]->field_size;
				size.right = size.left + games[0]->field_size;
				RoundRect(hdc, size.left, size.top, size.right, size.bottom, 6, 6);

				litery[i].let = letters[i];
				litery[i].s_bot = size.bottom;
				litery[i].s_left = size.left;
				litery[i].s_right = size.right;
				litery[i].s_top = size.top;

				DrawText(hdc, &letters[i], 1, &size, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				i++;
			}
		SelectObject(hdc, oldPen);
		DeleteObject(pen);
		SelectObject(hdc, oldBrush);
		DeleteObject(brush);
		EndPaint(window, &ps);
		break;
	}
	case WM_CHAR:
	{
		RECT size{ 0, 0, 0, 0 };
		size.top = games[0]->border + current_row * (games[0]->field_size + games[0]->margin) + games[0]->margin;
		size.left = games[0]->border + current_column * (games[0]->field_size + games[0]->margin) + games[0]->margin;
		size.bottom = size.top + games[0]->field_size;
		size.right = size.left + games[0]->field_size;
		HDC hdcs[4];
		HPEN pen = CreatePen(PS_SOLID, 2, RGB(222, 225, 233));
		HPEN oldpen[4];
		HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
		HBRUSH oldBrush[4];
		bool exists = false;
		for (int i = 0; i < window_count; ++i)
		{
			if (board_result[i] == false)
			{
				hdcs[i] = GetDC(games[i]->board_main);
				oldpen[i] = (HPEN)SelectObject(hdcs[i], pen);
				oldBrush[i] = (HBRUSH)SelectObject(hdcs[i], brush);
			}
		}
		if ((wparam >= 65 && wparam <= 90) || (wparam >= 97 && wparam <= 122))
		{
			if (current_column <= 4) word += tolower((char)wparam);
			wchar_t s = toupper((char)wparam);
			if (current_column <= 4 && current_row < games[0]->rows)
			{
				current_column++;
				for (int i = 0; i < window_count; ++i)
				{
					if (board_result[i] == false)
					{
						DrawText(hdcs[i], &s, 1, &size, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
				}
			}
		}
		// backspace
		if (wparam == 8)
		{
			if (current_column != 0)
			{
				word.pop_back();
				current_column--;
				size.left = games[0]->border + current_column * (games[0]->field_size + games[0]->margin) + games[0]->margin;
				size.right = size.left + games[0]->field_size;
				for (int i = 0; i < window_count; ++i)
				{
					RoundRect(hdcs[i], size.left, size.top, size.right, size.bottom, 6, 6);
					SelectObject(hdcs[i], oldpen[i]);
					SelectObject(hdcs[i], oldBrush[i]);
				}
				DeleteObject(pen);
				DeleteObject(brush);
			}
		}
		if (wparam == 13)
		{
			if (current_column == 5)
			{
				if (CheckWord(word))
				{

					CheckColors(word, hdcs);

					for (int i = 0; i < window_count; i++)
					{
						if (goals[i] == word)
						{
							board_result[i] = true;
							EndGameOverlay(hdcs[i], games[i]->board_main, i);
							ReleaseDC(games[i]->board_main, hdcs[i]);
						}
						if (current_row == games[0]->rows - 1)
						{
							EndGameOverlay(hdcs[i], games[i]->board_main, i);
							ReleaseDC(games[i]->board_main, hdcs[i]);
						}
					}
					current_row++;

				}
				else
				{
					for (int i = 0; i < window_count; ++i)
					{
						for (LONG column = 0; column < games[0]->columns; ++column)
						{
							size.left = games[0]->border + column * (games[0]->field_size + games[0]->margin) + games[0]->margin;
							size.right = size.left + games[0]->field_size;
							RoundRect(hdcs[i], size.left, size.top, size.right, size.bottom, 6, 6);
						}
						SelectObject(hdcs[i], oldpen[i]);
						SelectObject(hdcs[i], oldBrush[i]);
					}
					DeleteObject(pen);
					DeleteObject(brush);
				}

				word = "";
				current_column = 0;
			}

		}
		break;
	}


	case WM_COMMAND:
	{
		WORD wmId = LOWORD(wparam);
		CreateDiff(wparam, window);
	}
	break;

	case WM_CLOSE:
		DestroyWindow(m_main);
		delete(games);
		return 0;
		WritePrivateProfileString(L"WORDLE", L"DIFFICULTY", (LPWSTR)current_diff, L".\\Wordle.ini");
	case WM_DESTROY:
		PostQuitMessage(EXIT_SUCCESS);
		for (int i = 0; i < window_count; i++)
		{
			DeleteDC(hdcs[i]);
		}
		WritePrivateProfileString(L"WORDLE", L"DIFFICULTY", (LPWSTR)current_diff, L".\\Wordle.ini");
		return 0;
		break;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

void game::CheckColors(std::string pick, HDC* hdcs)
{
	if (pick.length() != 5) return;
	RECT size{ 0, 0, 0, 0 };
	size.top = games[0]->border + current_row * (games[0]->field_size + games[0]->margin) + games[0]->margin;
	size.left = games[0]->border + current_column * (games[0]->field_size + games[0]->margin) + games[0]->margin;
	size.bottom = size.top + games[0]->field_size;
	size.right = size.left + games[0]->field_size;

	RECT windowsize{ 0, 0, 0,0 };


	HPEN pengreen = CreatePen(PS_SOLID, 1, RGB(121, 184, 81));
	HBRUSH brushgreen = CreateSolidBrush(RGB(121, 184, 81));

	HPEN penyellow = CreatePen(PS_SOLID, 1, RGB(243, 194, 55));
	HBRUSH brushyellow = CreateSolidBrush(RGB(243, 194, 55));

	HPEN pengray = CreatePen(PS_SOLID, 1, RGB(164, 174, 196));
	HBRUSH brushgray = CreateSolidBrush(RGB(164, 174, 196));


	HDC hdckeybrd = GetDC(m_main);

	bool coloured = false;
	for (int j = 0; j < 5; ++j)
	{
		for (int i = 0; i < window_count; i++)
		{
			coloured = false;
			for (int k = 0; k < 5; ++k)
			{
				if (pick[j] == goals[i][k])
				{
					if (j == k)
					{
						// pokoloruj okienko na zielono
						size.left = games[0]->border + j * (games[0]->field_size + games[0]->margin) + games[0]->margin;
						size.right = size.left + games[0]->field_size;
						SelectObject(hdcs[i], pengreen);
						SelectObject(hdcs[i], brushgreen);
						RoundRect(hdcs[i], size.left, size.top, size.right, size.bottom, 6, 6);
						wchar_t s = toupper(pick[j]);
						SetBkMode(hdcs[i], TRANSPARENT);
						DrawText(hdcs[i], &s, 1, &size, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						coloured = true;

						SelectObject(hdckeybrd, pengreen);
						SelectObject(hdckeybrd, brushgreen);

						for (int t = 0; t < 26; t++)
						{
							if (litery[t].let == toupper(pick[j]))
							{
								if (litery[t].colors[i] != 'Z' && board_result[i] == false)
								{
									windowsize.left = litery[t].s_left;
									windowsize.right = litery[t].s_right;
									windowsize.bottom = litery[t].s_bot;
									windowsize.top = litery[t].s_top;
									if (window_count == 1)
									{
										RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top, litery[t].s_right, litery[t].s_bot, 3, 3);
									}
									else if (window_count == 2)
									{
										if (i == 0)
										{
											RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top, litery[t].s_left + games[0]->field_size / 2, litery[t].s_bot, 3, 3);
										}
										else if (i == 1)
										{
											RoundRect(hdckeybrd, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top, litery[t].s_right, litery[t].s_bot, 3, 3);
										}
									}
									else if (window_count == 4)
									{
										if (i == 0)
										{
											RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top + games[0]->field_size / 2, 3, 3);
										}
										else if (i == 1)
										{
											RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top + games[0]->field_size / 2, litery[t].s_left + games[0]->field_size / 2, litery[t].s_bot, 3, 3);
										}
										else if (i == 2)
										{
											RoundRect(hdckeybrd, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top, litery[t].s_right, litery[t].s_top + games[0]->field_size / 2, 3, 3);
										}
										else if (i == 3)
										{
											RoundRect(hdckeybrd, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top + games[0]->field_size / 2, litery[t].s_right, litery[t].s_bot, 3, 3);
										}
									}
									SetBkMode(hdckeybrd, TRANSPARENT);
									DrawText(hdckeybrd, &s, 1, &windowsize, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

									litery[t].colors[i] = 'Z';
								}
								break;
							}
						}
					}
					else
					{
						// pokoloruj okienko na zolto
						if (coloured == false)
						{
							size.left = games[0]->border + j * (games[0]->field_size + games[0]->margin) + games[0]->margin;
							size.right = size.left + games[0]->field_size;
							SelectObject(hdcs[i], penyellow);
							SelectObject(hdcs[i], brushyellow);
							RoundRect(hdcs[i], size.left, size.top, size.right, size.bottom, 6, 6);
							wchar_t s = toupper(pick[j]);
							SetBkMode(hdcs[i], TRANSPARENT);
							DrawText(hdcs[i], &s, 1, &size, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							coloured = true;

							SelectObject(hdckeybrd, penyellow);
							SelectObject(hdckeybrd, brushyellow);

							for (int t = 0; t < 26; t++)
							{
								if (litery[t].let == toupper(pick[j]))
								{
									if (litery[t].colors[i] == 'N' && board_result[i] == false)
									{
										windowsize.left = litery[t].s_left;
										windowsize.right = litery[t].s_right;
										windowsize.bottom = litery[t].s_bot;
										windowsize.top = litery[t].s_top;
										if (window_count == 1)
										{
											RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top, litery[t].s_right, litery[t].s_bot, 3, 3);
										}
										else if (window_count == 2)
										{
											if (i == 0)
											{
												RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top, litery[t].s_left + games[0]->field_size / 2, litery[t].s_bot, 3, 3);
											}
											else if (i == 1)
											{
												RoundRect(hdckeybrd, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top, litery[t].s_right, litery[t].s_bot, 3, 3);
											}
										}
										else if (window_count == 4)
										{
											if (i == 0)
											{
												RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top + games[0]->field_size / 2, 3, 3);
											}
											else if (i == 1)
											{
												RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top + games[0]->field_size / 2, litery[t].s_left + games[0]->field_size / 2, litery[t].s_bot, 3, 3);
											}
											else if (i == 2)
											{
												RoundRect(hdckeybrd, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top, litery[t].s_right, litery[t].s_top + games[0]->field_size / 2, 3, 3);
											}
											else if (i == 3)
											{
												RoundRect(hdckeybrd, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top + games[0]->field_size / 2, litery[t].s_right, litery[t].s_bot, 7, 7);
											}
										}
										SetBkMode(hdckeybrd, TRANSPARENT);
										DrawText(hdckeybrd, &s, 1, &windowsize, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
										litery[t].colors[i] = 'Y';
									}
									break;
								}
							}
						}

					}
				}
			}
			// jestli niepokolorowane to na szaro
			if (!coloured)
			{
				size.left = games[0]->border + j * (games[0]->field_size + games[0]->margin) + games[0]->margin;
				size.right = size.left + games[0]->field_size;
				SelectObject(hdcs[i], pengray);
				SelectObject(hdcs[i], brushgray);
				RoundRect(hdcs[i], size.left, size.top, size.right, size.bottom, 6, 6);
				wchar_t s = toupper(pick[j]);
				SetBkMode(hdcs[i], TRANSPARENT);
				DrawText(hdcs[i], &s, 1, &size, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				SelectObject(hdckeybrd, pengray);
				SelectObject(hdckeybrd, brushgray);

				for (int t = 0; t < 26; t++)
				{
					if (litery[t].let == toupper(pick[j]))
					{
						if (litery[t].colors[i] == 'N' && board_result[i] == false)
						{
							windowsize.left = litery[t].s_left;
							windowsize.right = litery[t].s_right;
							windowsize.bottom = litery[t].s_bot;
							windowsize.top = litery[t].s_top;
							if (window_count == 1)
							{
								RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top, litery[t].s_right, litery[t].s_bot, 3, 3);
							}
							else if (window_count == 2)
							{
								if (i == 0)
								{
									RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top, litery[t].s_left + games[0]->field_size / 2, litery[t].s_bot, 3, 3);
								}
								else if (i == 1)
								{
									RoundRect(hdckeybrd, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top, litery[t].s_right, litery[t].s_bot, 3, 3);
								}
							}
							else if (window_count == 4)
							{
								if (i == 0)
								{
									RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top + games[0]->field_size / 2, 3, 3);
								}
								else if (i == 1)
								{
									RoundRect(hdckeybrd, litery[t].s_left, litery[t].s_top + games[0]->field_size / 2, litery[t].s_left + games[0]->field_size / 2, litery[t].s_bot, 3, 3);
								}
								else if (i == 2)
								{
									RoundRect(hdckeybrd, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top, litery[t].s_right, litery[t].s_top + games[0]->field_size / 2, 3, 3);
								}
								else if (i == 3)
								{
									RoundRect(hdckeybrd, litery[t].s_left + games[0]->field_size / 2, litery[t].s_top + games[0]->field_size / 2, litery[t].s_right, litery[t].s_bot, 3, 3);
								}
							}
							SetBkMode(hdckeybrd, TRANSPARENT);
							DrawText(hdckeybrd, &s, 1, &windowsize, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

							litery[t].colors[i] = 'S';
						}
						break;
					}
				}
			}
		}
	}
	DeleteObject(pengreen);
	DeleteObject(brushgreen);
	DeleteObject(penyellow);
	DeleteObject(brushyellow);
	DeleteObject(pengray);
	DeleteObject(brushgray);
}



void game::EndGameOverlay(HDC& hdc, HWND& hWnd, int i)
{
	COLORREF col;
	LPCWSTR s;
	std::wstring stemp;
	if (board_result[i] == true)
	{
		col = RGB(0, 255, 0);
		s = L"";
	}
	else
	{
		stemp = std::wstring(goals[i].begin(), goals[i].end());
		col = RGB(255, 0, 0);
		s = stemp.c_str();
	}

	HBITMAP bitmap = CreateCompatibleBitmap(hdc, 800, 800);
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);

	HBRUSH brush = CreateSolidBrush(col);
	HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, brush);

	RECT rc = { 0, 0, 800, 800 };
	FillRect(memDC, &rc, brush);


	BLENDFUNCTION blend =
	{
		.BlendOp = AC_SRC_OVER,
		.BlendFlags = 0,
		.SourceConstantAlpha = 150,
		.AlphaFormat = 0
	};


	AlphaBlend(
		hdc,
		0, 0, 800, 800,
		memDC,
		0, 0, 800, 800,
		blend
	);


	SelectObject(memDC, oldBrush);
	DeleteObject(brush);

	SelectObject(memDC, oldBitmap);
	DeleteObject(bitmap);

	DeleteDC(memDC);

	RECT win{ 0,0, 0, 0 };
	GetClientRect(games[i]->board_main, &win);
	RECT key{ 0,0, win.right - win.left, win.bottom - win.top };

	SetTextColor(hdc, RGB(255, 255, 255));
	if(board_result[i] == false) DrawText(hdc, s, 5, &key, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

}


//https://www.reddit.com/r/cpp_questions/comments/2pc3i2/how_to_read_a_random_word_from_a_text_file/
std::string game::RandomWord()
{
	return words[rand() % words.size()];
}

HWND game::create_window()
{
	int centerX = GetSystemMetrics(SM_CXSCREEN) / 2 - 320;
	int centerY = GetSystemMetrics(SM_CYSCREEN) / 2 + 150;
	return CreateWindowExW(
		0,
		s_class_name.c_str(),
		L"WORDLE - KEYBOARD",
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
		WS_BORDER | WS_MINIMIZEBOX,
		centerX,
		centerY,
		640,
		260,
		nullptr,
		nullptr,
		m_instance,
		this);
}


void game::kill_games()
{
	draw_keyboard();
	DestroyWindow(games[0]->board_main);
	for (int i = 0; i < 4; i++)
	{
		board_result[i] = false;
	}
	goals[0] = "";
	if (window_count == 4)
	{
		DestroyWindow(games[1]->board_main);
		DestroyWindow(games[2]->board_main);
		DestroyWindow(games[3]->board_main);
		goals[1] = "";
		goals[2] = "";
		goals[3] = "";
	}
	else if (window_count == 2)
	{
		DestroyWindow(games[1]->board_main);
		goals[1] = "";
	}
	current_row = 0;
	current_column = 0;
}

void game::CreateDiff(WORD wmId, HWND window)
{
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	RECT size{ 0, 0, games[0]->width, games[0]->height() };
	int smolwidth = size.right - size.left;
	int smolheight = size.bottom - size.top;
	HMENU menu = GetMenu(window);
	switch (wmId)
	{
	case ID_EASY:
		current_diff = "1";
		CheckMenuItem(menu, current, MF_UNCHECKED);
		CheckMenuItem(menu, ID_EASY, MF_CHECKED);
		current = ID_EASY;

		kill_games();
		goals[0] = RandomWord();

		games[0] = new board{ m_main, 6 };

		size.bottom = games[0]->height();

		AdjustWindowRect(&size, games[0]->style, false);
		smolheight = size.bottom - size.top;
		smolwidth = size.right - size.left;

		ShowWindow(games[0]->board_main, SW_SHOWNA);

		window_count = 1;
		break;
	case ID_MEDIUM:
		current_diff = "2";
		CheckMenuItem(menu, current, MF_UNCHECKED);
		CheckMenuItem(menu, ID_MEDIUM, MF_CHECKED);
		current = ID_MEDIUM;

		kill_games();

		games[0] = new board{ m_main, 8 };

		size.bottom = games[0]->height();
		AdjustWindowRect(&size, games[0]->style, false);
		smolheight = size.bottom - size.top;
		smolwidth = size.right - size.left;

		goals[0] = RandomWord();
		goals[1] = RandomWord();

		MoveWindow(games[0]->board_main, (width / 2 - smolwidth) / 2, (height - smolheight) / 2, smolwidth, smolheight, true);
		ShowWindow(games[0]->board_main, SW_SHOWNA);

		games[1] = new board{ m_main, 8 };
		MoveWindow(games[1]->board_main, (3 * width / 2 - smolwidth) / 2, (height - smolheight) / 2, smolwidth, smolheight, true);
		ShowWindow(games[1]->board_main, SW_SHOWNA);

		window_count = 2;
		break;
	case ID_HARD:
		current_diff = "3";
		CheckMenuItem(menu, current, MF_UNCHECKED);
		CheckMenuItem(menu, ID_HARD, MF_CHECKED);
		current = ID_HARD;

		kill_games();

		games[0] = new board{ m_main, 10 };

		size.bottom = games[0]->height();
		AdjustWindowRect(&size, games[0]->style, false);
		smolheight = size.bottom - size.top;
		smolwidth = size.right - size.left;

		goals[0] = RandomWord();
		goals[1] = RandomWord();
		goals[2] = RandomWord();
		goals[3] = RandomWord();

		MoveWindow(games[0]->board_main, (width / 2 - smolwidth) / 2, (height / 2 - smolheight) / 2, smolwidth, smolheight, true);
		ShowWindow(games[0]->board_main, SW_SHOWNA);

		games[1] = new board{ m_main, 10 };
		MoveWindow(games[1]->board_main, (width / 2 - smolwidth) / 2, (3 * height / 2 - smolheight) / 2, smolwidth, smolheight, true);
		ShowWindow(games[1]->board_main, SW_SHOWNA);

		games[2] = new board{ m_main, 10 };
		MoveWindow(games[2]->board_main, (3 * width / 2 - smolwidth) / 2, (height / 2 - smolheight) / 2, smolwidth, smolheight, true);
		ShowWindow(games[2]->board_main, SW_SHOWNA);

		games[3] = new board{ m_main, 10 };
		MoveWindow(games[3]->board_main, (3 * width / 2 - smolwidth) / 2, (3 * height / 2 - smolheight) / 2, smolwidth, smolheight, true);
		ShowWindow(games[3]->board_main, SW_SHOWNA);

		window_count = 4;
		break;
	}
}

void game::SetDiffIni()
{
	CHAR diff[256];
	GetPrivateProfileString(L"WORDLE", L"DIFFICULTY", L"1", (LPWSTR)diff, 256, L".\\Wordle.ini");

	if (diff[0] == '1')
	{
		current = ID_EASY;
	}
	else if (diff[0] == '2')
	{
		current = ID_MEDIUM;
	}
	else if (diff[0] == '3')
	{
		current = ID_HARD;
	}
	current_diff = diff;
}

game::game(HINSTANCE instance, const char* filename)
	: m_instance{ instance }, m_main{}, window_count{ 1 }, current_row{ 0 }, current_column{ 0 }, filename{ filename }, current{ ID_EASY }
{
	SetDiffIni();
	get_words(filename);
	games = new board * [4];
	srand(time(nullptr));
	register_class();
	m_main = create_window();
	games[0] = new board{ m_main, 6 };
	CreateDiff(current, m_main);
}

int game::run(int show_command)
{
	ShowWindow(m_main, show_command);
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

