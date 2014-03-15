#include <stdio.h>
#include <vector>
#include <stack>
#include <string.h>

#include "map.h"

C_Map::C_Map(void)
{
	for(int x = 0; x < TILES_ON_X; x++) {
		for(int y = 0; y < TILES_ON_Y; y++) {
		   tiles[x][y].setCoordX(x);
		   tiles[x][y].setCoordY(y);
		   tiles[x][y].setArea(AREA_NAN);
		   tiles[x][y].hasParameter = false;
		}
	}
}

C_Map::~C_Map(void)
{
   if(mergedTiles.size())
      mergedTiles.clear();
}

void
C_Map::drawGrid(float tileSize)
{
   /// Number of thin lines per tile (only 2 is accurate enough).
   static const int linesPerTile = 3;
   float x, y;
	float w = TILES_ON_X * tileSize;
	float h = TILES_ON_Y * tileSize;
	float step = tileSize / linesPerTile;

	glBegin(GL_LINES);
	glColor3f(0.9f, 0.9f, 0.9f);

   /// Draw thin lines
   /// These are bound to loose precision
	for(y = 0; y < h; y += step )  {
		glVertex3f(0, y, 0);
		glVertex3f(w, y, 0);
	}

	for(x = 0; x < w; x += step ) {
		glVertex3f(x, 0, 0);
		glVertex3f(x, h, 0);
	}

   /// Draw bold lines
	glColor3f(0.7f, 0.7f, 0.7f);
	for(x = 0; x < TILES_ON_X; x++) {
		for(y = 0; y < TILES_ON_Y; y++) {
			glVertex2f(x * tileSize, y * tileSize);
			glVertex2f((x+1) * tileSize, y * tileSize);

			glVertex2f((x+1) * tileSize, y * tileSize);
			glVertex2f((x+1) * tileSize, (y+1) * tileSize);

			glVertex2f((x+1) * tileSize, (y+1) * tileSize);
			glVertex2f(x * tileSize, (y+1) * tileSize);

			glVertex2f(x * tileSize, (y+1) * tileSize);
			glVertex2f(x * tileSize, y * tileSize);
		}
	}
	glEnd();

   /// Draw marked tiles
	for(int tile_x = 0; tile_x < TILES_ON_X; tile_x++) {
		for(int tile_y = 0; tile_y < TILES_ON_Y; tile_y++) {
			tiles[tile_x][tile_y].draw(tileSize);
		}
	}
}

bool
C_Map::saveMap(const char *filename)
{
	int nTiles = 0, x, y;
	float tileSize = 10.0f;

	FILE *fd = fopen(filename, "w");

	if(!fd)
		return false;

   /// Count tiles that either has a parameter or is not the default type
	for(x = 0; x < TILES_ON_X; x++) {
		for(y = 0; y < TILES_ON_Y; y++) {
			if(tiles[x][y].getType() || tiles[x][y].hasParameter)
				++nTiles;
		}
	}

	fprintf(fd, "%d\n", TILES_ON_X);			/// Save x dimension
	fprintf(fd, "%d\n", TILES_ON_Y);			/// Save y dimension
	fprintf(fd, "%d\n", nTiles);			/// Save how many tiles will be saved
	fprintf(fd, "%f\n", tileSize);		/// Save tile's dimensions

	for(x = 0; x < TILES_ON_X; x++) {
		for(y = 0; y < TILES_ON_Y; y++) {
		   /// Write only non zero tiles.
			if(tiles[x][y].getType()) {
				fprintf(fd, "%d %d %d", x, y, tiles[x][y].getType());

				if ( tiles[x][y].hasParameter )
					fprintf(fd, " %s\n", tiles[x][y].getParameter().c_str());
				else
					fprintf(fd, "\n");
			} else if(tiles[x][y].hasParameter) {
      		/// And the empty tiles with a parameter
				fprintf(fd, "%d %d %d", x, y, 0);
				fprintf(fd, " %s\n", tiles[x][y].getParameter().c_str());
			}
		}
	}

	fclose(fd);

	printf("map saved.\n");

	return true;
}

