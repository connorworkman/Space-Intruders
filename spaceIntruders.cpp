/*	Space Intruders		*/
/*    Author: Connor Workman	*/

#include <SDL2/SDL.h>// main SDL library
#include <SDL2/SDL_image.h>// for textures
#include <SDL2/SDL_mixer.h>// for sound
#include <SDL2/SDL_ttf.h>// for score-keeping
#include <stdlib.h>
#include <algorithm>// for erase/remove_if
#include <vector>// for vectors
#include <iostream>
#include <time.h>
#include <sstream>
#include "imageTexture.hpp"
#include "player.hpp"
#include "playerLazer.hpp"
#include "alienLazer.hpp"
#include "alien.hpp"
#include "alienFighter.hpp"
#include "game.hpp"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
//animation constants
const int NUM_ANIMATION_FRAMES = 4;
const int MENU_ANIMATION_FRAMES = 6;
//game speed constants
const int GAMEOVER_FRAMES = 2;
const int VICTORY_ANIMATION_FRAMES = 9;
const int GAME_SPEED = 5000;
const int FRAME_RATE = 10;
const int TICK_INTERVAL = 20;
//TODO create additional alien class that flies across top row, worth extra points
//TODO create enemy lazer function for Alien class

SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

//instantiate all available textures
ImageTexture playerAnimationTexture;
ImageTexture backgroundTexture;
ImageTexture playerLazerTexture;
ImageTexture alienAnimationTexture;
ImageTexture gameOverTexture;
ImageTexture youWinTexture;
ImageTexture alienLazerTexture;
ImageTexture mainMenuTexture;
ImageTexture fighterTexture;
//TODO ImageTexture explosionTexture; //explosion texture should be explosion animation
ImageTexture alienFighterTexture;
ImageTexture scoreTexture;
ImageTexture pointsTexture;
SDL_Rect alienClips[NUM_ANIMATION_FRAMES];
SDL_Rect gameOverClips[GAMEOVER_FRAMES];
SDL_Rect youWinClips[VICTORY_ANIMATION_FRAMES];
SDL_Rect playerClips[NUM_ANIMATION_FRAMES];
SDL_Rect menuClips[MENU_ANIMATION_FRAMES];
//TODO add explosion textures, enemy lazer textures?, additional different enemy textures
SDL_Color white;
TTF_Font *times;


ImageTexture::ImageTexture() {
	sdlTexture = NULL;
	sdlTextureWidth = 0;
	sdlTextureHeight = 0;
}

bool ImageTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	free();
	SDL_Surface* loadedSurface = TTF_RenderText_Solid(times,textureText.c_str(),textColor);
	sdlTexture = SDL_CreateTextureFromSurface(renderer,loadedSurface);
	sdlTextureWidth = loadedSurface->w;
	sdlTextureHeight = loadedSurface->h;
	SDL_FreeSurface(loadedSurface);
	return (sdlTexture != NULL);
}

ImageTexture::~ImageTexture() {free();}

bool ImageTexture::loadFromFile(std::string path) {
	free();//free previous textures before attempting new load
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	sdlTextureWidth = loadedSurface->w;
	sdlTextureHeight = loadedSurface->h;
	SDL_FreeSurface(loadedSurface);//free old surface
	sdlTexture = newTexture;
	return sdlTexture != NULL;
}

void ImageTexture::free() {
	if(sdlTexture != NULL) {
		//destroy texture and return all values back to defaults
		SDL_DestroyTexture( sdlTexture );
		sdlTexture = NULL;
		sdlTextureWidth = 0;
		sdlTextureHeight = 0;
	}
}

void ImageTexture::setColorMode(Uint8 red, Uint8 green, Uint8 blue) {
	SDL_SetTextureColorMod(sdlTexture, red, green, blue);
}

void ImageTexture::setBlendMode(SDL_BlendMode blending) {
	SDL_SetTextureBlendMode(sdlTexture, blending);
}

void ImageTexture::setAlphaMode(Uint8 alpha){
	SDL_SetTextureAlphaMod(sdlTexture, alpha);
}

void ImageTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderRect = {x, y, sdlTextureWidth, sdlTextureHeight};
	if (clip!= NULL) {
		renderRect.w = clip->w;
		renderRect.h = clip->h;
	}
	SDL_RenderCopyEx(renderer, sdlTexture, clip, &renderRect, angle, center, flip);
}

int ImageTexture::getWidth() {
	return sdlTextureWidth;
}

int ImageTexture::getHeight() {
	return sdlTextureHeight;
}


Player::Player() {
    positX = (SCREEN_WIDTH/2)-this->PLAYER_WIDTH;
    positY = SCREEN_HEIGHT-this->PLAYER_HEIGHT;
    velocX = 0;
}

