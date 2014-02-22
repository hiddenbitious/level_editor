#ifndef _MAP_H_
#define _MAP_H_

#include "globals.h"
#include "tile.h"

#define MAX_PARAMETER_LENGTH			256   /// Maximum length of a parameter.
#define MAX_TILE_TYPES                8   /// Number of different tile types
#define TILES_ON_X                   80
#define TILES_ON_Y                   50

#define MAX_TILE_TYPES                8   /// Number of different tile types
#define TILES_ON_X                   80
#define TILES_ON_Y                   50

typedef struct {
   /// Tile center
   int x, y;
   /// Tile size
   int width, height;
} mergedTile_t;

class C_Map {
public:
   C_Map(void);
   ~C_Map(void);
   bool readMap(const char *filename);
   bool saveMap(const char *filename);
   void saveGeometryToFile(const char *filename);
   void mergeTiles(void);

   void drawGrid(float tileSize);

   /// All map tiles
   tile tiles[80][50];

private:
//   float tileSize;
   /// Holds merged tiles after 1st and 2nd pass.
   /// Used only to generate geometry
   vector<mergedTile_t> mergedTiles;

   /// Merging passes
   void firstPass(int x, int y, bool **visitedTiles);
   void secondPass(void);

   void divideAreas(void);
   void floodFill(tile *startTile, areaTypes_t area);
};
#endif
