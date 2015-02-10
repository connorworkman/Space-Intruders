#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>



class PlayerLazer {
	public:
		static const int LAZER_WIDTH = 5;
		static const int LAZER_HEIGHT = 5;
		PlayerLazer();//set player lazer defaults
		void setPlayerLazerPositX(int playerLocationWhenFiring);
		void setFired(int x);
		int getFired();
		void handleEvent(SDL_Event& e);
		void move();
		void render();
	private:
		int fired;
		int playerLazerPositX;
		int playerLazerPositY;
		int playerLazerVelocY;
};