bool
C_Map::readMap(const char *filename)
{
	int _xTiles, _yTiles, _tileSize, tmp, nTiles, _x, _y;
	int i, c, sum = 0;
	char buf[MAX_PARAMETER_LENGTH];
	int counters[N_TILE_TYPES] = {0};

	FILE *fd;

	if((fd = fopen(filename, "r")) == NULL)
		return false;

	fscanf(fd, "%d", &_xTiles);		/// Read size on x.
	fscanf(fd, "%d", &_yTiles);		/// Read size on y.
	fscanf(fd, "%d", &nTiles);		   /// Read number of tiles stored in file.
	fscanf(fd, "%f", &_tileSize);	   /// Read tile size (not used).

   assert(_xTiles == TILES_ON_X);
   assert(_yTiles == TILES_ON_Y);
   assert(nTiles <= TILES_ON_X * TILES_ON_Y);

	for(i = 0; i < nTiles; i++) {
		fscanf(fd, "%d", &_x);			/// Read x coords
		fscanf(fd, "%d", &_y);			/// Read y coords
		c = fscanf(fd, "%d", &tmp);	/// Read tile type
		assert(_x < TILES_ON_X);
		assert(_y < TILES_ON_Y);
		assert(tmp < N_TILE_TYPES);

		tiles[_x][_y].setType((tileTypes_t)tmp);
		tiles[_x][_y].setCoordX(_x);
		tiles[_x][_y].setCoordY(_y);

		/// Count tiles
		++counters[tmp];
		++sum;

   	/// There's a parameter. Read it.
		if(!c) {
		   /// fgets doesn't stop at white spaces but it stops at '\n'
			fgets(buf, MAX_PARAMETER_LENGTH, fd);
			tiles[_x][_y].setParameter(buf);
		   /// One loop is lost everytime a parameter is read.
			--i;
         --counters[tmp];
		}
	}

	fclose(fd);

   printf("\n*****\n");
	printf("Read map %s. Found:\n", filename);
	for(int i = 0; i < N_TILE_TYPES; i++) {
	   if(counters[i])
         printf("\tType %d: %d tiles\n", i, counters[i]);
	}
	printf("\tTotal %d tiles\n", sum);
   printf("*****\n");

	return true;
}

void
C_Map::secondPass(void)
{
   vector<mergedTile_t>::iterator itx;
   vector<mergedTile_t>::iterator ity;

   for(itx = mergedTiles.begin(); itx < mergedTiles.end(); itx++) {
      for(ity = mergedTiles.begin(); ity < mergedTiles.end(); ity++) {
         /// Merge rows
         if((*itx).y + (*itx).height == (*ity).y &&
            (*itx).x == (*ity).x &&
            (*itx).width == (*ity).width) {
            (*itx).height += (*ity).height;
            mergedTiles.erase(ity);
            --itx;
         } /// Merge collums
         else if((*itx).x + (*itx).width == (*ity).x &&
            (*itx).y == (*ity).y &&
            (*itx).height == (*ity).height) {
            (*itx).width += (*ity).width;
            mergedTiles.erase(ity);
            --itx;
         }
      }
   }
}

