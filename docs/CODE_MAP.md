# MetroCraft code map

*Md Rejaul Korim Sadi (231-115-349) and Kazi Abu Jafor Arif (231-115-068) · CSE 402 Computer Graphics & Image Processing Lab*

Use this page to jump straight to the code behind anything shown in the game. Line numbers are for version 1.0.

**Finding a line in Code::Blocks:** open the file from the **Projects** panel on the left (MetroCraft, then Sources, then `lib` or `src`), press **Ctrl+G** and type the line number. **Ctrl+Shift+G** jumps to a function by name.

## The folders

```
MetroCraft/
├── src/
│   └── main.cpp        the program: OpenGL setup, camera, draw order, loop, mouse     (show this first)
├── lib/
│   ├── campus.cpp      the campus: gate, main building, roof sign, Centre For Excellence, developer board, field, track, pond, flags, trees
│   ├── world.cpp       every block in the world: terrain and hills, cube faces, plants, chunks, ray cast
│   ├── effects.cpp     the outline box, flying particles, the block in your hand
│   ├── sky.cpp         sun, moon, stars, clouds, sky colour and fog
│   ├── hud.cpp         flat 2D shapes: crosshair, hotbar, block icons, logo, text, title screen
│   ├── textures.cpp    the pictures on the blocks, painted pixel by pixel
│   ├── player.cpp      walking, gravity and collision (no drawing)
│   ├── game.cpp        game state, keys, breaking and placing (no drawing)
│   ├── blocks.h        the list of block types and which picture each face uses
│   └── noise.h         the random numbers behind the hills and caves
├── tests/              checks that run without a window
└── deps/               freeglut headers and import library for the Windows build
```

## Where each shape is drawn

