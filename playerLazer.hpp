#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>



class PlayerLazer {
	public:
		static const int LAZER_WIDTH = 5;
		static const int LAZER_HEIGHT = 5;
		PlayerLazer();//set player lazer defaults
		void setPlayerLazerPosit(int playerLocationWhenFiring);
		void handleEvent(SDL_Event& e);
		void playerLazerMove();
		void render();
	private:
		int playerLazerHorizontalPosit;
		int playerLazerVerticalPosit;
		int playerLazerVerticalVelocity;
};