void
C_Map::firstPass(int x, int y, bool **visitedTiles)
{
   int xx = x, yy = y;
   int width = 1, height = 1;
   mergedTile_t merged = {x, y, width, height};

   /// Mark start tile as visited
   visitedTiles[xx][yy] = true;

   /// Scan row
   if((tiles[xx + 1][yy].getType() == TILE_WALL && !visitedTiles[xx + 1][yy]) /*|| yy == TILES_ON_Y - 1*/) {
      while(tiles[xx + 1][yy].getType() == TILE_WALL && xx <= TILES_ON_X - 2) {
         ++xx;
         ++width;
         visitedTiles[xx][yy] = true;
      }

      merged.width = width;
   } else if((tiles[xx][yy + 1].getType() == TILE_WALL && !visitedTiles[xx][yy + 1]) /*|| xx == TILES_ON_X - 1*/) {
      /// ... else scan the column
      while(tiles[xx][yy + 1].getType() == TILE_WALL && yy <= TILES_ON_Y - 2) {
         ++yy;
         ++height;
         visitedTiles[xx][yy] = true;
      }

      merged.height = height;
   }

   merged.neighbourAreas[0] = AREA_NAN;
   merged.neighbourAreas[1] = AREA_NAN;
   merged.neighbourAreas[2] = AREA_NAN;
   merged.neighbourAreas[3] = AREA_NAN;
   mergedTiles.push_back(merged);
}

map_bbox_t
C_Map::mergeTiles(void)
{
   int x, y, wallTiles = 0, wallTiles2;
   map_bbox_t bbox = {TILES_ON_X + 1, TILES_ON_Y + 1, -1, -1};

   /// Erase vector
   if(mergedTiles.size())
      mergedTiles.clear();

   /// initialize
   bool **visitedTiles = new bool *[TILES_ON_X];
   for(x = 0; x < TILES_ON_X; x++) {
      visitedTiles[x] = new bool[TILES_ON_Y];
      memset((void *)visitedTiles[x], false, sizeof(bool) * TILES_ON_Y);
   }

   /// Count wall tiles
   /// Not necessary to count wall tiles. Just to print some statistics
   for(x = 0; x < TILES_ON_X; x++) {
      for(y = 0; y < TILES_ON_Y; y++) {
         if(tiles[x][y].getType() == TILE_WALL) wallTiles++;
      }
   }

   /// Scan all tiles
   /// Each time a wall tile is found scan for more wall tiles and try to
   /// merge them into rows or columns
   for(x = 0; x < TILES_ON_X; x++) {
      for(y = 0; y < TILES_ON_Y; y++) {
         if(tiles[x][y].getType() == TILE_WALL && visitedTiles[x][y] == false)
            firstPass(x, y, visitedTiles);

         /// While scanning all map tiles, it is a good oportunity
         /// to calculate map's bounding box
         if(tiles[x][y].getType() == TILE_WALL) {
            if(x > bbox.maxX) bbox.maxX = x;
            if(x < bbox.minX) bbox.minX = x;

            if(y > bbox.maxY) bbox.maxY = y;
            if(y < bbox.minY) bbox.minY = y;
         }
      }
   }

   wallTiles2 = mergedTiles.size();

   /// Print merged tiles after 1st pass
//   for(x = 0; x < mergedTiles.size(); ++x)
//      printf("%d: (%d, %d): %d x %d\n", x, mergedTiles[x].x, mergedTiles[x].y, mergedTiles[x].width, mergedTiles[x].height);

   /// Try to merge the rows and columns into blocks
   secondPass();

//   /// Print merged tiles after 2nd pass
//   printf("\n\n");
//   for(x = 0; x < mergedTiles.size(); ++x)
//      printf("%d: (%d, %d): %d x %d\n", x, mergedTiles[x].x, mergedTiles[x].y, mergedTiles[x].width, mergedTiles[x].height);


   /// Find merged tiles' neighbours

   printf("\n*****\n");
   printf("Map merged:\n");
   printf("\tInitial wall tiles: %d\n", wallTiles);
   printf("\tWall tiles after 1st pass: %d\n", wallTiles2);
   printf("\tWall tiles after 2nd pass: %lu\n", mergedTiles.size());
   printf("*****\n");

   for(x = 0; x < TILES_ON_X; x++)
      delete[] visitedTiles[x];
   delete[] visitedTiles;

   return bbox;
}

