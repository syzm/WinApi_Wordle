#define NOMINMAX
#include <windows.h>
#include "game.h"



int WINAPI wWinMain(HINSTANCE instance,
	HINSTANCE /*prevInstance*/,
	LPWSTR /*command_line*/,
	int show_command)
{
	// passing the filename of .txt which contains words for the game
	game app{ instance, "Wordle.txt"};
	return app.run(show_command);
}