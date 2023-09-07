# (TODO: your game's title)

Author: (TODO: your name)

Design: (TODO: In two sentences or fewer, describe what is new and interesting about your game.)

Screen Shot:

![Screen Shot](screenshot.png)

How Your Asset Pipeline Works:

(TODO: describe the steps in your asset pipeline, from source files to tiles/backgrounds/whatever you upload to the PPU466.)

Loading character sprites: 
I have one character sprite [link](/sprites/smile.png)
I use the load_png function given to import the data from the PNG.
Every time I encounter a new color, I create a hash value for it using the RGBA values so that I know its index within a palette, then I add said color to that index of the palette. 
As I iterate over the PNG data pixel by pixel, I check the sprite color and set each bit of the tile based on the pixel color and the index of the palette table the color refers to. 
I use tile 32 to represent the sprites. 

Loading backgrounds: 
Background files are loaded in as .PNGs, their names and the order in which backgrounds / levels appear is hard coded into PlayMode.cpp. 
The rooms are not hard-coded, just the order in which they appear. 
In the files used to generate rooms, red pixels are solid, green pixels are hallways, blue pixels are starting points, and black pixels are ending points. 
If the pixel is red, it is registered as a "wall" and displayed by the PPU using a black tile. 
If it is red or blue, it is registered as a hallway and displayed with a white tile, and if it is black, it is registered as an end point and displayed with a blue tile. 
I implemented this part after a class discussion in which a student, Matei Budiu, showed that they had created 4x4 palette files to make loading colors easier. 
I also did this - the black, white and blue colors are loaded in using a 4x4 palette file with the available background colors to draw with.

How To Play:

Move the smiling character through white hallways towards the blue square. The levels loop forever, there is no way to win or exit. Stop when you feel better

This game was built with [NEST](NEST.md).

