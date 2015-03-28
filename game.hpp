/*	Space Intruders		*/
/*    Author: Connor Workman	*/
#ifndef _GAME_HPP
#define _GAME_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Game {
	public:
		Game(); // constructor calls init() and loadMedia() instantiates game variables
		~Game();
		/*void init();*/
		
		void close();

		void getUserInput();
		
		void updateGameState();
		void checkCollision();
		void checkAlienHord();
		void moveAlienHord();
		void dropAlienHord();
		void fireAlienLazers();
		void moveAlienLazers();
		void playSound();
		
		void main();
		int mainGameLoop();
		void loadMedia();
		int time_left();
		void incrementNextTime();

		void drawGame();
		void drawVictoryScreen();
		void drawDeadScreen();
		void drawPauseScreen();
		
		void mainMenu(bool preMenuOnly);
		
		void setRestart(bool restartSetting);
		bool getRestart();
		void setUpLevelOne();
		void setUpLevelTwo();
		void setUpLevelThree();
		bool getVictory();
		bool getDead();
		int getLevel();

	private:
		int level;
		int alienCount;
		int alienFighterCount;
		int next_time;
		int scrollingOffset;
		std::vector<AlienFighter> alienFighterVector;
		std::vector<AlienFighter>::iterator fighterIterator;
		std::vector<Alien> alienVector;
		std::vector<Alien>::iterator alienIterator;
		int frame;
		int frameCount;
		Player player;
		PlayerLazer pLazer;
		SDL_Event event;
		bool dead;
		bool victory;
		bool quit;
		bool drop;
		bool wallHit;
		int dropCount;
		int imgFlags;
		int score;
		bool restart;
		bool pause;
};
#endif