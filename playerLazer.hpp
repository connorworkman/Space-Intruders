#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>



class PlayerLazer {
	public:
		static const int LAZER_WIDTH = 5;
		static const int LAZER_HEIGHT = 5;
		PlayerLazer();//set player lazer defaults
		~PlayerLazer();
		void setPlayerLazerPositX(int playerLocationWhenFiring);
		int getPositX();
		int getPositY();
		void setFired(bool x);
		bool getFired();	
		bool move();
		void setVelocY(int newVelocY);
		void render();
		void reset();
	private:
		bool fired;
		int playerLazerPositX;
		int playerLazerPositY;
		int playerLazerVelocY;
};
