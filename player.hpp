#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Player {
	public:
		static const int PLAYER_WIDTH = 20;
		static const int PLAYER_HEIGHT = 25;
		static const int PLAYER_VEL = 5;
		Player();
		void handleEvent(SDL_Event& e);
		
		void move();
		
		int getX();
		int getY();
		
		void setVelocX(int newVelocity);
		
		void render(SDL_Rect* playerClip);

	private:
		int positX;
		int positY;
		int velocX;
};
#endif