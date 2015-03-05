#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class AlienLazer {
	public:
		static const int LAZER_WIDTH = 3;
		static const int LAZER_HEIGHT = 20;
		static const int DEFAULT_VELOCITY = 2;
		AlienLazer();//set player lazer defaults
		~AlienLazer();
		void setX(int x);
		void setY(int y);
		int getX();
		int getY();
		void fire();
		bool getFired();	
		bool move();
		void setVelocY(int y);
		void render();
		void reset();
		int getVelocY();
	private:
		bool fired;
		int positX;
		int positY;
		int velocY;
};
