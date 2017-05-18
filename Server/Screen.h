#ifndef SCREEN_H
#define SCREEN_H

#include "Packages.h"
#include "curses.h"
#include <string>

#define ENTER			10
#define BACKSPACE		8 
#define ASCII_START		31
#define ASCII_END		127		

#define HELP_FILE		"resources/Help.txt"

class Screen
{
public:
	Screen();

	void initTerminal();
	void setCommandLine();
	void putNext(std::string inputText);
	void putClear(std::string inputText);
	void showErrorMsg(errorCodes code, std::string errorMsg);
	void shiftLinesUp();
	unsigned inputPos;
	WINDOW * terminalWindow;
private:
	
	unsigned lastLine;

};
#endif // !SCREEN_H
