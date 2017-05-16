#include "Screen.h"
#include <fstream>
#include <sstream>

Screen::Screen():lastLine(0)
{
	terminalWindow = initscr();
	nodelay(terminalWindow, TRUE);
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	wbkgd(terminalWindow, COLOR_PAIR(1));
	attron(A_BOLD);
	scrollok(terminalWindow, TRUE);
}

void Screen::initTerminal()
{
	clear();
	move(terminalWindow->_maxy - 3, 0);
	for (int i = 0; i < terminalWindow->_maxx; i++)
	{
		printw("-");
		move(terminalWindow->_cury, i);
	}
	setCommandLine();
}

void Screen::setCommandLine()
{
	move(terminalWindow->_maxy - 2, 0);
	clrtoeol();
	color_set(2, NULL);
	printw("Server>");
	color_set(1, NULL);
	refresh();
}

void Screen::putClear(std::string inputText)
{
	initTerminal();
	move(0, 0);
	printw(inputText.c_str());
	lastLine = terminalWindow->_cury;
	move(terminalWindow->_maxy - 2, 7);
	refresh();
}

void Screen::putNext(std::string inputText)
{	
	if (lastLine == terminalWindow->_maxy - 5)
	{
		shiftLinesUp();
		move(lastLine, 0);
	}
	else
	{
		move(lastLine + 1, 0);
	}

	printw(inputText.c_str());
	lastLine = terminalWindow->_cury;
	move(terminalWindow->_maxy - 2, 7);
	refresh();
}

void Screen::showErrorMsg(errorCodes code, std::string errorMsg)
{
	switch (code)
	{
	case NOT_DEFINED:
		putNext(errorMsg);
		break;
	case FILE_NOT_FOUND:
		putNext("RRQ failed. File not found.");
		break;
	case FILE_ALREADY_EXISTS:
		putNext("WRQ failed. File already exists");
		break;
	default:
		break;
	}
}

void Screen::shiftLinesUp()
{
	char *aux = new char[terminalWindow->_maxx];
	for (int i = 0; i < lastLine; i++)
	{
		mvinstr(i + 1, 0, aux);
		mvprintw(i, 0, aux);
		refresh();
	}
}