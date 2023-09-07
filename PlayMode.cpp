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
	// allocate bare minimum number of assets for maze - just 3 tiles.
	// first create bitplanes to use for each tile
	std::array<uint8_t, 8> zeros_bitplane = {0,0,0,0,0,0,0,0};
	std::array<uint8_t, 8> ones_bitplane = {255,255,255,255,255,255,255,255};

	// then we establish tile 0 and 1 as the tiles to use for 
	// black and white background squares, respectively
	ppu.tile_table[0].bit0 = zeros_bitplane;
	ppu.tile_table[0].bit1 = zeros_bitplane;

	ppu.tile_table[1].bit0 = zeros_bitplane;
	ppu.tile_table[1].bit1 = ones_bitplane;

	ppu.tile_table[2].bit0 = ones_bitplane;
	ppu.tile_table[2].bit1 = ones_bitplane;

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
		std::cout << i << std::endl;
		std::cout << (int)bg_palette_data.at(i)[0] << std::endl;
		std::cout << (int)bg_palette_data.at(i)[1] << std::endl;
		std::cout << (int)bg_palette_data.at(i)[2] << std::endl;
		std::cout << (int)bg_palette_data.at(i)[3] << std::endl;
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
	

	// set all background tiles to reference tile 1 in tile table
	// and palette 6. 0b00100000001 = 1536
	// to referene 
	int ppubgpx = ppu.BackgroundWidth * ppu.BackgroundHeight;
	for (int i=0; i<ppubgpx; i++){
		ppu.background[i] = 1281;
	}


	glm::uvec2 bg_size;
	std::vector< glm::u8vec4 > bg_data;
	OriginLocation bg_ol = OriginLocation::LowerLeftOrigin;
	std::string bg_path = data_path("sprites/bg3.png");
	load_png(bg_path, &bg_size, &bg_data, bg_ol);
	bool p_r, p_g, p_b;

	for (int i=0; i<bg_size.y; i++){
		
		for (int j=0; j<bg_size.x; j++){
			// std::cout << (int)(i*bg_size.y)+j << std::endl;
			p_r = bg_data.at((i*bg_size.x)+j)[0] > 250;
			p_g = bg_data.at((i*bg_size.x)+j)[1] > 250;
			p_b = bg_data.at((i*bg_size.x)+j)[2] > 250;

			if (p_r && !p_g && !p_b){
				// register red pixels as walls
				myRoom.insert_wall(((i*2)*bg_size.x)+j);
				// set the background to reference black tile
				ppu.background[((i*2)*bg_size.x)+j] = 1280;
			}
			else if (p_g && !p_r && !p_b){
				ppu.background[((i*2)*bg_size.x)+j] = 1281;
			}
			else if (p_b && !p_r && !p_g){
				// set the background to reference white tile
				ppu.background[((i*2)*bg_size.x)+j] = 1281;
				// make the blue tile the starting point 
				// myRoom.insert_starting_point((i%64)*8, (i-(i%64))*8);
				myRoom.insert_starting_point(j*8, i*8);
			}
			else if (!p_g && !p_b && !p_r){
				ppu.background[((i*2)*bg_size.x)+j] = 1282;
				myRoom.insert_ending_point(j*8, i*8);
			}
		}
	}

	ppu.background[960] = 1280;
	std::cout <<  myRoom.end_x << std::endl;
	std::cout <<  myRoom.end_y << std::endl;

	player_at.x = myRoom.start_x;
	player_at.y = myRoom.start_y;

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


	
	// if ( (myRoom.end_x == player_at.x ) && (myRoom.end_y == player_at.y) ){
	// 	std::cout << "die" << std::endl;
	// }

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	// background_fade += elapsed / 10.0f;
	// background_fade -= std::floor(background_fade);

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
	if ((myRoom.end_x-5 <= player_at.x) && (player_at.x <= myRoom.end_x+5) &&
		(myRoom.end_y-5 <= player_at.y) && (player_at.y <= myRoom.end_y+5) ){
		std::cout << "DIE" << std::endl;
	}
	std::cout << player_at.x << " " << myRoom.end_x << std::endl;
	std::cout << player_at.y << " " << myRoom.end_y << std::endl;
	// std::cout << (int)player_at.x << " " << (int)player_at.y << std::endl;
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



	//move all the other sprites off the screen
	for (uint32_t i = 1; i < 63; ++i) {
		ppu.sprites[i].x = int8_t(300);
		ppu.sprites[i].y = int8_t(300);

	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}