void Player::handleEvent( SDL_Event& event ) {
	if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {//keypress occurrs
       	switch(event.key.keysym.sym ) {
			case SDLK_LEFT:
				velocX -= PLAYER_VEL;
				break;
			case SDLK_RIGHT:
				velocX += PLAYER_VEL;
				break;
    	}
	}
	else if(event.type == SDL_KEYUP && event.key.repeat == 0) {//keypress is released
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				velocX += PLAYER_VEL;
				break;
			case SDLK_RIGHT:
				velocX -= PLAYER_VEL;
				break;
    	}
	}
}

void Player::move() {
    positX += velocX;
    //keep the player on the screen by correcting by velocX
    if((positX < 0) || (positX + PLAYER_WIDTH > SCREEN_WIDTH)) {
        positX -= velocX;
    }
}

int Player::getX() {return positX;}//player X position getter

int Player::getY() {return positY;}//player Y position getter

void Player::render(SDL_Rect* playerClip) {playerAnimationTexture.render(positX, positY, playerClip);}//player renderer, TODO add clip argument when player is animated
void Player::setVelocX(int newVelocity) {
	this->velocX = newVelocity;
}
/* PlayerLazer Functions: *
 * constructor ~ Initialize to defaults
 * destructor ~ reset to default location
 * setX ~ set the X coordinate of the lazer
 * setY ~ set the Y coordinate of the lazer
 * getX ~ get the X coordinate
 * getY ~ get the Y coordinate
 * fire ~ set the fired state to ON (true)
 * getFired ~ get the fired state of the object
 * setVelocY ~ set the vertical velocity of the lazer
 * move ~ move the lazer up the screen by its vertical velocity
 * reset ~ set lazer properties back to default
*/
PlayerLazer::PlayerLazer() {
	this->fired = false;
	this->positX = 0;
	this->positY = SCREEN_HEIGHT - Player::PLAYER_HEIGHT;
	this->velocY = 0;
}

PlayerLazer::~PlayerLazer() {}//empty destructor for PlayerLazer
void PlayerLazer::setX(int x) {
	this->positX = x+8;
}//place PlayerLazer x coordinate

int PlayerLazer::getX() {
	return this->positX;
}

int PlayerLazer::getY() {
	return this->positY;
}

bool PlayerLazer::move() {
	positY -= velocY;
	if (positY-LAZER_HEIGHT < 0) {//if lazer goes off screen
		return false;//return false so we can reset it
	}
	return true;
}

bool PlayerLazer::getFired() {
	return this->fired;
}//query the state of the PlayerLazer instance

void PlayerLazer::fire() {
	this->fired = true;
	this->velocY = PlayerLazer::DEFAULT_VELOCITY;
}

void PlayerLazer::setVelocY(int newVelocity) {
	this->velocY = newVelocity;
}//set lazer's vertical velocity

void PlayerLazer::reset() {
	this->fired = false;
	this->positY = SCREEN_HEIGHT - Player::PLAYER_HEIGHT;
	this->positX = 0;
	this->velocY = 0;
}

void PlayerLazer::render() {
	playerLazerTexture.render(positX, positY);
}

/* Alien Functions: *
 * constructor ~ set default values for an alien
 * destructor is empty
 * void levelOneAlienLayout(i) ~ set the initial position for each alien in the level (square arrangement)
 * void setX(x) ~ set X coordinate
 * void setY(y) ~ set Y coordinate
 * int getX() ~ get X
 * int getY() ~ get Y
 * void render(x,y,clip) ~ render the alien based on position and current clip
 * void move(frameCount, dropCount) ~ move the alien based on GAME_SPEED and frameCount, speed increases by dropCount
 * void drop() 	~ drop the aliens toward the player
 * setCollisionState ~ set the current collision state with playerLazer object
 * getCollisionState ~ get the current collision state (true or false)
*/
Alien::Alien() {
	positX = 0;
	positY = 0;
	collision = false;
}

void Alien::levelOneAlienLayout(int i) {
	int aliensPerRow = 8;
	this->positX = 60 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 95 + (i / aliensPerRow)*(5 + Alien::ALIEN_HEIGHT);
}
void Alien::levelTwoAlienLayout(int i) {
	int aliensPerRow = 9;
	this->positX = 60 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 95 + (i / aliensPerRow)*(5 + Alien::ALIEN_HEIGHT);
}

void Alien::setX(int x) {
	this->positX = x;
}

void Alien::setY(int y) {
	this->positY = y;
}

int Alien::getX() {
	return this->positX;
}

int Alien::getY() {
	return this->positY;
}

void Alien::render(int x, int y, SDL_Rect* clip) {
	alienAnimationTexture.render(x, y, clip);
}

void Alien::move(int moveSpeed) {
	this->positX += moveSpeed;
}

void Alien::drop() {
	this->positY += Alien::ALIEN_HEIGHT;
}

void Alien::setCollisionState(bool x) {
	this->collision = x;
}

bool Alien::getCollisionState() {
	return this->collision;
}

void Alien::setLazerX(int x) {this->aLazer.setX(x);}

void Alien::setLazerY(int y) {this->aLazer.setY(y);}

