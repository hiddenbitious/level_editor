#ifndef _POPUP_H_
#define _POPUP_H_

#include "globals.h"

class popUp
{
private:
	vector<string> options;
	int size_x, size_y;
	int windowWidth, windowHeight;
	int elements;
	int x, y;
	bool returns;

public:
	bool hasOptions(void);

	popUp(const vector<string>& options, bool returns, int mouse_x, int mouse_y, int windowWidth, int windowHeight);
	popUp(const string& option, bool returns, int mouse_x, int mouse_y, int windowWidth, int windowHeight);
	~popUp();

	void draw(int mouse_x, int mouse_y);
	void setWindowDimensions(int w, int h);

	int chooseOption(int mouse_x, int mouse_y);
};

#endif
