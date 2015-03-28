#ifndef PLAYERLAZER_HPP
#define PLAYERLAZER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include "imageTexture.hpp"

class PlayerLazer {
	public:
		static const int LAZER_WIDTH = 3;
		static const int LAZER_HEIGHT = 20;
		static const int DEFAULT_VELOCITY = 8;
		static const int SCREEN_WIDTH = 640;
		static const int SCREEN_HEIGHT = 480;
		static const int PLAYER_HEIGHT = 25;

		PlayerLazer();//set player lazer defaults
		~PlayerLazer();
		
		int getX();
		int getY();
		
		void setX(int x);
		void setVelocY(int y);

		void fire();
		bool getFired();	
		bool loadPlayerLazerTexture();
		bool move();
		
		void render();
		
		void reset();
	private:
		ImageTexture playerLazerTexture;
		bool fired;
		int positX;
		int positY;
		int velocY;
};
#endif