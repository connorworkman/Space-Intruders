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
		int time_left();
		int mainGameLoop();
		void incrementNextTime();
		void main();
	private:
		int alienCount = 80;
		int next_time;
		int scrollingOffset;
		std::vector<Alien> alienVector;
		std::vector<Alien>::iterator itty;
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
};
