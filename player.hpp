#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "imageTexture.hpp"

class Player {
	public:
		static const int SCREEN_WIDTH = 640;
		static const int SCREEN_HEIGHT = 480;
		static const int PLAYER_WIDTH = 20;
		static const int PLAYER_HEIGHT = 25;
		static const int PLAYER_VEL = 5;
		static const int NUM_ANIMATION_FRAMES = 4;

		Player();
		~Player();
		void handleEvent(SDL_Event& e);
		
		void move();
		
		int getX();
		int getY();
		bool loadPlayerTexture();
		void setVelocX(int newVelocity);
		SDL_Rect* getPlayerClip(int frame, int FRAME_RATE);
		void render(SDL_Rect* playerClip);

	private:
		ImageTexture playerAnimationTexture;
		SDL_Rect playerClips[NUM_ANIMATION_FRAMES];
		int positX;
		int positY;
		int velocX;
};
#endif