void Alien::setLazerVelocY(int newVelocity) {
	this->aLazer.setVelocY(newVelocity);
}

void Alien::fireLazer() {
	this->aLazer.fire();
}

bool Alien::getFired() {
	return this->aLazer.getFired();
}

void Alien::renderLazer() {
	this->aLazer.render();
}

bool Alien::moveLazer() {
	this->aLazer.move();
}

void Alien::resetLazer() {
	this->aLazer.reset();
}

int Alien::getLazerVelocY() {
	return this->aLazer.getVelocY();
}

int Alien::getLazerPositX() {
	return this->aLazer.getX();
}

int Alien::getLazerPositY() {
	return this->aLazer.getY();
}
AlienLazer::AlienLazer() {
	this->fired = false;
	this->positX = 0;
	this->positY = 0;
	this->velocY = 0;
}
AlienLazer::~AlienLazer() {}

void AlienLazer::setX(int x) {
	this->positX = x;
}

void AlienLazer::setY(int y) {
	this->positY = y;
}
int AlienLazer::getX() {return this->positX;}
int AlienLazer::getY() {return this->positY;}
bool AlienLazer::move() {
	positY += velocY;
	if (positY+LAZER_HEIGHT > SCREEN_HEIGHT) {
		return false;
	}
	return true;
}
bool AlienLazer::getFired() {
	return this->fired;
}
void AlienLazer::fire() {
	this->fired = true;
	this->velocY = AlienLazer::DEFAULT_VELOCITY;
}
void AlienLazer::setVelocY (int newVelocity) {
	this->velocY  = newVelocity;
}
void AlienLazer::reset() {
	this->fired = false;
	this->positY = 0;
	this->positX = 0;
	this->velocY = 0;
}
void AlienLazer::render() {
	alienLazerTexture.render(positX, positY);
}
int AlienLazer::getVelocY(){
	return this->velocY;
}

/* AlienFighter Functions: *
 * constructor ~ set default values for an alien
 * destructor is empty
 * void levelOneAlienLayout(i) ~ set the initial position for each alien in the level (square arrangement)
 * void setX(x) ~ set X coordinate
 * void setY(y) ~ set Y coordinate
 * int getX() ~ get X
 * int getY() ~ get Y
 * void render(x,y,clip) ~ render the alien based on position and current clip
 * void move(frameCount, dropCount) ~ move the alien based on GAME_SPEED and frameCount, speed increases by dropCount
 * void drop() 	~ drop the aliens toward the player
 * setCollisionState ~ set the current collision state with playerLazer object
 * getCollisionState ~ get the current collision state (true or false)
*/
AlienFighter::AlienFighter() {
	positX = 0;
	positY = 0;
	collision = false;
}

void AlienFighter::levelOneAlienLayout(int i) {
	int aliensPerRow = 8;
	this->positX = 70 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 30 + (i / aliensPerRow)*(5 + Alien::ALIEN_HEIGHT);
}
void AlienFighter::levelTwoAlienLayout(int i) {
	int aliensPerRow = 9;
	this->positX = 70 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 30 + (i / aliensPerRow)*(5 + Alien::ALIEN_HEIGHT);
}

void AlienFighter::setX(int x) {
	this->positX = x;
}

void AlienFighter::setY(int y) {
	this->positY = y;
}

int AlienFighter::getX() {
	return this->positX;
}

int AlienFighter::getY() {
	return this->positY;
}

void AlienFighter::render(int x, int y) {
	fighterTexture.render(x, y);
}

void AlienFighter::move(int moveSpeed) {
	this->positX += moveSpeed;
}

void AlienFighter::drop() {
	this->positY += Alien::ALIEN_HEIGHT;
}

void AlienFighter::setCollisionState(bool x) {
	this->collision = x;
}

bool AlienFighter::getCollisionState() {
	return this->collision;
}

void AlienFighter::setLazerX(int x) {this->aLazer.setX(x);}

void AlienFighter::setLazerY(int y) {this->aLazer.setY(y);}

void AlienFighter::setLazerVelocY(int newVelocity) {
	this->aLazer.setVelocY(newVelocity);
}

void AlienFighter::fireLazer() {
	this->aLazer.fire();
}

bool AlienFighter::getFired() {
	return this->aLazer.getFired();
}

void AlienFighter::renderLazer() {
	this->aLazer.render();
}

bool AlienFighter::moveLazer() {
	this->aLazer.move();
}

void AlienFighter::resetLazer() {
	this->aLazer.reset();
}

int AlienFighter::getLazerVelocY() {
	return this->aLazer.getVelocY();
}

int AlienFighter::getLazerPositX() {
	return this->aLazer.getX();
}

int AlienFighter::getLazerPositY() {
	return this->aLazer.getY();
}

