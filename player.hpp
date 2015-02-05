/*	Space Intruders		*/
/*    Author: Connor Workman	*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>





class Player {
	public:
		static const int PLAYER_WIDTH = 20;
		static const int PLAYER_HEIGHT = 20;
		static const int PLAYER_VEL = 5;
		Player();
		void handleEvent(SDL_Event& e);
		void move();
		int getPlayerPositX();
		void render();
	private:
		int playerPositX;
		int playerPositY;
		int playerVelocX;
};
