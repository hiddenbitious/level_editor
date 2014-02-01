#ifndef _POPUP_H_
#define _POPUP_H_

#include "globals.h"

class popUp
{
private:
	vector<string> options;
	int size_x, size_y;
	int elements;
	int x, y;
	bool returns;

public:
	bool hasOptions(void);

	popUp(const vector<string>& options, bool returns, int mouse_x, int mouse_y);
	popUp(const string& option, bool returns, int mouse_x, int mouse_y);
	~popUp();

	void draw(int mouse_x, int mouse_y);

	int chooseOption(int mouse_x, int mouse_y);
};

#endif