Game::Game() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");//rendering quality = 1
	
	window = SDL_CreateWindow("Space Intruders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
		exit(1);
	}
	
	//create the vsynced renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		std::cout << "Failed to create renderer." << std::endl;
		exit(1);
	}
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	imgFlags = IMG_INIT_PNG;
	IMG_Init(imgFlags);
	TTF_Init();

	times = TTF_OpenFont("Munro.ttf", 20);
	white = {255,255,255};
	if (times == NULL) {
		std::cout << "Failed to open font file." << std::endl;
		exit(1);
	}
	loadMedia();
	quit = false;
	drop = false;
	alienCount = 64;
	alienFighterCount = 16;
	alienFighterVector.resize(alienFighterCount);
	fighterIterator = alienFighterVector.begin();
	alienVector.resize(alienCount);
	alienIterator = alienVector.begin();
	int i = 0;
	//position aliens
	for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {
		alienIterator->levelOneAlienLayout(i);
		i++;
	}
	//position alien fighters
	i = 0;
	for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
		fighterIterator->levelOneAlienLayout(i);
		i++;
	}
	scrollingOffset = 0;
	frame = 0;
	frameCount = 0;
	dropCount = 1;
	wallHit = false;
	dead = false;
	victory = false;
	score = 0;
	restart = false;

	menuClips[0].x = 0;              menuClips[0].y = 0; menuClips[0].w = SCREEN_WIDTH; menuClips[0].h = SCREEN_HEIGHT;
	menuClips[1].x = SCREEN_WIDTH;   menuClips[1].y = 0; menuClips[1].w = SCREEN_WIDTH; menuClips[1].h = SCREEN_HEIGHT;
	menuClips[2].x = SCREEN_WIDTH*2; menuClips[2].y = 0; menuClips[2].w = SCREEN_WIDTH; menuClips[2].h = SCREEN_HEIGHT;
	menuClips[3].x = SCREEN_WIDTH*3; menuClips[3].y = 0; menuClips[3].w = SCREEN_WIDTH; menuClips[3].h = SCREEN_HEIGHT;
	menuClips[4].x = SCREEN_WIDTH*4; menuClips[4].y = 0; menuClips[4].w = SCREEN_WIDTH; menuClips[4].h = SCREEN_HEIGHT;
	menuClips[5].x = SCREEN_WIDTH*5; menuClips[5].y = 0; menuClips[5].w = SCREEN_WIDTH; menuClips[5].h = SCREEN_HEIGHT;

	playerClips[0].x = 0;                      playerClips[0].y = 0; playerClips[0].w = Player::PLAYER_WIDTH; playerClips[0].h = Player::PLAYER_HEIGHT;
	playerClips[1].x = Player::PLAYER_WIDTH;   playerClips[1].y = 0; playerClips[1].w = Player::PLAYER_WIDTH; playerClips[1].h = Player::PLAYER_HEIGHT;
	playerClips[2].x = Player::PLAYER_WIDTH*2; playerClips[2].y = 0; playerClips[2].w = Player::PLAYER_WIDTH; playerClips[2].h = Player::PLAYER_HEIGHT;
	playerClips[3].x = Player::PLAYER_WIDTH*3; playerClips[3].y = 0; playerClips[3].w = Player::PLAYER_WIDTH; playerClips[3].h = Player::PLAYER_HEIGHT;

	//alien animation clip data
	alienClips[0].x = 0;                    alienClips[0].y = 0; alienClips[0].w = Alien::ALIEN_WIDTH; alienClips[0].h = Alien::ALIEN_HEIGHT;
	alienClips[1].x = Alien::ALIEN_WIDTH;   alienClips[1].y = 0; alienClips[1].w = Alien::ALIEN_WIDTH; alienClips[1].h = Alien::ALIEN_HEIGHT;
	alienClips[2].x = Alien::ALIEN_WIDTH*2; alienClips[2].y = 0; alienClips[2].w = Alien::ALIEN_WIDTH; alienClips[2].h = Alien::ALIEN_HEIGHT;
	alienClips[3].x = Alien::ALIEN_WIDTH*3; alienClips[3].y = 0; alienClips[3].w = Alien::ALIEN_WIDTH; alienClips[3].h = Alien::ALIEN_HEIGHT;

	gameOverClips[0].x = 0;            gameOverClips[0].y = 0; gameOverClips[0].w = SCREEN_WIDTH; gameOverClips[0].h = SCREEN_HEIGHT;
	gameOverClips[1].x = SCREEN_WIDTH; gameOverClips[1].y = 0; gameOverClips[1].w = SCREEN_WIDTH; gameOverClips[1].h = SCREEN_HEIGHT;

	youWinClips[0].x = 0;              youWinClips[0].y = 0; youWinClips[0].w = SCREEN_WIDTH; youWinClips[0].h = SCREEN_HEIGHT;
	youWinClips[1].x = SCREEN_WIDTH;   youWinClips[1].y = 0; youWinClips[1].w = SCREEN_WIDTH; youWinClips[1].h = SCREEN_HEIGHT;
	youWinClips[2].x = SCREEN_WIDTH*2; youWinClips[2].y = 0; youWinClips[2].w = SCREEN_WIDTH; youWinClips[2].h = SCREEN_HEIGHT;
	youWinClips[3].x = SCREEN_WIDTH*3; youWinClips[3].y = 0; youWinClips[3].w = SCREEN_WIDTH; youWinClips[3].h = SCREEN_HEIGHT;
	youWinClips[4].x = SCREEN_WIDTH*4; youWinClips[4].y = 0; youWinClips[4].w = SCREEN_WIDTH; youWinClips[4].h = SCREEN_HEIGHT;
	youWinClips[5].x = SCREEN_WIDTH*5; youWinClips[5].y = 0; youWinClips[5].w = SCREEN_WIDTH; youWinClips[5].h = SCREEN_HEIGHT;
	youWinClips[6].x = SCREEN_WIDTH*6; youWinClips[6].y = 0; youWinClips[6].w = SCREEN_WIDTH; youWinClips[6].h = SCREEN_HEIGHT;
	youWinClips[7].x = SCREEN_WIDTH*7; youWinClips[7].y = 0; youWinClips[7].w = SCREEN_WIDTH; youWinClips[7].h = SCREEN_HEIGHT;
	youWinClips[8].x = SCREEN_WIDTH*8; youWinClips[8].y = 0; youWinClips[8].w = SCREEN_WIDTH; youWinClips[8].h = SCREEN_HEIGHT;

}
Game::~Game() {
	close();
}
void Game::loadMedia() {
	if(!playerAnimationTexture.loadFromFile("images/player.bmp"))      {std::cout << "Couldn't load player.bmp" << std::endl;}
	if(!backgroundTexture.loadFromFile("images/space.png"))            {std::cout << "Couldn't load space.png" << std::endl;}
	if(!gameOverTexture.loadFromFile("images/gameOver.bmp"))           {std::cout << "Couldn't load gameOver.bmp" << std::endl;}
	if(!playerLazerTexture.loadFromFile("images/playerLazer.bmp"))     {std::cout << "Couldn't load playerLazer.bmp" << std::endl;}
	if(!alienAnimationTexture.loadFromFile("images/animatedalien.bmp")){std::cout << "Couldn't load animatedalien.bmp" << std::endl;}
	if(!alienLazerTexture.loadFromFile("images/alienLazer.bmp"))       {std::cout << "Couldn't load playerLazer.bmp for alienLazer" << std::endl;}
	if(!youWinTexture.loadFromFile("images/victory.bmp"))               {std::cout << "Couldn't load YOU WIN screen" << std::endl;}
	if(!mainMenuTexture.loadFromFile("images/spaceIntrudersMenu.bmp")) {std::cout << "Couldn't load menu overlay." << std::endl;}
	if(!scoreTexture.loadFromRenderedText("Score:", white))            {std::cout << "Couldn't load text." << std::endl;}
	if(!fighterTexture.loadFromFile("images/alienFighter.bmp")) {std::cout << "Couldn't load alienFghter image." << std::endl;}
}
/*Close: frees textures, destroys window and renderer, and finally quits SDL */
void Game::close() {
	//free textures
	fighterTexture.free();
	playerAnimationTexture.free();
	backgroundTexture.free();	
	alienAnimationTexture.free();
	playerLazerTexture.free();	
	gameOverTexture.free();
	youWinTexture.free();
	alienLazerTexture.free();
	mainMenuTexture.free();
	scoreTexture.free();
	pointsTexture.free();
	//destroy window and renderer
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	//quit
	TTF_Quit();
	IMG_Quit();//quit SDL_Image
	SDL_Quit();//quit SDL
}