| Shape on screen | OpenGL primitive | File and line | Function | How to explain it |
|---|---|---|---|---|
| **A block (cube)** | `GL_TRIANGLES`, 2 per face | `lib/world.cpp` 199 | `emitFace()` | A cube has 6 faces. For each face, the 4 corners come from the `CORNER` table (line 159) and are listed counter-clockwise from outside. Each face is split into 2 triangles. |
| The cube's corner table | data | `lib/world.cpp` 159 | `CORNER[6][4][3]` | For each of the 6 directions (+Y, −Y, +X, −X, +Z, −Z), the 4 corner positions of a unit cube |
| Which faces get drawn | logic | `lib/world.cpp` 176 | `faceVisible()` | A face is drawn only if the block next to it is air or see-through. Buried faces are skipped. |
| A whole chunk of blocks | `glNewList` + `GL_TRIANGLES` | `lib/world.cpp` 246 | `buildChunk()` | Loops over every block in a 16 × 16 column, emits its visible faces and records them in a display list |
| Flowers and tall grass | `GL_TRIANGLES` | `lib/world.cpp` 226 | `emitPlant()` | Two flat pictures crossing in an X, each drawn from both sides |
| Hills around the campus | blocks | `lib/world.cpp` 52 | `generate()` | Height is flat on the campus and rises with noise outside it, using `campusEdge()` |
| **Main academic building** | blocks | `lib/campus.cpp` 112 | `mainBuilding()` | A concrete slab per storey. Along the front, the x position decides the block: white columns at the three joins, solid brick on the left, windows with a mullion every 4th block in the middle, a brick row and two window rows on the right, then a white MU banner on top of that. Stairs and the roof sunshade are added last. |
| **METROPOLITAN UNIVERSITY roof sign** | blocks | `lib/campus.cpp` 177 | `universitySign()` | A white board on three legs; `writeText()` (line 84) turns each letter of the 5-row font (`letterRow()`, line 48) into red or black blocks |
| **DEVELOPED BY SADI AND ARIF board** | blocks | `lib/campus.cpp` 189 | `developerSign()` | The same `writeText()`, stepping along −z instead of +x so the board faces the field from the west side |
| **Centre For Excellence** | blocks | `lib/campus.cpp` 203 | `centreForExcellence()` | Slabs and glass-and-mullion walls for two floors, then a hip roof: 8 layers of roof tiles, each 2 blocks smaller on every side and 1 block higher. On the front: a glass box, the name wall in the slim 3-wide font (`narrowRow()`, line 31), the balloon arch and a hanging glass box. |
| **METROPOLITAN UNIVERSITY gate** | blocks | `lib/campus.cpp` 276 | `entranceGate()` | A concrete beam on two pillars, with the same letters in black and red |
| **Running track and field** | blocks | `lib/campus.cpp` 309 | `field()` | A block is track if its distance from the centre line is between 16 and 20, which makes a stadium shape. White lines, a centre circle and goals are drawn with concrete. |
| Pond and its ghat | blocks | `lib/campus.cpp` 329 | `pond()` | A box of water right of the road, with three concrete steps going down into it |
| Flags (Bangladesh first) | blocks | `lib/campus.cpp` 294 | `flags()` | Concrete poles with painted blocks on top; green with a red centre for Bangladesh |
| White building, gazebo | blocks | `lib/campus.cpp` 337, 361 | `whiteBuilding()`, `gazebo()` | Walls with window rows and red trim; four posts under a stepped green roof |
| Front lawn | blocks | `lib/campus.cpp` 382 | `frontLawn()` | Flower rows, oaks on the left and palms on the right in front of the building |
| Palm trees | blocks | `lib/campus.cpp` 97 | `palm()` | A log trunk with leaf arms in 8 directions, the long ones drooping at the end |
| Oak trees | blocks | `lib/world.cpp` 123 | `placeTree()` | A column of logs with a rounded blob of leaves on top |
| **Outline around the target block** | `GL_LINES` | `lib/effects.cpp` 8 | `drawSelection()` | The 12 edges of a box drawn as lines, slightly bigger than the block |
| **Broken-block particles** | `GL_QUADS` | `lib/effects.cpp` 74 | `drawParticles()` | Small squares that always face the camera (billboards). The camera's right and up directions are read from the modelview matrix. |
| **Block in your hand** | `GL_QUADS` | `lib/effects.cpp` 108 | `drawHand()` | 3 faces of a cube (top, front, side) with their own projection, moved with `glTranslatef`, `glRotatef` and `glScalef` |
| **Sun and moon** | `GL_QUADS` | `lib/sky.cpp` 51 | `drawSky()` | One square, 120 units away, moved with the player so it looks infinitely far |
| **Stars** | `GL_POINTS` | `lib/sky.cpp` 33 | `drawStars()` | 600 points spread over a half-sphere, brightness changing with `sin` |
| **Clouds** | `GL_QUADS` | `lib/sky.cpp` 73 | `drawClouds()` | Flat 8 × 8 squares high in the sky, picked by noise and sliding slowly |
| **Rectangles: crosshair, panels, hotbar slots** | `GL_QUADS` | `lib/hud.cpp` 76 | `rect()` | A square from 4 corners in 2D pixel coordinates |
| Fade at top and bottom of the title screen | `GL_QUADS` | `lib/hud.cpp` 82 | `gradientRect()` | A rectangle with different colours at the top and bottom corners, which OpenGL blends smoothly |
| **Isometric block icons in the hotbar** | `GL_QUADS` | `lib/hud.cpp` 90 | `blockIcon()` | 3 textured diamonds (top, left, right) placed to look like a 3D cube in 2D |
| **The METROCRAFT logo** | `GL_QUADS` | `lib/hud.cpp` 156 | `drawLogo()` | Each letter is a 5 × 7 grid (`GLYPHS`, line 137). Every filled cell is a small textured square with a shadow. |
| Crosshair | `GL_QUADS` | `lib/hud.cpp` 300 | `drawHUD()` | Two thin rectangles crossing in the middle of the screen |
| Big text (Sadi & Arif, splash line) | stroke font | `lib/hud.cpp` 58 | `strokeText()` | freeglut's line-drawn letters, scaled with `glScalef` and rotated with `glRotatef` |
| Small text (HUD) | bitmap font | `lib/hud.cpp` 23 | `text()` | `glutBitmapString` at a raster position, drawn twice for a shadow |

## Where each graphics idea lives

