#ifndef _MAP_H_
#define _MAP_H_

#include "globals.h"
#include "tile.h"

typedef struct {
   /// Tile's start coordinates
   int x, y;
   /// Tile size
   int width, height;
   areaTypes_t neighbourAreas[4];
} mergedTile_t;

typedef struct {
   int minX, minY, maxX, maxY;
} map_bbox_t;

class C_Map {
public:
   C_Map(void);
   ~C_Map(void);
   void drawGrid(float tileSize);
   bool readMap(const char *filename);
   bool saveMap(const char *filename);

   /**
    * Writes to file the final geometry need to build the BSP tree
    * for the map
    */
   void saveBspGeometryToFile(const char *filename);

   void saveMapAreasToFile(const char *filename);

   /// All map tiles
   tile tiles[TILES_ON_X][TILES_ON_Y];

   /**
    * Starting point of the merging algorithm.
    * The purpose is to detect wall tiles that form
    * rows, columns or even blocks and merge them into one block
    */
   map_bbox_t mergeTiles(void);

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

   /**
    * After merging tiles into blocks and after divinding the map into areas
    * this function detects the areas that surrounds each merged tile block.
    * The purpose is to further reduce the polygon by removing the wall edges
    * that are facing outside the map.
    */
   int setNeighbourAreas(void);

   /**
    * Swipes a merged tile's wall all neighbour tiles.
    * If all tiles are VOID then this wall is considered to face outside the map
    * and VOID is returned, else if at least 1 WALLKABLE tile is found
    * the wall is considered to be facing inside the map
    */
   areaTypes_t detectAreaAcrossWall(mergedTile_t *tile, int neighbour);
};
#endif
