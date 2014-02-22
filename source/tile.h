#ifndef _TILE_H_
#define _TILE_H_

#include "globals.h"

typedef enum {TILE_0,
              TILE_WALL,
              TILE_2,
              TILE_3,
              TILE_4,
              TILE_5,
              TILE_6,
              TILE_7,

              N_TILE_TYPES} tileTypes_t;

/// RGB colors of the available tile types
static const float selections[N_TILE_TYPES + 1][3] = { {1.0f,	0.0f,	0.0f},	//Wall
                                                       {0.0f,	1.0f,	0.0f},	//Staircase
                                                       {0.0f,	0.0f,	1.0f},	//Door
                                                       {0.0f,	0.0f,	0.0f},	//Monster (type can be a parameter)
                                                       {1.0f,	0.0f,	1.0f},	//Whatever
                                                       {1.0f,	1.0f,	0.0f},
                                                       {0.0f,	1.0f,	1.0f},
                                                       {1.0f,	1.0f,	1.0f},
                                                       /// Tile type 0 with parameter.
                                                       {0.8f, 0.8f, 0.8f}};

class tile
{
public:
	int x, y;			/// coordinates

private:
	int type;			/// tile's type
	string parameter;	/// parameter saved to/read from file

public:
	bool hasParameter;

	tile(int x = 0, int y = 0);
	~tile(void);

	void draw(float tileSize);

	void setParameter(string param);

   inline void setCoordX(int x) { this->x = x; }
   inline void setCoordY(int y) { this->y = y; }
   inline void setType(int type){ this->type = type; }
   inline int getType(void) { return this->type; }
   inline string getParameter(void) { return this->parameter; }
};

#endif
