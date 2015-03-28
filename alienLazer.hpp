#ifndef _ALIEN_LAZER_HPP
#define _ALIEN_LAZER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class AlienLazer {
	public:
		static const int LAZER_WIDTH = 3;
		static const int LAZER_HEIGHT = 20;
		static const int DEFAULT_VELOCITY = 2;
		AlienLazer();//set player lazer defaults
		~AlienLazer();

		void setX(int x);
		void setY(int y);
		void setVelocY(int y);

		int getX();
		int getY();
		int getVelocY();
		bool getFired();

		void fire();
		bool move();
		void reset();
		
		void render();
		
	private:
		bool fired;
		int positX;
		int positY;
		int velocY;
};
#endif