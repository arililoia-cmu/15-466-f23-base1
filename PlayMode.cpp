#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>
#include "load_save_png.hpp"
#include "data_path.hpp"
#include <map>
#include <random>


// struct Rooms {
// };


// also keep track of which pixels are where on the screen
// void set_tile_uniform_color(int tile_index, int palette_index, int color_index){

// 	std::array< uint8_t, 8 > bit0;
// 	std::array< uint8_t, 8 > bit1;
// 	// code for filling an array with 255/0 taken from here: 
// 	// https://en.cppreference.com/w/cpp/algorithm/fill

// 	// fill in the tiles uniformly 
// 	if (color_index == 0){
// 		std::fill(bit0, bit0 + bit0.size(), 0);
// 		std::fill(bit1, bit1 + bit1.size(), 0);
// 	}
// 	else if (color_index == 1){
// 		std::fill(bit0, bit0 + bit0.size(), 0);
// 		std::fill(bit1, bit1 + bit1.size(), 255);
// 	}
// 	else if (color_index == 2){
// 		std::fill(bit0, bit0 + bit0.size(), 255);
// 		std::fill(bit1, bit1 + bit1.size(), 0);
// 	}
// 	else if (color_index == 3){
// 		std::fill(bit0, bit0 + bit0.size(), 255);
// 		std::fill(bit1, bit1 + bit1.size(), 255);
// 	}

// }





PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	

	glm::uvec2 frowny_size;
	std::vector< glm::u8vec4 > frowny_data;
	OriginLocation frowny_ol = OriginLocation::LowerLeftOrigin;
	std::string frowny_path = data_path("sprites/frog8x8.png");
	load_png(frowny_path, &frowny_size, &frowny_data, frowny_ol);

	std::map<uint32_t, int> map_colors_to_palette_indices;
	int palette_index = 0;
	uint32_t hash_value;
	//
	std::array< uint8_t, 8 > bit0; 
	std::array< uint8_t, 8 > bit1;

	// zero out data to be safe
	for (int i=0; i<8; i++){
		bit0[i] = 0;
		bit1[i] = 0;
	}

	for(int i=0; i<8; i++){
		for (int j=0; j<8; j++){
			
			// create a hash value for seen colors to map them to colors in palettes
			hash_value = (uint32_t)frowny_data.at((i*8)+j)[0] | ((uint32_t)frowny_data.at((i*8)+j)[1] << 8) | 
				((uint32_t)frowny_data.at((i*8)+j)[2] << 16) | ((uint32_t)frowny_data.at((i*8)+j)[3] << 24);
		

			// map colors to hash values to keep track of which colors are where in the palette
			if (!map_colors_to_palette_indices.count(hash_value)){
					map_colors_to_palette_indices[hash_value] = palette_index;
					// set the palette table index for the sprite to the correct color
					ppu.palette_table[7][palette_index] = glm::u8vec4(
						frowny_data.at((i*8)+j)[0],
						frowny_data.at((i*8)+j)[1],
						frowny_data.at((i*8)+j)[2],
						frowny_data.at((i*8)+j)[3]
					);
					palette_index++;
			}
			
			// set bits based on the color value 
			if (map_colors_to_palette_indices[hash_value] == 1){
				bit0[i] |= 1;
			}
			else if (map_colors_to_palette_indices[hash_value] == 2){
				bit1[i] |= 1;
			}
			else if (map_colors_to_palette_indices[hash_value] == 3){
				bit0[i] |= 1;
				bit1[i] |= 1;
			}
			
			// shift left - determine the next bit in the row
			bit0[i] <<= 1;
			bit1[i] <<= 1;
			
		}
	}

	// set the sprite bits to the bits generated using the asset pipeline
	ppu.tile_table[32].bit0 = bit0;
	ppu.tile_table[32].bit1 = bit1;


	
	// now working on the background
	// allocate bare minimum number of assets for maze - just 2 tiles.
	// first create bitplanes to use for each tile
	std::array<uint8_t, 8> zeros_bitplane = {0,0,0,0,0,0,0,0};
	std::array<uint8_t, 8> ones_bitplane = {1,1,1,1,1,1,1,1};

	// then we establish tile 0 and 1 as the tiles to use for 
	// black and white background squares, respectively
	ppu.tile_table[0].bit0 = zeros_bitplane;
	ppu.tile_table[0].bit1 = zeros_bitplane;

	ppu.tile_table[1].bit0 = zeros_bitplane;
	ppu.tile_table[0].bit1 = ones_bitplane;

	// then we read in color from background_palette
	// this idea was taken from a student in class - do not remember their name.
	glm::uvec2 bg_palette_size;
	std::vector< glm::u8vec4 > bg_palette_data;
	OriginLocation bg_palette_ol = OriginLocation::LowerLeftOrigin;
	std::string bg_palette_path = data_path("sprites/bg_palette.png");
	load_png(bg_palette_path, &bg_palette_size, &bg_palette_data, bg_palette_ol);

	// std::cout << (int)bg_palette_data.at(0)[0] << std::endl;
	// std::cout << (int)bg_palette_data.at(1)[0] << std::endl;
	// std::cout << (int)bg_palette_data.at(2)[0] << std::endl;
	

	// if time permits - put this code into a function
	// establish palette table 5 as the one we use for the background
	for(int i=0; i<4; i++){
		ppu.palette_table[5][i] = glm::u8vec4(
			bg_palette_data.at(i)[0],
			bg_palette_data.at(i)[1],
			bg_palette_data.at(i)[2],
			bg_palette_data.at(i)[3]
		);
	}

	// bring in background
	// could avoid code replication by putting the above into a function and referencing
	// palette table indices with pointers + some other stuff i think
	glm::uvec2 bg_size;
	std::vector< glm::u8vec4 > bg_data;
	OriginLocation bg_ol = OriginLocation::LowerLeftOrigin;
	std::string bg_path = data_path("sprites/background.png");
	load_png(bg_path, &bg_size, &bg_data, bg_ol);

	// set all background tiles to reference tile 0 in tile table
	// and palette 6. 0b00100000000 = 1536
	int ppubgpx = ppu.BackgroundWidth * ppu.BackgroundHeight;
	for (int i=0; i<ppubgpx; i++){
		ppu.background[i] = 1280;
	}

	


	int bg_num_pixels = bg_size.x * bg_size.y;
	for (int i=0; i<bg_num_pixels; i++){
		// all pixels will either be black or white
		// if the pixel is black
		if (bg_data.at(i)[0] == 0){
			myRoom.insert_wall(i);
		}
	}

	myRoom.print_wall();




	// // debugging code to print out background info
	for (int i=0; i<16;i++){
		std::cout << "background[" << i << "] palette index " << (int)((ppu.background[i] >> 8) & 7)  <<  " tile index " << (int)(ppu.background[i] & 255) << std::endl;
	}

	









	
	
	



	//Also, *don't* use these tiles in your game:


	// { //use tiles 0-16 as some weird dot pattern thing:
	// 	std::array< uint8_t, 8*8 > distance;
	// 	for (uint32_t y = 0; y < 8; ++y) {
	// 		for (uint32_t x = 0; x < 8; ++x) {
	// 			float d = glm::length(glm::vec2((x + 0.5f) - 4.0f, (y + 0.5f) - 4.0f));
	// 			d /= glm::length(glm::vec2(4.0f, 4.0f));
	// 			distance[x+8*y] = uint8_t(std::max(0,std::min(255,int32_t( 255.0f * d ))));
	// 		}
	// 	}
	// 	for (uint32_t index = 0; index < 16; ++index) {
	// 		PPU466::Tile tile;
	// 		uint8_t t = uint8_t((255 * index) / 16);
	// 		for (uint32_t y = 0; y < 8; ++y) {
	// 			uint8_t bit0 = 0;
	// 			uint8_t bit1 = 0;
	// 			for (uint32_t x = 0; x < 8; ++x) {
	// 				uint8_t d = distance[x+8*y];
	// 				if (d > t) {
	// 					bit0 |= (1 << x);
	// 				} else {
	// 					bit1 |= (1 << x);
	// 				}
	// 			}
	// 			tile.bit0[y] = bit0;
	// 			tile.bit1[y] = bit1;
	// 		}
	// 		ppu.tile_table[index] = tile;
	// 	}
	// }



	

	//use sprite 32 as a "player":
	

	//makes the outside of tiles 0-16 solid:
	// ppu.palette_table[0] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// };

	//makes the center of tiles 0-16 solid:
	// ppu.palette_table[1] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// };

	//used for the player: (tile table 32)
	// ppu.palette_table[7] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0xff, 0xff, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// };

	// ppu.palette_table[7] = {
	// 	glm::u8vec4(255, 0, 0, 255),
	// 	glm::u8vec4(255, 0, 0, 255),
	// 	glm::u8vec4(255, 0, 0, 255),
	// 	glm::u8vec4(255, 0, 0, 255),
	// };

	//used for the misc other sprites:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x88, 0x88, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	constexpr float PlayerSpeed = 30.0f;
	if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	// ppu.background_color = glm::u8vec4(
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
	// 	0xff
	// );

	//tilemap gets recomputed every frame as some weird plasma thing:
	//NOTE: don't do this in your game! actually make a map or something :-)
	// for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
	// 	for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
	// 		//TODO: make weird plasma thing
	// 		ppu.background[x+PPU466::BackgroundWidth*y] = ((x+y)%16);
	// 	}
	// }

	//background scroll:
	// ppu.background_position.x = int32_t(-0.5f * player_at.x);
	// ppu.background_position.y = int32_t(-0.5f * player_at.y);

	//player sprite:
	ppu.sprites[0].x = int8_t(player_at.x);
	ppu.sprites[0].y = int8_t(player_at.y);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 7;

	//some other misc sprites:
	for (uint32_t i = 1; i < 63; ++i) {
		float amt = (i + 2.0f * background_fade) / 62.0f;
		ppu.sprites[i].x = int8_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].y = int8_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].index = 32;
		ppu.sprites[i].attributes = 6;
		if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}
