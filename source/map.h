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

#define NEIGHBOUR_LEFT                0
#define NEIGHBOUR_RIGHT               1
#define NEIGHBOUR_BELOW               2
#define NEIGHBOUR_ABOVE               3

typedef struct {
   /// Tile's start coordinates
   int x, y;
   /// Tile size
   int width, height;
   areaTypes_t neighbourAreas[4];
} mergedTile_t;

class C_Map {
public:
   C_Map(void);
   ~C_Map(void);
   bool readMap(const char *filename);
   bool saveMap(const char *filename);
   void saveGeometryToFile(const char *filename);
   void drawGrid(float tileSize);

   /// All map tiles
   tile tiles[TILES_ON_X][TILES_ON_Y];

   /**
    * Starting point of the merging algorithm.
    * The purpose is to detect wall tiles that form
    * rows, columns or even blocks and merge them into one block
    */
   void mergeTiles(void);

private:
   /// Holds merged tiles after 1st and 2nd pass.
   /// Used only to generate geometry
   vector<mergedTile_t> mergedTiles;

   /**
    * Detects rows or columns of consecutive tiles and merges them into 1 block.
    * The merged tiles are stored in the mergedTiles vector
    */
   void firstPass(int x, int y, bool **visitedTiles);

   /**
    * Runs a second pass on the merged tiles vector generated from the first pass.
    * The purpose is to further merge tile rows with same width, or columns with same height
    * into rectangles.
    */
   void secondPass(void);

   /**
    * Divide map into areas:
    * Walkable (aka inside the map) and VOID
    */
   void divideAreas(void);

   /**
    * Performs the flood fill algorithm in the map.
    * The flood begins from startTile
    */
   void floodFill(tile *startTile, areaTypes_t area);
   int setNeighboutAreas(void);
   areaTypes_t detectAreaAcrossWall(mergedTile_t *tile, int dirX, int dirY, int neighbour);
};
#endif
