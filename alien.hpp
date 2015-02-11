/*	Space Intruders		*/
/*    Author: Connor Workman	*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>



class Alien {
	public:
		static const int ALIEN_WIDTH = 30;
		static const int ALIEN_HEIGHT = 30;
		Alien();
		void setAlienPosit(int alienNumber);
		int getAlienHorizontalPosit();
		int getAlienVerticalPosit();
		void move(int m);
		//void handleEvent(SDL_Event& e);//TODO implement explosion/destruction event when hit by playerLazer
		void render(int x, int y, SDL_Rect* clip);
	private:
		int alienHorizontalPosit;
		int alienVerticalPosit;
};
