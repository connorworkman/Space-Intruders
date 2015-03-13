/*	Space Intruders		*/
/*    Author: Connor Workman	*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
	
class Alien {
	public:
		static const int ALIEN_WIDTH = 40;
		static const int ALIEN_HEIGHT = 28;
		Alien();

		void levelOneAlienLayout(int i);
		void levelTwoAlienLayout(int i);
		void levelThreeAlienLayout(int i);

		void resetLazer();
		void setX(int x);
		void setY(int y);
		void setCollisionState(bool x);
		void setLazerX(int x);
		void setLazerY(int y);
		void setLazerVelocY(int newVelocity);

		int getX();
		int getY();
		int getLazerVelocY();
		int getLazerPositX();
		int getLazerPositY();
		bool getCollisionState();
		bool getFired();

		void fireLazer();
		bool moveLazer();
		void drop();
		void explode();
		void hide();
		void move(int moveSpeed);
		
		void renderLazer();
		void render(int x, int y, SDL_Rect* clip);

	private:
		AlienLazer aLazer;
		bool collision;
		int positX;
		int positY;
};