void
C_Map::floodFill(tile *startTile, areaTypes_t area)
{
   tile *tmpTile;
   int x, y;

   stack<tile *> tilesToExamine;
   tilesToExamine.push(startTile);
   startTile->setArea(area);

   while(!tilesToExamine.empty()) {
      tmpTile = tilesToExamine.top();
      tilesToExamine.pop();

      x = tmpTile->x;
      y = tmpTile->y;

//      printf("(%d %d)\n", x, y);

      assert(x >= 0 && x < TILES_ON_X);
      assert(y >= 0 && y < TILES_ON_Y);

      /// Add more tiles
      if(x < TILES_ON_X - 1) {
         if(tiles[x + 1][y].getArea() == AREA_NAN && tiles[x + 1][y].getType() != TILE_WALL) {
            tilesToExamine.push(&tiles[x + 1][y]);
            tiles[x + 1][y].setArea(area);
         }
      }

      if(x > 0) {
         if(tiles[x - 1][y].getArea() == AREA_NAN && tiles[x - 1][y].getType() != TILE_WALL) {
            tilesToExamine.push(&tiles[x - 1][y]);
            tiles[x - 1][y].setArea(area);
         }
      }

      if(y < TILES_ON_Y - 1) {
         if(tiles[x][y + 1].getArea() == AREA_NAN && tiles[x][y + 1].getType() != TILE_WALL) {
            tilesToExamine.push(&tiles[x][y + 1]);
            tiles[x][y + 1].setArea(area);
         }
      }

      if(y > 0) {
         if(tiles[x][y - 1].getArea() == AREA_NAN && tiles[x][y - 1].getType() != TILE_WALL) {
            tilesToExamine.push(&tiles[x][y - 1]);
            tiles[x][y - 1].setArea(area);
         }
      }
   }
}

void
C_Map::divideAreas(void)
{
   bool found = false;
   int x, y;

   /// Detect tiles that are 100% walkable. Hopefully these will be tha map start tiles
   /// (staircases, doors, or teleporters for remote map areas)
   for(x = 0; x < TILES_ON_X && !found; ++x) {
      for(y = 0; y < TILES_ON_Y; ++y) {
         if(tiles[x][y].getType() == TILE_4) {
            found = true;
            --x;
            break;
         }
      }
   }

   assert(found);

   /// After finding the tile perform a flood fill on it
   printf("Start tile found at (%d, %d)\n", x, y);
   floodFill(&tiles[x][y], AREA_WALKABLE);

//   printf("\n*****\nWalkable tiles:\n");
//   for(x = 0; x < TILES_ON_X; ++x) {
//      for(y = 0; y < TILES_ON_Y; ++y) {
//         if(tiles[x][y].getArea() == AREA_WALKABLE)
//            printf("   (%d, %d)\n", x, y);
//      }
//   }
//   printf("*****");

   /// Find one tile that is 100% a AREA_VOID area and flood fill
   found = false;
   for(x = 0; x < TILES_ON_X && !found; ++x) {
      for(y = 0; y < TILES_ON_Y; ++y) {
         if(tiles[x][y].getType() == TILE_0 && tiles[x][y].getArea() == AREA_NAN) {
            found = true;
            --x;
            break;
         }
      }
   }

   assert(found);

   /// After finding the tile perform a flood fill on it
   printf("Found one AREA_VOID tile at (%d, %d)\n", x, y);
   floodFill(&tiles[x][y], AREA_VOID);

//   printf("\n*****\nVoid tiles:\n");
//   for(x = 0; x < TILES_ON_X; ++x) {
//      for(y = 0; y < TILES_ON_Y; ++y) {
//         if(tiles[x][y].getArea() == AREA_VOID)
//            printf("   (%d, %d)\n", x, y);
//      }
//   }
//   printf("*****");
}

/**
 * Swipes a merged tile's wall all neighbour tiles.
 * If all tiles are AREA_VOID then this wall is considered to face outside the map
 * and AREA_VOID is returned, else if at least 1 WALLKABLE tile is found
 * the wall is considered to be facing inside the map
 */
