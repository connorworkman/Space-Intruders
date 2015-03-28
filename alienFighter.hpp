/*	Space Intruders		*/
/*    Author: Connor Workman	*/
#ifndef _ALIEN_FIGHTER_HPP
#define _ALIEN_FIGHTER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
	
class AlienFighter {
	public:
		static const int ALIEN_WIDTH = 35;
		static const int ALIEN_HEIGHT = 35;
		AlienFighter();

		void levelOneAlienLayout(int i);
		void levelTwoAlienLayout(int i);
		void levelThreeAlienLayout(int i);

		void setX(int x);
		void setY(int y);
		void setCollisionState(bool x);
		void setLazerX(int x);
		void setLazerY(int y);
		void setLazerVelocY(int newVelocity);

		int getX();
		int getY();
		bool getFired();
		int getLazerVelocY();
		int getLazerPositX();
		int getLazerPositY();
		bool getCollisionState();

		bool moveLazer();
		void resetLazer();
		void drop();
		void explode();
		void hide();
		void move(int moveSpeed);
		void fireLazer();
		
		void render(int x, int y, SDL_Rect* alienFighterClip);
		void renderLazer();
		
		
	private:
		AlienLazer aLazer;
		bool collision;
		int positX;
		int positY;
};
#endif