| Idea | File and line | What to point at |
|---|---|---|
| OpenGL settings | `src/main.cpp` 4 | `initGL()`: depth test, culling, alpha test, blending, fog |
| Perspective projection | `src/main.cpp` 15 | `gluPerspective` in `setCamera()` |
| Viewing transform (camera) | `src/main.cpp` 15 | `glRotatef` for pitch and yaw, `glTranslatef` by minus the eye |
| Draw order of a frame | `src/main.cpp` 28 | `display()`: solid first, water last, 2D on top, then `glutSwapBuffers` |
| Animation loop | `src/main.cpp` 51 | `timer()` every 16 ms |
| Viewport | `src/main.cpp` 57 | `reshape()` |
| Mouse look | `src/main.cpp` 64 | `mouseMove()` |
| Picking a block | `src/main.cpp` 72, `lib/world.cpp` 304 | `mouseButton()` calls `World::raycast()` |
| Translate, rotate, scale on one object | `lib/effects.cpp` 108 | `drawHand()` |
| 2D overlay (orthographic) | `lib/hud.cpp` 10 | `ortho()` with `gluOrtho2D` |
| Texture creation | `lib/textures.cpp` 188 | `buildAtlas()`: `glGenTextures`, `GL_NEAREST`, `glTexImage2D` |
| Texture coordinates | `lib/textures.cpp` 219 | `tileUV()` finds one picture inside the atlas |
| Texture animation | `lib/textures.cpp` 203 | `animateWater()` with `glTexSubImage2D` |
| Shading by face direction | `lib/world.cpp` 151 | `SHADE[6]`: top bright, sides darker, bottom darkest |
| Corner shadows (ambient occlusion) | `lib/world.cpp` 184 | `cornerAO()` |
| Display lists | `lib/world.cpp` 246, 293 | `glNewList` in `buildChunk()`, `glCallList` in `drawOpaque()` |
| Sky colour and fog | `lib/sky.cpp` 21 | `clearSky()` |
| Collision | `lib/player.cpp` 15, 35 | `collides()` and `moveAxis()` |

## If she asks...

**"Show me how you draw a cube."**
Open `lib/world.cpp` at line 159 for the `CORNER` table, then line 199 for `emitFace()`. Say: "A cube is 6 faces. Each face has 4 corners from this table, sent as 2 triangles with `glVertex3f`. `glTexCoord2f` puts the texture on it, and `glColor3f` sets how bright it is."

**"How did you make the campus building?"**
`lib/campus.cpp` line 112, `mainBuilding()`. Say: "It's all cubes, copied from a photo of the real building. For each storey we place a concrete floor slab, then walk along the front: where x is on a join it's a white column, on the left it's brick, in the middle it's windows, on the right it's brick and windows, and a white banner goes over that. The same cube code from `emitFace()` draws all of it."

**"How did you write the university name in blocks?"**
`lib/campus.cpp` line 48, `letterRow()`, and line 84, `writeText()`. Say: "Each letter is 5 rows of 1s and dots. For every 1 we place one block. The step (dx, dz) decides which way the text runs, so the same function writes the roof sign, the gate and the side board."

**"Why don't you draw every face?"**
`lib/world.cpp` line 176, `faceVisible()`. Faces touching another solid block can never be seen, so they're skipped. That's most of the faces in the world.

**"Where is a 2D shape?"**
`lib/hud.cpp` line 76, `rect()`, and line 90, `blockIcon()`. After `ortho()` switches to `gluOrtho2D`, one unit is one pixel, so a rectangle is just 4 `glVertex2f` corners.

**"Where do you use lines and points?"**
Lines: `lib/effects.cpp` line 8, the outline box with `GL_LINES`. Points: `lib/sky.cpp` line 33, the stars with `GL_POINTS`.

**"Where are transformations on an object?"**
`lib/effects.cpp` line 108, `drawHand()`: `glTranslatef` moves the cube to the lower right, `glRotatef` turns it and swings it, `glScalef` makes it small. `glPushMatrix` / `glPopMatrix` around the sun in `lib/sky.cpp` line 51 is another example.

**"Where is the camera?"**
`src/main.cpp` line 15, `setCamera()`.

**"How is the texture made?"**
`lib/textures.cpp` line 51, `paint()`, sets each 16 × 16 picture pixel by pixel. `buildAtlas()` at line 188 uploads them as one texture.
