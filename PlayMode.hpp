#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <set>
#include <deque>
#include <iostream>

struct Room {
    std::set<int> walls;

    // Method to insert an element into the 'data' vector
    void insert_wall(int wall_index) {
        walls.insert(wall_index);
    }

	bool is_pixel_in_wall(int xpos, int ypos){
		// because each background tile is 8 bits
		// we round down to see if a pixel is inside a black tile
		int xpos_new = (xpos - (xpos % 8))/8;
		int ypos_new = (xpos - (xpos % 8))/8;
		int set_search_value = (xpos_new*64) + ypos_new;
		
		std::cout << "set_search_value: " << set_search_value << std::endl;

		return walls.find(set_search_value) != walls.end();
		// code for checking if set contains an element taken from this answer:
		// https://stackoverflow.com/questions/1701067/how-to-check-that-an-element-is-in-a-stdset
	}


};

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//some weird background animation:
	float background_fade = 0.0f;

	Room myRoom;
	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);



	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