areaTypes_t
C_Map::detectAreaAcrossWall(mergedTile_t *tile, int neighbour)
{
   int x = 0, y = 0;
   int sx = tile->x;
   int sy = tile->y;
   int width = tile->width;
   int height = tile->height;
   areaTypes_t area = AREA_VOID;

   switch(neighbour) {
   case NEIGHBOUR_ABOVE:
      if(width > 1) {
         while(x < width) {
            if(tiles[sx + x][sy + height].getArea() == AREA_WALKABLE) {
               area = AREA_WALKABLE;
               goto done;
            }
            ++x;
         }
      } else {
         area = tiles[sx][sy + height].getArea();
         goto done;
      }
      break;

   case NEIGHBOUR_BELOW:
      if(width > 1) {
         while(x < width) {
            if(tiles[sx + x][sy - 1].getArea() == AREA_WALKABLE) {
               area = AREA_WALKABLE;
               goto done;
            }
            ++x;
         }
      } else {
         area = tiles[sx][sy - 1].getArea();
         goto done;
      }
      break;

   case NEIGHBOUR_LEFT:
      if(height > 1) {
         while(y < height) {
            if(tiles[sx - 1][sy + y].getArea() == AREA_WALKABLE) {
               area = AREA_WALKABLE;
               goto done;
            }
            ++y;
         }
      } else {
         area = tiles[sx - 1][sy].getArea();
         goto done;
      }
      break;

   case NEIGHBOUR_RIGHT:
      if(height > 1) {
         while(y < height) {
            if(tiles[sx + width][sy + y].getArea() == AREA_WALKABLE) {
               area = AREA_WALKABLE;
               goto done;
            }
            ++y;
         }
      } else {
         area = tiles[sx + width][sy].getArea();
         goto done;
      }
      break;
   }

   done:
   return area != AREA_NAN ? area : AREA_VOID;
}

/**
 * For each merged tile, detects it's neighbour areas.
 * According to the areas found it also counts the number of polygons
 */
int
C_Map::setNeighbourAreas(void)
{
   assert(mergedTiles.size());

   /// Initial estimation
   int nPolys = mergedTiles.size() * 5;
   int x, y;

   assert(nPolys);

   /// Loop through merged tiles and set their neighbour areas
   for(unsigned int i = 0; i < mergedTiles.size(); i++) {
      x = mergedTiles[i].x;
      y = mergedTiles[i].y;
      if(x == 1 && y == 12)
         printf("wtf\n");

      /// Check left neigbhour
      if(x > 0) {
         mergedTiles[i].neighbourAreas[NEIGHBOUR_LEFT] = detectAreaAcrossWall(&mergedTiles[i], NEIGHBOUR_LEFT);
      } else {
         mergedTiles[i].neighbourAreas[NEIGHBOUR_LEFT] = AREA_VOID;
      }
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_LEFT] == AREA_VOID ||
         mergedTiles[i].neighbourAreas[NEIGHBOUR_LEFT] == AREA_WALL)
         --nPolys;

      /// Check right neigbhour
      if(x + mergedTiles[i].width < TILES_ON_X - 1) {
         mergedTiles[i].neighbourAreas[NEIGHBOUR_RIGHT] = detectAreaAcrossWall(&mergedTiles[i], NEIGHBOUR_RIGHT);
      } else {
         mergedTiles[i].neighbourAreas[NEIGHBOUR_RIGHT] = AREA_VOID;
      }
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_RIGHT] == AREA_VOID ||
         mergedTiles[i].neighbourAreas[NEIGHBOUR_RIGHT] == AREA_WALL)
         --nPolys;

      /// Check neigbhour below
      if(y > 0) {
         mergedTiles[i].neighbourAreas[NEIGHBOUR_BELOW] = detectAreaAcrossWall(&mergedTiles[i], NEIGHBOUR_BELOW);
      } else {
         mergedTiles[i].neighbourAreas[NEIGHBOUR_BELOW] = AREA_VOID;
      }
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_BELOW] == AREA_VOID ||
         mergedTiles[i].neighbourAreas[NEIGHBOUR_BELOW] == AREA_WALL)
         --nPolys;

      /// Check neigbhour above
      if(y + mergedTiles[i].height < TILES_ON_Y - 1) {
         mergedTiles[i].neighbourAreas[NEIGHBOUR_ABOVE] = detectAreaAcrossWall(&mergedTiles[i], NEIGHBOUR_ABOVE);
      } else {
         mergedTiles[i].neighbourAreas[NEIGHBOUR_ABOVE] = AREA_VOID;
      }
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_ABOVE] == AREA_VOID ||
         mergedTiles[i].neighbourAreas[NEIGHBOUR_ABOVE] == AREA_WALL)
         --nPolys;

      /// If at least one neighbour area is AREA_VOID then the roof is omited
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_ABOVE] == AREA_VOID ||
         mergedTiles[i].neighbourAreas[NEIGHBOUR_BELOW] == AREA_VOID ||
         mergedTiles[i].neighbourAreas[NEIGHBOUR_LEFT]  == AREA_VOID ||
         mergedTiles[i].neighbourAreas[NEIGHBOUR_RIGHT] == AREA_VOID)
         --nPolys;
   }

   return nPolys;
}

