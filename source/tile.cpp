#include "tile.h"

tile::tile(void)
{
   x = 0;
   y = 0;
   parameter = "";
   hasParameter = false;
   area = AREA_NAN;
   type = TILE_0;
}

tile::tile(int _x, int _y)
{
   x = _x;
   y = _y;
   parameter = "";
   hasParameter = false;
   area = AREA_NAN;
   type = TILE_0;
}

tile::~tile(void)
{
   parameter.clear();
}

void tile::setParameter(string param)
{
   /// DO NOT ALLOW digits as parameters. It will mess up with the loader.
	if(param.size ()) {
		if(isdigit(param[0])) {
			hasParameter = false;
			this->parameter.clear();
			return;
		}

		hasParameter = true;
		this->parameter = param;

      /// Remove new line characters
		for(unsigned int i = 0; i < parameter.size(); i++) {
			if(parameter[i] == '\n')
				parameter.erase(i, 1);
		}
	} else {
		hasParameter = false;
		this->parameter.clear ();
	}
}

void tile::draw(float tileSize)
{
	if(this->type == 0 && this->hasParameter == false)
		return;

	if(this->type)
		glColor3fv(selections[this->type-1]);
	else
		glColor3f(0.9f, 0.9f, 0.9f);

   /// Tiles with no parameters are drawn normally (lines)
	if(this->hasParameter == false) {
		glBegin(GL_LINES);
			glVertex2f(x * tileSize		, y * tileSize );
			glVertex2f((x+1) * tileSize, y * tileSize );

			glVertex2f((x+1) * tileSize, y * tileSize );
			glVertex2f((x+1) * tileSize, (y+1) * tileSize );

			glVertex2f((x+1) * tileSize, (y+1) * tileSize );
			glVertex2f(x * tileSize		, (y+1) * tileSize );

			glVertex2f(x * tileSize		, (y+1) * tileSize );
			glVertex2f(x * tileSize		, y * tileSize );

			glVertex2f((x+1) * tileSize, y * tileSize );
			glVertex2f(x * tileSize		, (y+1) * tileSize );
		glEnd();
	} else { /// The rest are drawn as solid quads
		glBegin(GL_QUADS);
			glVertex2f(x * tileSize		, y * tileSize );
			glVertex2f((x+1) * tileSize, y * tileSize );
			glVertex2f((x+1) * tileSize, (y+1) * tileSize );
			glVertex2f(x * tileSize		, (y+1) * tileSize );
		glEnd();
	}
}
