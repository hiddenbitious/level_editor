#ifndef _POPUP_H_
#define _POPUP_H_

class popUp
{
private:
	vector<string> options;
	int size_x , size_y;
	int elements;
	int x , y;
	bool returns;

public:
//returns returns
	bool hasOptions ( void );

	popUp ( const vector<string>& options , bool returns );
	popUp ( const string& option , bool returns );
	~popUp ();

	void draw ( void );

	int chooseOption ( void );
};

#endif