void Game::checkCollision() {
	for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {	
		//Check for Lazer-to-Alien Collision
		if (pLazer.getX() <= alienIterator->getX() + Alien::ALIEN_WIDTH 
			&& pLazer.getX() + PlayerLazer::LAZER_WIDTH >= alienIterator->getX() 
			&& pLazer.getY() >= alienIterator->getY() - Alien::ALIEN_HEIGHT 
			&& pLazer.getY() - PlayerLazer::LAZER_HEIGHT <= alienIterator->getY())
		{
			score += 10;
			alienIterator->setCollisionState(true);	
			pLazer.reset();
			//TODO alienIterator->explode();
		}
		
		if (alienIterator->getY() >= 430) {
				// EARTH IS DESTROYED!
				dead = true;
				return;
		}
		//Check for Player-to-AlienLazer collision
		if (player.getX() <= alienIterator->getLazerPositX() + AlienLazer::LAZER_WIDTH
			&& player.getX() + Player::PLAYER_WIDTH >= alienIterator->getLazerPositX()
			&& player.getY() >= alienIterator->getLazerPositY() - AlienLazer::LAZER_HEIGHT
			&& player.getY() - Player::PLAYER_HEIGHT <= alienIterator->getLazerPositY())
		{
			dead = true;
			return;
		}
		
	}//end (alienVector loop)
	for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {	
		//Check for Lazer-to-Alien Collision
		if (pLazer.getX() <= fighterIterator->getX() + AlienFighter::ALIEN_WIDTH 
			&& pLazer.getX() + PlayerLazer::LAZER_WIDTH >= fighterIterator->getX() 
			&& pLazer.getY() >= fighterIterator->getY() - AlienFighter::ALIEN_HEIGHT 
			&& pLazer.getY() - PlayerLazer::LAZER_HEIGHT <= fighterIterator->getY())
		{
			score += 20;
			fighterIterator->setCollisionState(true);	
			pLazer.reset();
			//TODO alienIterator->explode();
		}
		
		if (fighterIterator->getY() >= 430) {
				// EARTH IS DESTROYED!
				dead = true;
				return;
		}
		//Check for Player-to-AlienLazer collision
		if (player.getX() <= fighterIterator->getLazerPositX() + AlienLazer::LAZER_WIDTH
			&& player.getX() + Player::PLAYER_WIDTH >= fighterIterator->getLazerPositX()
			&& player.getY() >= fighterIterator->getLazerPositY() - AlienLazer::LAZER_HEIGHT
			&& player.getY() - Player::PLAYER_HEIGHT <= fighterIterator->getLazerPositY())
		{
			dead = true;
			return;
		}
		
	}//end (alienVector loop)
}
void Game::checkAlienMove() {
	//move the alien according to frameCount and dropCount
	for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {
		if (frameCount < GAME_SPEED/2) {//we're moving left
			if (alienIterator->getX() <= 0) {
				//an alien has hit the left wall, so let's change the frameCount so that we now move right
				frameCount = GAME_SPEED/2;
				wallHit = true;
				drop = true;
			}
		}
		else if (frameCount >= GAME_SPEED/2) {//we're moving right
			if (SCREEN_WIDTH - (Alien::ALIEN_WIDTH + alienIterator->getX()) <= 0) {
				//an alien has hit the right wall, so let's change the frameCount so that we now move left
				frameCount = 0;
				wallHit = true;
				drop = true;
			}
		}
	}
	//move the alienFighters according to frameCount and dropCount
	for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
		if (frameCount < GAME_SPEED/2) {//we're moving left
			if (fighterIterator->getX() <= 0) {
				//an alienFighter has hit the left wall, so let's change the frameCount so that we now move right
				frameCount = GAME_SPEED/2;
				wallHit = true;
				drop = true;
			}
		}
		else if (frameCount >= GAME_SPEED/2) {//we're moving right
			if (SCREEN_WIDTH - (AlienFighter::ALIEN_WIDTH + fighterIterator->getX()) <= 0) {
				//an alien has hit the right wall, so let's change the frameCount so that we now move left
				frameCount = 0;
				wallHit = true;
				drop = true;
			}
		}
	}
}
void Game::fireAlienLazers() {
	if (!alienVector.empty() && !alienFighterVector.empty()) {//otherwise aliens fire randomly
		int randomNum = rand() % alienVector.size();//random number between 0 and alienVector size
		int randomFightNum = rand() % alienFighterVector.size();
		if (randomNum % 5 == 0) {//only use randoms that are equivalent to 0 mod 5
			//decidedly, it is too difficult for the player if all aliens fire lazers
			/*if (!alienVector[randomNum].getFired()) {
				alienVector[randomNum].fireLazer();
				alienVector[randomNum].setLazerX(alienVector[randomNum].getX()+(Alien::ALIEN_WIDTH/2));
				alienVector[randomNum].setLazerY(alienVector[randomNum].getY()+(Alien::ALIEN_HEIGHT));
			}*/
			if (!alienFighterVector[randomFightNum].getFired()) {
				alienFighterVector[randomFightNum].fireLazer();
				alienFighterVector[randomFightNum].setLazerX(alienFighterVector[randomFightNum].getX()+(AlienFighter::ALIEN_WIDTH/2));
				alienFighterVector[randomFightNum].setLazerY(alienFighterVector[randomFightNum].getY()+(AlienFighter::ALIEN_HEIGHT));
			}
		}
	}
}

