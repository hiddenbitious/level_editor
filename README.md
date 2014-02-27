level_editor
============

Level / map editor for old school dungeon crawling games.

Create 2D maps for old school dungeon crawling / rogue like games or tile based games.

Controls:
- Left mouse button marks a tile.
- Right mouse button deletes a tile
- Middle mouse button choose between tile types
- Ctrl + left mouse button enters a comment to a tile
- Ctrl + s saves map

Uses OpenGL + glut for rendering the map and the text so i supposed it can be considered as cross platform. 
For now the map size is fixed at 80 x 50 tiles.

The editor is mainly designed to generate 3D geometry of the map so it can be used for a 3D fps 
rogue like / dungeon crawling game. Various optimizations are applied in order to discard redundant geometry.

TODO:
Use cegui (http://cegui.org.uk/) to create a rich user interface.
