#ifndef _TILE_H_
#define _TILE_H_

#include "globals.h"

class tile
{
private:
	int x, y;			/// coordinates
	int type;			/// tile's type
	string parameter;	/// parameter saved to/read from file

public:
	bool hasParameter;

	tile(int x = 0, int y = 0);
	~tile(void);

	void draw(void);

	void setParameter ( string param );
	void setType ( int type );

	string getParameter(void);
	int getType(void);

	void setCoordX(int x);
	void setCoordY(int y);
};

#endif
