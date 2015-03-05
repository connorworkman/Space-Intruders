#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class AlienFighter {
	public:
		static const int ALIEN_WIDTH =  30;
		static const int ALIEN_HEIGHT = 20;
		AlienFighter();
		~AlienFighter();
		void move();
		int getX();
		int getY();
		void render();
	private:
		int positX;
		int positY;
		int velocX;
};
