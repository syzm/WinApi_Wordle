#pragma once

#include <string>
#include <Windows.h>
#include "board.h"
#include <vector>

class board;

class game
{
private:
	bool register_class();

	// getting words for game from file
	std::vector<std::string> words;
	void get_words(const char* filename);

	// check if the word exists
	bool CheckWord(std::string word);

	static std::wstring const s_class_name;

	static LRESULT CALLBACK window_proc_static(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);

	void draw_keyboard();

	LRESULT window_proc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);

	HWND create_window();

	HINSTANCE m_instance;
	HWND m_main;

	board **games;
	void kill_games();

	HDC hdcs[4];

	WORD current;
	const char* current_diff;
	void CreateDiff(WORD wmId, HWND window);

	void SetDiffIni();
	int window_count;

	int current_row;
	int current_column;

	const char* filename;
	int filelen;

	std::string word;


	void CheckColors(std::string pick, HDC* hdcs);

	std::string RandomWord();
	std::string goals[4];
	bool board_result[4] = { false, false, false, false };

	void EndGameOverlay(HDC&hdc, HWND& hWnd, int i);

public:
	game(HINSTANCE instance, const char* filename);
	friend class board;
	int run(int show_command);

};