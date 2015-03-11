/*	Space Intruders		*/
/*    Author: Connor Workman	*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
	
class AlienFighter {
	public:
		static const int ALIEN_WIDTH = 30;
		static const int ALIEN_HEIGHT = 28;
		AlienFighter();
		void levelOneAlienLayout(int i);
		void levelTwoAlienLayout(int i);
		void setX(int x);
		void setY(int y);
		int getX();
		int getY();
		void drop();
		void explode();
		void hide();
		void setCollisionState(bool x);
		bool getCollisionState();
		void move(int moveSpeed);
		//void handleEvent(SDL_Event& e);//TODO implement explosion/destruction event when hit by playerLazer
		void render(int x, int y);
		void setLazerX(int x);
		void setLazerY(int y);
		void setLazerVelocY(int newVelocity);
		void fireLazer();
		bool getFired();
		void renderLazer();
		bool moveLazer();
		void resetLazer();
		int getLazerVelocY();
		int getLazerPositX();
		int getLazerPositY();
	private:
		AlienLazer aLazer;
		bool collision;
		int positX;
		int positY;
};