void Game::moveAliens() {
	if (wallHit == false && drop == false) {//TODO we can make this a seperate function (many parameters, and dropCount should be passed by ref)
		for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {
			if (frameCount < GAME_SPEED/2) {//move every alien left	every few frames
				if (frameCount % 5 == 0) {
					alienIterator->move(-(dropCount));
				}
			}
			else if (frameCount >= GAME_SPEED/2) {//move every alien right every few frames
				if (frameCount % 5 == 0) {
					alienIterator->move(dropCount);
				}
			}
		}
		for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
			if (frameCount < GAME_SPEED/2) {
				if (frameCount % 5 == 0) {
					fighterIterator->move(-(dropCount));
				}
			}
			else if (frameCount >= GAME_SPEED/2) {
				if (frameCount % 5 == 0) {
					fighterIterator->move(dropCount);
				}
			}
		}
	}
	wallHit = false;

}

void Game::moveAlienLazers() {
	//move alien lazers
	for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {
		if (alienIterator->getFired()) {
			if (!(alienIterator->moveLazer())) {
				alienIterator->resetLazer();
			}
		}
	}
	for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
		if (fighterIterator->getFired()) {
			if (!(fighterIterator->moveLazer())) {
				fighterIterator->resetLazer();
			}
 		}
	}
}
void Game::dropAliens() {
	//drop aliens down a row when they've reached the screen edge
	if (drop) {
		drop = false;
		for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {
			alienIterator->drop();
		}
		for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
			fighterIterator->drop();
		}
		dropCount++;
	}
}

