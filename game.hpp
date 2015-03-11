/*	Space Intruders		*/
/*    Author: Connor Workman	*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Game {
	public:
		Game(); // constructor calls init() and loadMedia() instantiates game variables
		~Game();
		/*void init();*/
		void loadMedia();
		void close();
		void getUserInput();
		void updateGameState();
		void drawGame();
		void drawVictoryScreen();
		void drawDeadScreen();
		int time_left();
		int mainGameLoop();
		void incrementNextTime();
		void main();
		void mainMenu();
		bool getRestart();
		void setUpLevelTwo();
		void setRestart(bool x);
		bool getVictory();
		void setUpLevelOne();
		bool getDead();
		void checkCollision();
		void fireAlienLazers();
		void checkAlienMove();
		void moveAliens();
		void moveAlienLazers();
		void dropAliens();
	private:
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
};
