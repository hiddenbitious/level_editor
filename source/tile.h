#ifndef _TILE_H_
#define _TILE_H_

#include "globals.h"

#define MAX_TILE_TYPES                8   /// Number of different tile types
#define TILES_ON_X                   80
#define TILES_ON_Y                   50

/// RGB colors of the available tile types
static const float selections[MAX_TILE_TYPES+1][3] = { {1.0f,	0.0f,	0.0f},	//Wall
                                          {0.0f,	1.0f,	0.0f},	//Staircase
                                          {0.0f,	0.0f,	1.0f},	//Door
                                          {0.0f,	0.0f,	0.0f},	//Monster (type can be a parameter)
                                          {1.0f,	0.0f,	1.0f},	//Whatever
                                          {1.0f,	1.0f,	0.0f},
                                          {0.0f,	1.0f,	1.0f},
                                          {1.0f,	1.0f,	1.0f},
                                          /// Tile type 0 with parameter.
                                          {0.9f, 0.9f, 0.9f}};

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

	void draw(float tileSize);

	void setParameter(string param);
	void setType(int type);

	string getParameter(void);
	int getType(void);

	void setCoordX(int x);
	void setCoordY(int y);
};

#endif