void Game::getUserInput() {//get user input
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_SPACE]) {
		if (pLazer.getFired() == false) {
			pLazer.setX(player.getX());
			pLazer.fire();
		}
	}
	while(SDL_PollEvent(&event) != 0) {//remove an event from the event queue and process it
		if(event.type == SDL_QUIT) {//exit requested by user
			quit = true;
		}
		player.handleEvent(event);//player space ship events handled in player function
	}
}

void Game::updateGameState() {

	srand(time(NULL));//seed random number generator
	//update current player score
	std::string scoreString;
	std::ostringstream convert;
	convert << score;
	scoreString = convert.str();
	if(!pointsTexture.loadFromRenderedText(scoreString, white)) {std::cout << "Couldn't load score text." << std::endl;}
	
	//update player lazer location
	if (!(pLazer.move())) {//try to move the player's lazer. If move is invalid (lazer goes off screen), reset it.
		pLazer.reset();
	}
	//check aliens for lazer collision or arrival at Earth
	checkCollision();
	
	//erase dead aliens
	alienVector.erase( remove_if( alienVector.begin(), alienVector.end(),
		[](Alien alien) {return alien.getCollisionState();}), alienVector.end());
	alienFighterVector.erase( remove_if( alienFighterVector.begin(), alienFighterVector.end(),
		[](AlienFighter alienFighter) {return alienFighter.getCollisionState();}), alienFighterVector.end());
	
	//check for empty alienVector (game is won)
	if (alienVector.empty() && alienFighterVector.empty()) {
		victory = true;
		return;
	}

	fireAlienLazers();

	//move the player according to keyboard input
	player.move();

	checkAlienMove();

	moveAliens();
	moveAlienLazers();

	dropAliens();
	frameCount++;
	if (frameCount >= GAME_SPEED) {frameCount = 0;}
	--scrollingOffset;
	if(scrollingOffset < -backgroundTexture.getHeight()) {
		scrollingOffset = 0;
	}
}
void Game::drawVictoryScreen() {
	int victoryFrame = 0;
	while (victory) {
		SDL_RenderClear(renderer);
		backgroundTexture.render(0, scrollingOffset);
		backgroundTexture.render(0, scrollingOffset + backgroundTexture.getHeight());

		SDL_Rect* youWinClip = &youWinClips[victoryFrame/VICTORY_ANIMATION_FRAMES];
		youWinTexture.render(0,0,youWinClip);
		++frame;
		++victoryFrame;
		if (victoryFrame/VICTORY_ANIMATION_FRAMES == VICTORY_ANIMATION_FRAMES) {victoryFrame = 0;}
		if (frame/FRAME_RATE == NUM_ANIMATION_FRAMES) {frame = 0;}
		
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				close();
				exit(0);
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				restart = true;
				return;
			}
		}
	SDL_RenderPresent(renderer);
	}
}

void Game::drawDeadScreen() {
	while (dead) {
		//UNNEEDED //SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		backgroundTexture.render(0, scrollingOffset);
		backgroundTexture.render(0, scrollingOffset + backgroundTexture.getHeight());
		SDL_Rect* currentAlienClip = &alienClips[frame/FRAME_RATE];
		//render each alien in the vector
		for ( alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++ alienIterator ) {
			alienIterator -> render( alienIterator->getX(), alienIterator->getY(), currentAlienClip);
		}
		for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
			fighterIterator->render(fighterIterator->getX(), fighterIterator->getY());
		}
		SDL_Rect* currentGameOverClip = &gameOverClips[(frame/2)/FRAME_RATE];
		gameOverTexture.render(0,0,currentGameOverClip);
		++frame;
		if (frame/FRAME_RATE == NUM_ANIMATION_FRAMES) {
			frame = 0;
		}
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				close();
				exit(0);
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				restart = true;
				return;
			}
		}
		SDL_RenderPresent(renderer);
	}
}