void
C_Map::saveBspGeometryToFile(const char *filename)
{
   const int verticesPerPoly = 4;
   const int nBrushes = 1;
   /// Shadow global variable.
   const float tileSize = 20.0f;
   int nPolys, polysWriten = 0;
   map_bbox_t bbox;

   C_Vertex v1, v2, v3, v0;
   C_TexCoord center;
   C_TexCoord halfDims;

   /// Merge tiles into columns, rows or blocks of wall tiles.
   bbox = mergeTiles();

   /// Divide map into areas, walkable and void
   divideAreas();

   /// Detect merged tiles' surrounding areas and count the final poly count
   /// (after removing walls that face outside the map etc)
   nPolys = setNeighbourAreas();

   /// Print final merged tiles
   printf("\n\n");
   for(int x = 0; x < mergedTiles.size(); ++x) {
      printf("%d: (%d, %d): %d x %d\n", x, mergedTiles[x].x, mergedTiles[x].y, mergedTiles[x].width, mergedTiles[x].height);
      printf("   %d %d %d %d\n", mergedTiles[x].neighbourAreas[NEIGHBOUR_LEFT], mergedTiles[x].neighbourAreas[NEIGHBOUR_ABOVE],
                                 mergedTiles[x].neighbourAreas[NEIGHBOUR_RIGHT], mergedTiles[x].neighbourAreas[NEIGHBOUR_BELOW]);
   }

   printf("\n*****\n");
   printf("Writing \"%s\"...\n", filename);
   printf("   wall tiles: %lu total polygons: %d\n", mergedTiles.size(), nPolys);

   FILE *fp = fopen(filename, "w");
   assert(fp);

   /// Write number of polys
   fwrite(&nPolys, sizeof(int), 1, fp);
   /// Write number of brushes
   fwrite(&nBrushes, sizeof(int),1, fp);
   /// Write again number of polys
   /// This is the number of polys in brush and since there is only one brush
   /// all the polys goes to it
   fwrite(&nPolys, sizeof(int), 1, fp);

   /// Generate wall geometry
   for(unsigned int i = 0; i < mergedTiles.size(); i++) {
      /// For merged tile there are 5 polys
      /// 4 walls and 1 roof

      /// Calculate vertices
      halfDims.u = mergedTiles[i].width * tileSize / 2.0f;
      halfDims.v = mergedTiles[i].height * tileSize / 2.0f;
      center.u = /*minX + maxX - */(mergedTiles[i].x * tileSize + halfDims.u);
      center.v = /*minY + maxY - */(mergedTiles[i].y * tileSize + halfDims.v);

      /// Left wall
      assert(mergedTiles[i].neighbourAreas[NEIGHBOUR_LEFT] != AREA_NAN);
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_LEFT] == AREA_WALKABLE) {
         /// Write number of vertices
         fwrite(&verticesPerPoly, sizeof(int), 1, fp);
         v0.z = center.u - halfDims.u;
         v0.y = -tileSize / 2.0f;
         v0.x = center.v + halfDims.v;

         v1.z = center.u - halfDims.u;
         v1.y = tileSize / 2.0f;
         v1.x = center.v + halfDims.v;

         v2.z = center.u - halfDims.u;
         v2.y = tileSize / 2.0f;
         v2.x = center.v - halfDims.v;

         v3.z = center.u - halfDims.u;
         v3.y = -tileSize / 2.0f;
         v3.x = center.v - halfDims.v;

         fwrite(&v0, sizeof(float), 3, fp);
         fwrite(&v1, sizeof(float), 3, fp);
         fwrite(&v2, sizeof(float), 3, fp);
         fwrite(&v3, sizeof(float), 3, fp);
         ++polysWriten;
      }

      /// Bottom wall
      assert(mergedTiles[i].neighbourAreas[NEIGHBOUR_ABOVE] != AREA_NAN);
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_ABOVE] == AREA_WALKABLE) {
         /// Write number of vertices
         fwrite(&verticesPerPoly, sizeof(int), 1, fp);
         v0.z = center.u - halfDims.u;
         v0.y = -tileSize / 2.0f;
         v0.x = center.v + halfDims.v;

         v1.z = center.u + halfDims.u;
         v1.y = -tileSize / 2.0f;
         v1.x = center.v + halfDims.v;

         v2.z = center.u + halfDims.u;
         v2.y = tileSize / 2.0f;
         v2.x = center.v + halfDims.v;

         v3.z = center.u - halfDims.u;
         v3.y = tileSize / 2.0f;
         v3.x = center.v + halfDims.v;

         fwrite(&v0, sizeof(float), 3, fp);
         fwrite(&v1, sizeof(float), 3, fp);
         fwrite(&v2, sizeof(float), 3, fp);
         fwrite(&v3, sizeof(float), 3, fp);
         ++polysWriten;
      }

      /// Right wall
      assert(mergedTiles[i].neighbourAreas[NEIGHBOUR_RIGHT] != AREA_NAN);
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_RIGHT] == AREA_WALKABLE) {
         /// Write number of vertices
         fwrite(&verticesPerPoly, sizeof(int), 1, fp);
         v0.z = center.u + halfDims.u;
         v0.y = -tileSize / 2.0f;
         v0.x = center.v + halfDims.v;

         v1.z = center.u + halfDims.u;
         v1.y = -tileSize / 2.0f;
         v1.x = center.v - halfDims.v;

         v2.z = center.u + halfDims.u;
         v2.y = tileSize / 2.0f;
         v2.x = center.v - halfDims.v;

         v3.z = center.u + halfDims.u;
         v3.y = tileSize / 2.0f;
         v3.x = center.v + halfDims.v;

         fwrite(&v0, sizeof(float), 3, fp);
         fwrite(&v1, sizeof(float), 3, fp);
         fwrite(&v2, sizeof(float), 3, fp);
         fwrite(&v3, sizeof(float), 3, fp);
         ++polysWriten;
      }

      /// Back wall
      assert(mergedTiles[i].neighbourAreas[NEIGHBOUR_BELOW] != AREA_NAN);
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_BELOW] == AREA_WALKABLE) {
         /// Write number of vertices
         fwrite(&verticesPerPoly, sizeof(int), 1, fp);
         v0.z = center.u + halfDims.u;
         v0.y = -tileSize / 2.0f;
         v0.x = center.v - halfDims.v;

         v1.z = center.u - halfDims.u;
         v1.y = -tileSize / 2.0f;
         v1.x = center.v - halfDims.v;

         v2.z = center.u - halfDims.u;
         v2.y = tileSize / 2.0f;
         v2.x = center.v - halfDims.v;

         v3.z = center.u + halfDims.u;
         v3.y = tileSize / 2.0f;
         v3.x = center.v - halfDims.v;

         fwrite(&v0, sizeof(float), 3, fp);
         fwrite(&v1, sizeof(float), 3, fp);
         fwrite(&v2, sizeof(float), 3, fp);
         fwrite(&v3, sizeof(float), 3, fp);
         ++polysWriten;
      }

      /// Top/roof wall
      /// Write number of vertices
      /// If at least one neighbour area is AREA_VOID then the roof is omited
      if(mergedTiles[i].neighbourAreas[NEIGHBOUR_ABOVE] != AREA_VOID &&
         mergedTiles[i].neighbourAreas[NEIGHBOUR_BELOW] != AREA_VOID &&
         mergedTiles[i].neighbourAreas[NEIGHBOUR_LEFT]  != AREA_VOID &&
         mergedTiles[i].neighbourAreas[NEIGHBOUR_RIGHT] != AREA_VOID) {
         fwrite(&verticesPerPoly, sizeof(int), 1, fp);
         v0.z = center.u + halfDims.u;
         v0.y = tileSize / 2.0f;
         v0.x = center.v + halfDims.v;

         v1.z = center.u + halfDims.u;
         v1.y = tileSize / 2.0f;
         v1.x = center.v - halfDims.v;

         v2.z = center.u - halfDims.u;
         v2.y = tileSize / 2.0f;
         v2.x = center.v - halfDims.v;

         v3.z = center.u - halfDims.u;
         v3.y = tileSize / 2.0f;
         v3.x = center.v + halfDims.v;

         fwrite(&v0, sizeof(float), 3, fp);
         fwrite(&v1, sizeof(float), 3, fp);
         fwrite(&v2, sizeof(float), 3, fp);
         fwrite(&v3, sizeof(float), 3, fp);
         ++polysWriten;
      }
   }

   assert(polysWriten == nPolys);

   fclose(fp);
   printf("Done\n");
   printf("*****\n");
}

