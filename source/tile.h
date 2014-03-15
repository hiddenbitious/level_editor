#ifndef _TILE_H_
#define _TILE_H_

#include "globals.h"

#define MAX_PARAMETER_LENGTH			256   /// Maximum length of a parameter.

#define TILES_ON_X                   80
#define TILES_ON_Y                   50

#define NEIGHBOUR_LEFT                0
#define NEIGHBOUR_RIGHT               1
#define NEIGHBOUR_BELOW               2
#define NEIGHBOUR_ABOVE               3

typedef enum {AREA_NAN,          /// 0 Not categorized yet
              AREA_VOID,         /// 1
              AREA_WALKABLE,     /// 2
              AREA_WALL,         /// 3

              N_AREA_TYPES} areaTypes_t;

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
static const float selections[N_TILE_TYPES + 1][3] = { {1.0f,	0.0f,	0.0f},	// Wall
                                                       {0.0f,	1.0f,	0.0f},	// Staircase
                                                       {0.0f,	0.0f,	1.0f},	// Door
                                                       {0.0f,	0.0f,	0.0f},	// Monster (type can be a parameter)
                                                       {1.0f,	0.0f,	1.0f},	// Whatever
                                                       {1.0f,	1.0f,	0.0f},
                                                       {0.0f,	1.0f,	1.0f},
                                                       {1.0f,	1.0f,	1.0f},
                                                       /// Tile type 0 with parameter.
                                                       {0.8f, 0.8f, 0.8f}};

class tile
{
private:
	tileTypes_t type;	/// tile's type
	string parameter;	/// parameter saved to/read from file
	areaTypes_t area;

public:
   tile(void);
	tile(int x, int y);
	~tile(void);

	int x, y;			/// coordinates

	bool hasParameter;
	void setParameter(string param);

	void draw(float tileSize);

   inline void setCoordX(int x) { this->x = x; }
   inline void setCoordY(int y) { this->y = y; }
   inline void setType(tileTypes_t type){ this->type = type; area = (type == TILE_WALL) ? AREA_WALL : AREA_NAN; }
   inline tileTypes_t getType(void) { return this->type; }
   inline void setArea(areaTypes_t area){ this->area = area; }
   inline areaTypes_t getArea(void) { return this->area; }
   inline string getParameter(void) { return this->parameter; }
};

#endif