void Game::drawGame() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	backgroundTexture.render(0, scrollingOffset);
	backgroundTexture.render(0, scrollingOffset + backgroundTexture.getHeight());
	scoreTexture.render(SCREEN_WIDTH-scoreTexture.getWidth()-100,SCREEN_HEIGHT-scoreTexture.getHeight());
	pointsTexture.render(SCREEN_WIDTH-pointsTexture.getWidth(),SCREEN_HEIGHT-pointsTexture.getHeight());

	drawVictoryScreen();

	drawDeadScreen();

	if (!dead && !victory) {
		SDL_Rect* playerClip = &playerClips[frame/FRAME_RATE];
		player.render(playerClip);
		//set the alien current clip
		SDL_Rect* currentAlienClip = &alienClips[frame/FRAME_RATE];	
		//render each alien in the vector
		for ( alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++ alienIterator ) {
			if (alienIterator->getFired()) {
				alienIterator->renderLazer();//TODO replace with independent lazer class				
			}
			alienIterator->render(alienIterator->getX(), alienIterator->getY(), currentAlienClip);
		}
		for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
			if (fighterIterator->getFired()) {
				fighterIterator->renderLazer();
			}
			fighterIterator->render(fighterIterator->getX(), fighterIterator->getY());
		}
		if (pLazer.getFired()) {
			pLazer.render();
		}
	}
	++frame;
	if (frame/FRAME_RATE == NUM_ANIMATION_FRAMES) {
		frame = 0;
	}

	SDL_RenderPresent(renderer);//refresh renderer
}

int Game::time_left() {
	int now;
	now = SDL_GetTicks();
	if(next_time <= now) {
		return 0;
	} else {
		return next_time - now;
	}
}

bool Game::getRestart() {
	return this->restart;
}
void Game::setRestart(bool x) {
	this->restart = x;
}
bool Game::getVictory() {
	return this->victory;
}
bool Game::getDead() {
	return this->dead;
}

void Game::setUpLevelTwo() {
	quit = false;
	drop = false;
	player.setVelocX(0);
	alienCount += 1;
	alienFighterCount += 2;
	alienFighterVector.clear();
	alienVector.clear();
	alienFighterVector.resize(alienFighterCount);
	fighterIterator = alienFighterVector.begin();
	alienVector.resize(alienCount);
	alienIterator = alienVector.begin();
	int i = 0;
	for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {
		alienIterator->levelTwoAlienLayout(i);
		i++;
	}
	i = 0;
	for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
		fighterIterator->levelTwoAlienLayout(i);
		i++;
	}
	scrollingOffset = 0;
	frame = 0;
	frameCount = 0;
	dropCount = 1;
	wallHit = false;
	dead = false;
	victory = false;
	score = 0;
}

void Game::setUpLevelOne() {
	
	quit = false;
	drop = false;
	alienVector.clear();
	alienFighterVector.clear();
	player.setVelocX(0);
	alienFighterVector.resize(alienFighterCount);
	fighterIterator = alienFighterVector.begin();
	alienVector.resize(alienCount);
	alienIterator = alienVector.begin();
	int i = 0;
	for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {
		alienIterator->levelOneAlienLayout(i);
		i++;
	}
	i = 0;
	for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
		fighterIterator->levelOneAlienLayout(i);
		i++;
	}
	scrollingOffset = 0;
	frame = 0;
	frameCount = 0;
	dropCount = 1;
	wallHit = false;
	dead = false;
	victory = false;
	score = 0;
}

void Game::mainMenu() {
	bool enter = false;
	int menuFrame = 0;
	while(!enter) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		backgroundTexture.render(0, scrollingOffset);
		backgroundTexture.render(0, scrollingOffset + backgroundTexture.getHeight());
		SDL_Rect* mainMenuClip = &menuClips[menuFrame/MENU_ANIMATION_FRAMES];
		mainMenuTexture.render(0,0,mainMenuClip);
		++frame;
		++menuFrame;
		//reset counters if limit reached
		if (menuFrame/MENU_ANIMATION_FRAMES == MENU_ANIMATION_FRAMES) {menuFrame = 0;}
		if (frame/FRAME_RATE == NUM_ANIMATION_FRAMES) {frame = 0;}
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				close();
				exit(0);
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				enter = true;
			}
		}
		--scrollingOffset;
		if(scrollingOffset < -backgroundTexture.getHeight()) {
			scrollingOffset = 0;
		}
		SDL_RenderPresent(renderer);
	}
}

void Game::main() {
	
	while(!quit && !restart && !dead) {//MAIN GAME LOOP
		getUserInput();
		updateGameState();
		SDL_Delay(time_left());
		next_time += TICK_INTERVAL;
		drawGame();
	}
	//close();//free all textures and exit
}

int main(int argc, char* args[]) {
	Game g;
	//if (restart) {g.setUpLevelTwo();}
	g.mainMenu();
	do {
		g.setRestart(false);
		g.main();
		if (g.getRestart()) {
			if (g.getVictory()) {
				std::cout << "Going to level two" << std::endl;
				g.setUpLevelTwo();
			}
			if (g.getDead()) {
				g.setUpLevelOne();
				g.main();
			}
		}
	} while (g.getRestart());
	return 0;
}