void
C_Map::saveMapAreasToFile(const char *filename)
{
	int nTiles = 0, x, y;
	int nWalls = 0, nWalkables = 0;

	FILE *fd = fopen(filename, "w");

	if(!fd) {
	   assert(0);
		return;
   }

   /// Count all the NON VOID tiles
	for(x = 0; x < TILES_ON_X; x++) {
		for(y = 0; y < TILES_ON_Y; y++) {
			if(tiles[x][y].getArea() != AREA_VOID) {
				++nTiles;
				if(tiles[x][y].getType() == TILE_WALL) {
				   assert(tiles[x][y].getArea() == AREA_WALL);
				   ++nWalls;
            } else if(tiles[x][y].getType() == TILE_0) {
               ++nWalkables;
            }
         }
		}
	}

	printf("\n*****\n");
   printf("Writing \"%s\"...\n", filename);
   printf("\t%d non void tiles\n", nTiles);
   printf("\t%d walls %d walkables\n", nWalls, nWalkables);

	fprintf(fd, "%d\n", TILES_ON_X);			/// Save x dimension
	fprintf(fd, "%d\n", TILES_ON_Y);			/// Save y dimension

	for(x = 0; x < TILES_ON_X; ++x) {
		for(y = 0; y < TILES_ON_Y; ++y) {
         fprintf(fd, "%d %d %d %d\n", x, y, tiles[x][y].getType(), tiles[x][y].getArea());
		}
   }

   fclose(fd);

   printf("Done\n");
   printf("*****\n");
}
