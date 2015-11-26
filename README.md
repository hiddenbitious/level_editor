level_editor
============

Create 2D maps for old school dungeon crawling / rogue like games or tile based games.

Controls:
- Left mouse button marks a tile.
- Right mouse button deletes a tile.
- Middle mouse button choose between tile types.
- Ctrl + left mouse button enters a comment to a tile.
- Ctrl + s saves map.

Uses OpenGL + glut for rendering the map and the text. For now the map size is fixed at 80 x 50 tiles.

The editor by default loads the map.txt found in the same folder. Map filename can be passed in the command line argument.
The editor also generates 3D geometry for the map based on a fixed tile size. Various optimizations are applied in order to discard redundant geometry.

When a map is saved (ctrl + s) a name will be requested. The map is saved in ASCII format. A binary file with the same name and a .bsp suffix is also saved (binary file).
It contains the generated map's geometry.

These two files are loaded by the from_scratch engine (https://github.com/hiddenbitious/from_scratch).

**Screenshots**: (https://www.dropbox.com/s/jcciq9mlkigqu87/editor2.png?dl=0) (https://www.dropbox.com/s/1r8yrbr949erbqr/editor.png?dl=0)
