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
const int PAUSE_ANIM_FRAMES = 6;
const int GAMEOVER_FRAMES = 2;
const int VICTORY_ANIMATION_FRAMES = 9;
const int FRAME_RATE = 10;

//game speed constants
const int GAME_SPEED = 5000;
const int TICK_INTERVAL = 20;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

Mix_Chunk *lazerSound = NULL;
Mix_Chunk *soundNote = NULL;

//instantiate game textures
ImageTexture playerAnimationTexture;
ImageTexture backgroundTexture;
ImageTexture playerLazerTexture;
ImageTexture alienAnimationTexture;
ImageTexture gameOverTexture;
ImageTexture youWinTexture;
ImageTexture alienLazerTexture;
ImageTexture mainMenuTexture;
ImageTexture fighterTexture;
ImageTexture scoreTexture;
ImageTexture pointsTexture;
ImageTexture pauseTexture;

SDL_Rect alienClips[NUM_ANIMATION_FRAMES];
SDL_Rect gameOverClips[GAMEOVER_FRAMES];
SDL_Rect youWinClips[VICTORY_ANIMATION_FRAMES];
SDL_Rect playerClips[NUM_ANIMATION_FRAMES];
SDL_Rect menuClips[MENU_ANIMATION_FRAMES];
SDL_Rect fighterClips[NUM_ANIMATION_FRAMES];
SDL_Rect pauseClips[MENU_ANIMATION_FRAMES];

SDL_Color white;
TTF_Font *font;


ImageTexture::ImageTexture() {
	sdlTexture = NULL;
	sdlTextureWidth = 0;
	sdlTextureHeight = 0;
}

bool ImageTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	free();
	SDL_Surface* loadedSurface = TTF_RenderText_Solid(font,textureText.c_str(),textColor);
	sdlTexture = SDL_CreateTextureFromSurface(renderer,loadedSurface);
	sdlTextureWidth = loadedSurface->w;
	sdlTextureHeight = loadedSurface->h;
	SDL_FreeSurface(loadedSurface);
	return (sdlTexture != NULL);
}

ImageTexture::~ImageTexture() {
	free();
}

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

int Player::getX() {
	return positX;
}//player X position getter

int Player::getY() {
	return positY;
}//player Y position getter

void Player::render(SDL_Rect* playerClip) {
	playerAnimationTexture.render(positX, positY, playerClip);
}

void Player::setVelocX(int newVelocity) {
	this->velocX = newVelocity;
}

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

Alien::Alien() {
	positX = 0;
	positY = 0;
	collision = false;
}

void Alien::levelOneAlienLayout(int i) {
	int aliensPerRow = 8;
	this->positX = 60 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 110 + (i / aliensPerRow)*(5 + Alien::ALIEN_HEIGHT);
}

void Alien::levelTwoAlienLayout(int i) {
	int aliensPerRow = 9;
	this->positX = 60 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 110 + (i / aliensPerRow)*(5 + Alien::ALIEN_HEIGHT);
}

void Alien::levelThreeAlienLayout(int i) {
	int aliensPerRow = 8;
	this->positX = 60 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 140 + (i / aliensPerRow)*(5 + Alien::ALIEN_HEIGHT);
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

AlienFighter::AlienFighter() {
	positX = 0;
	positY = 0;
	collision = false;
}

void AlienFighter::levelOneAlienLayout(int i) {
	int aliensPerRow = 8;
	this->positX = 63 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 25 + (i / aliensPerRow)*(10 + Alien::ALIEN_HEIGHT);
}
void AlienFighter::levelTwoAlienLayout(int i) {
	int aliensPerRow = 9;
	this->positX = 63 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 25 + (i / aliensPerRow)*(10 + Alien::ALIEN_HEIGHT);
}

void AlienFighter::levelThreeAlienLayout(int i) {
	int aliensPerRow = 8;
	this->positX = 63 + (i % aliensPerRow)*(20+Alien::ALIEN_WIDTH);
	this->positY = 20 + (i / aliensPerRow)*(10+Alien::ALIEN_HEIGHT);
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

void AlienFighter::render(int x, int y, SDL_Rect* currentClip) {
	fighterTexture.render(x, y, currentClip);
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

/*Construct the game, intialize all data members.*/
Game::Game() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cout << "SDL did not initialize... Exiting..." << std::endl;
		close();
		exit(1);
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");//rendering quality = 1
	
	window = SDL_CreateWindow("Space Intruders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
		close();
		exit(1);
	}
	
	//create the vsynced renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		std::cout << "Failed to create renderer." << std::endl;
		close();
		exit(1);
	}
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	imgFlags = IMG_INIT_PNG;
	IMG_Init(imgFlags);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	TTF_Init();

	font = TTF_OpenFont("Munro.ttf", 20);
	white = {255,255,255};
	if (font == NULL) {
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
	level = 1;
	pause = false;

	menuClips[0].x = 0;              menuClips[0].y = 0; menuClips[0].w = SCREEN_WIDTH; menuClips[0].h = SCREEN_HEIGHT;
	menuClips[1].x = SCREEN_WIDTH;   menuClips[1].y = 0; menuClips[1].w = SCREEN_WIDTH; menuClips[1].h = SCREEN_HEIGHT;
	menuClips[2].x = SCREEN_WIDTH*2; menuClips[2].y = 0; menuClips[2].w = SCREEN_WIDTH; menuClips[2].h = SCREEN_HEIGHT;
	menuClips[3].x = SCREEN_WIDTH*3; menuClips[3].y = 0; menuClips[3].w = SCREEN_WIDTH; menuClips[3].h = SCREEN_HEIGHT;
	menuClips[4].x = SCREEN_WIDTH*4; menuClips[4].y = 0; menuClips[4].w = SCREEN_WIDTH; menuClips[4].h = SCREEN_HEIGHT;
	menuClips[5].x = SCREEN_WIDTH*5; menuClips[5].y = 0; menuClips[5].w = SCREEN_WIDTH; menuClips[5].h = SCREEN_HEIGHT;


	pauseClips[0].x = 0;              pauseClips[0].y = 0; pauseClips[0].w = SCREEN_WIDTH; pauseClips[0].h = SCREEN_HEIGHT;
	pauseClips[1].x = SCREEN_WIDTH;   pauseClips[1].y = 0; pauseClips[1].w = SCREEN_WIDTH; pauseClips[1].h = SCREEN_HEIGHT;
	pauseClips[2].x = SCREEN_WIDTH*2; pauseClips[2].y = 0; pauseClips[2].w = SCREEN_WIDTH; pauseClips[2].h = SCREEN_HEIGHT;
	pauseClips[3].x = SCREEN_WIDTH*3; pauseClips[3].y = 0; pauseClips[3].w = SCREEN_WIDTH; pauseClips[3].h = SCREEN_HEIGHT;
	pauseClips[4].x = SCREEN_WIDTH*4; pauseClips[4].y = 0; pauseClips[4].w = SCREEN_WIDTH; pauseClips[4].h = SCREEN_HEIGHT;
	pauseClips[5].x = SCREEN_WIDTH*5; pauseClips[5].y = 0; pauseClips[5].w = SCREEN_WIDTH; pauseClips[5].h = SCREEN_HEIGHT;

	playerClips[0].x = 0;                      playerClips[0].y = 0; playerClips[0].w = Player::PLAYER_WIDTH; playerClips[0].h = Player::PLAYER_HEIGHT;
	playerClips[1].x = Player::PLAYER_WIDTH;   playerClips[1].y = 0; playerClips[1].w = Player::PLAYER_WIDTH; playerClips[1].h = Player::PLAYER_HEIGHT;
	playerClips[2].x = Player::PLAYER_WIDTH*2; playerClips[2].y = 0; playerClips[2].w = Player::PLAYER_WIDTH; playerClips[2].h = Player::PLAYER_HEIGHT;
	playerClips[3].x = Player::PLAYER_WIDTH*3; playerClips[3].y = 0; playerClips[3].w = Player::PLAYER_WIDTH; playerClips[3].h = Player::PLAYER_HEIGHT;

	//alien animation clip data
	alienClips[0].x = 0;                    alienClips[0].y = 0; alienClips[0].w = Alien::ALIEN_WIDTH; alienClips[0].h = Alien::ALIEN_HEIGHT;
	alienClips[1].x = Alien::ALIEN_WIDTH;   alienClips[1].y = 0; alienClips[1].w = Alien::ALIEN_WIDTH; alienClips[1].h = Alien::ALIEN_HEIGHT;
	alienClips[2].x = Alien::ALIEN_WIDTH*2; alienClips[2].y = 0; alienClips[2].w = Alien::ALIEN_WIDTH; alienClips[2].h = Alien::ALIEN_HEIGHT;
	alienClips[3].x = Alien::ALIEN_WIDTH*3; alienClips[3].y = 0; alienClips[3].w = Alien::ALIEN_WIDTH; alienClips[3].h = Alien::ALIEN_HEIGHT;

	fighterClips[0].x = 0;                           fighterClips[0].y = 0; fighterClips[0].w = AlienFighter::ALIEN_WIDTH; fighterClips[0].h = AlienFighter::ALIEN_HEIGHT;
	fighterClips[1].x = AlienFighter::ALIEN_WIDTH;   fighterClips[1].y = 0; fighterClips[1].w = AlienFighter::ALIEN_WIDTH; fighterClips[1].h = AlienFighter::ALIEN_HEIGHT;
	fighterClips[2].x = AlienFighter::ALIEN_WIDTH*2; fighterClips[2].y = 0; fighterClips[2].w = AlienFighter::ALIEN_WIDTH; fighterClips[2].h = AlienFighter::ALIEN_HEIGHT;
	fighterClips[3].x = AlienFighter::ALIEN_WIDTH*3; fighterClips[3].y = 0; fighterClips[3].w = AlienFighter::ALIEN_WIDTH; fighterClips[3].h = AlienFighter::ALIEN_HEIGHT;

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

int Game::getLevel() {
	return this->level;
}

Game::~Game() {
	close();
}
void Game::loadMedia() {
	if(!playerAnimationTexture.loadFromFile("images/player.bmp")) {
		std::cout << "Couldn't load player.bmp" << std::endl;
	}
	if(!backgroundTexture.loadFromFile("images/space.png")) {
		std::cout << "Couldn't load space.png" << std::endl;
	}
	if(!gameOverTexture.loadFromFile("images/gameOver.bmp")) {
		std::cout << "Couldn't load gameOver.bmp" << std::endl;
	}
	if(!playerLazerTexture.loadFromFile("images/playerLazer.bmp")) {
		std::cout << "Couldn't load playerLazer.bmp" << std::endl;
	}
	if(!alienAnimationTexture.loadFromFile("images/animatedalien.bmp")) {
		std::cout << "Couldn't load animatedalien.bmp" << std::endl;
	}
	if(!alienLazerTexture.loadFromFile("images/alienLazer.bmp")) {
		std::cout << "Couldn't load playerLazer.bmp for alienLazer" << std::endl;
	}
	if(!youWinTexture.loadFromFile("images/victory.bmp")) {
		std::cout << "Couldn't load YOU WIN screen" << std::endl;
	}
	if(!mainMenuTexture.loadFromFile("images/spaceIntrudersMenu.bmp")) {
		std::cout << "Couldn't load menu overlay." << std::endl;
	}
	if(!scoreTexture.loadFromRenderedText("Score:", white)) {
		std::cout << "Couldn't load text." << std::endl;
	}
	if(!fighterTexture.loadFromFile("images/animatedFighter2.bmp")) {
		std::cout << "Couldn't load alienFghter image." << std::endl;
	}
	if(!pauseTexture.loadFromFile("images/spaceIntrudersMenu.bmp")) {
		std::cout << "Couldn't load pause menu image."<< std::endl;
	}
	soundNote = Mix_LoadWAV("sounds/beep.wav");
	if (soundNote == NULL) {
		std::cout << "Couldn't load sound file." << std::endl;
	}
	lazerSound = Mix_LoadWAV("sounds/lazer.wav");
	if (lazerSound == NULL) {
		std::cout << "Couldn't load lazerSound file." << std::endl;
	}
}
/*Close: frees textures, destroys window and renderer, and finally quits SDL */
void Game::close() {
	//free textures
	Mix_FreeChunk(soundNote);
	Mix_FreeChunk(lazerSound);
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
	pauseTexture.free();

	//destroy window and renderer
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	//quit
	TTF_Quit();
	Mix_Quit();
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
		}
		
		if (alienIterator->getY() >= SCREEN_HEIGHT - Player::PLAYER_HEIGHT - Alien::ALIEN_HEIGHT) {
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
		
		if (fighterIterator->getY() >= SCREEN_HEIGHT - Player::PLAYER_HEIGHT - AlienFighter::ALIEN_HEIGHT) {
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
void Game::checkAlienHord() {
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
	if (!alienFighterVector.empty()) {//otherwise aliens fire randomly
		int randomNum = rand() % alienFighterVector.size();
		if (alienVector.size() != 0) {
			int randomNum = rand() % alienVector.size();//random number between 0 and alienVector size
		}
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

void Game::playSound() {
	if (dropCount < 5) {
		if (frameCount % 50 == 0) {
			Mix_PlayChannel(-1, soundNote, 0);
		}
	}
	else if (dropCount < 10) {
		if (frameCount % 35 == 0)
			Mix_PlayChannel(-1, soundNote, 0);
	} else {
		if (frameCount % 20 == 0) {
			Mix_PlayChannel(-1, soundNote, 0);
		} 
	}
}

void Game::moveAlienHord() {
	if (wallHit == false && drop == false) {//TODO we can make this a seperate function (many parameters, and dropCount should be passed by ref)
		for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {
			if (frameCount < GAME_SPEED/2) {//move every alien left	every few frames
				if (frameCount % 5 == 0) {
					alienIterator->move((-((dropCount+1*3)/2)));
				}
			}
			else if (frameCount >= GAME_SPEED/2) {//move every alien right every few frames
				if (frameCount % 5 == 0) {
					alienIterator->move(((dropCount+1*3)/2));
				}
			}
		}
		for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
			if (frameCount < GAME_SPEED/2) {
				if (frameCount % 5 == 0) {
					fighterIterator->move((-((dropCount+1*3)/2)));
				}
			}
			else if (frameCount >= GAME_SPEED/2) {
				if (frameCount % 5 == 0) {
					fighterIterator->move(((dropCount+1*3)/2));
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
void Game::dropAlienHord() {
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
		if (alienVector.size() < alienCount/2) {
			dropCount ++;
		}
		if (alienVector.size() < 5 && alienFighterVector.size() < alienFighterCount/2) {
			dropCount ++;
		}
	}
}

void Game::getUserInput() {//get user input
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_SPACE]) {
		if (pLazer.getFired() == false) {
			pLazer.setX(player.getX());
			pLazer.fire();
			Mix_PlayChannel(-1, lazerSound, 0);
		}
	}
	while(SDL_PollEvent(&event) != 0) {//remove an event from the event queue and process it
		if(event.type == SDL_QUIT) {//exit requested by user
			quit = true;
		}
		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
			pause = true;
			return;
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
	if(!pointsTexture.loadFromRenderedText(scoreString, white)) {
		std::cout << "Couldn't load score text." << std::endl;
	}
	
	//update player lazer location
	if (!(pLazer.move())) {//try to move the player's lazer. If move is invalid (lazer goes off screen), reset it.
		pLazer.reset();
	}
	//check aliens for lazer collision or arrival at Earth
	checkCollision();
	
	//erase dead aliens
	alienVector.erase( remove_if( alienVector.begin(), alienVector.end(),
		[](Alien alien) {return alien.getCollisionState();}), alienVector.end());
	//erase dead alien Fighters
	alienFighterVector.erase( remove_if( alienFighterVector.begin(), alienFighterVector.end(),
		[](AlienFighter alienFighter) {return alienFighter.getCollisionState();}), alienFighterVector.end());
	
	//check for empty alienVector (game is won)
	if (alienVector.empty() && alienFighterVector.empty()) {
		victory = true;
		return;
	}

	fireAlienLazers();
	player.move();
	checkAlienHord();
	playSound();
	moveAlienHord();
	moveAlienLazers();
	dropAlienHord();
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
		scoreTexture.render(SCREEN_WIDTH-scoreTexture.getWidth()-70,SCREEN_HEIGHT-scoreTexture.getHeight());
		pointsTexture.render(SCREEN_WIDTH-pointsTexture.getWidth(),SCREEN_HEIGHT-pointsTexture.getHeight());
		SDL_Rect* youWinClip = &youWinClips[victoryFrame/VICTORY_ANIMATION_FRAMES];
		youWinTexture.render(0,0,youWinClip);
		++frame;
		++victoryFrame;
		if (victoryFrame/VICTORY_ANIMATION_FRAMES == VICTORY_ANIMATION_FRAMES) {
			victoryFrame = 0;
		}
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

void Game::drawDeadScreen() {
	while (dead) {
		SDL_RenderClear(renderer);
		backgroundTexture.render(0, scrollingOffset);
		backgroundTexture.render(0, scrollingOffset + backgroundTexture.getHeight());
		SDL_Rect* currentAlienClip = &alienClips[frame/FRAME_RATE];
		SDL_Rect* currentFighterClip = &fighterClips[frame/FRAME_RATE];
		//render each alien in the vector
		for ( alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++ alienIterator ) {
			alienIterator -> render( alienIterator->getX(), alienIterator->getY(), currentAlienClip);
		}
		for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
			fighterIterator->render(fighterIterator->getX(), fighterIterator->getY(), currentFighterClip);
		}
		gameOverTexture.render(0,0);
		scoreTexture.render(SCREEN_WIDTH-scoreTexture.getWidth()-70,SCREEN_HEIGHT-scoreTexture.getHeight());
		pointsTexture.render(SCREEN_WIDTH-pointsTexture.getWidth(),SCREEN_HEIGHT-pointsTexture.getHeight());
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
	scoreTexture.render(SCREEN_WIDTH-scoreTexture.getWidth()-70,SCREEN_HEIGHT-scoreTexture.getHeight());
	pointsTexture.render(SCREEN_WIDTH-pointsTexture.getWidth(),SCREEN_HEIGHT-pointsTexture.getHeight());
	if (victory) {
		drawVictoryScreen();
	}
	if (dead) {
		drawDeadScreen();
	}
	if (pause) {
		drawPauseScreen();
	}
	if (!dead && !victory && !pause) {
		SDL_Rect* playerClip = &playerClips[frame/FRAME_RATE];
		player.render(playerClip);
		//set the alien current clip
		SDL_Rect* currentAlienClip = &alienClips[frame/FRAME_RATE];	
		SDL_Rect* currentFighterClip = &fighterClips[frame/FRAME_RATE];
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
			fighterIterator->render(fighterIterator->getX(), fighterIterator->getY(), currentFighterClip);
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

void Game::setUpLevelOne() {
	pLazer.reset();
	quit = false;
	drop = false;
	alienVector.clear();
	alienFighterVector.clear();
	player.setVelocX(0);
	alienCount = 64;
	alienFighterCount = 16;
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

void Game::setUpLevelTwo() {
	pLazer.reset();
	level = 2;
	quit = false;
	drop = false;
	player.setVelocX(0);
	alienCount = 63;
	alienFighterCount = 18;
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
}

void Game::setUpLevelThree() {
	pLazer.reset();
	quit = false;
	drop = false;
	alienVector.clear();
	alienFighterVector.clear();
	player.setVelocX(0);
	alienCount = 56;
	alienFighterCount = 24;
	alienFighterVector.resize(alienFighterCount);
	fighterIterator = alienFighterVector.begin();
	alienVector.resize(alienCount);
	alienIterator = alienVector.begin();
	int i = 0;
	for (alienIterator = alienVector.begin(); alienIterator != alienVector.end(); ++alienIterator) {
		alienIterator->levelThreeAlienLayout(i);
		i++;
	}
	i = 0;
	for (fighterIterator = alienFighterVector.begin(); fighterIterator != alienFighterVector.end(); ++fighterIterator) {
		fighterIterator->levelThreeAlienLayout(i);
		i++;
	}
	scrollingOffset = 0;
	frame = 0;
	frameCount = 0;
	dropCount = 3;
	wallHit = false;
	dead = false;
	victory = false;
	score = 0;
	level = 3;
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
		if (menuFrame/MENU_ANIMATION_FRAMES == MENU_ANIMATION_FRAMES) {
			menuFrame = 0;
		}
		if (frame/FRAME_RATE == NUM_ANIMATION_FRAMES) {
			frame = 0;
		}
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				close();
				exit(0);
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				enter = true;
				return;
			}
		}
		--scrollingOffset;
		if(scrollingOffset < -backgroundTexture.getHeight()) {
			scrollingOffset = 0;
		}
		SDL_RenderPresent(renderer);
	}
}

void Game::drawPauseScreen() {
	int pauseFrame = 0;
	while (pause) {
		SDL_RenderClear(renderer);
		backgroundTexture.render(0,scrollingOffset);
		backgroundTexture.render(0,scrollingOffset + backgroundTexture.getHeight());
		SDL_Rect* pauseClip = &pauseClips[pauseFrame/PAUSE_ANIM_FRAMES];
		pauseTexture.render(0,0,pauseClip);
		++frame;
		++pauseFrame;
		if (pauseFrame/PAUSE_ANIM_FRAMES == PAUSE_ANIM_FRAMES) {
			pauseFrame = 0;
		}
		if (frame/FRAME_RATE == NUM_ANIMATION_FRAMES) {
			frame = 0;
		}

		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				close();
				exit(0);
			}
			if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN)) {
				pause = false;
				return;
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
				if (g.getLevel() == 1) {
					g.setUpLevelTwo();
				} else {
					g.setUpLevelThree();
				}
			}
			if (g.getDead()) {
				if (g.getLevel() == 1) {
					g.setUpLevelOne();//set to lv3 for testing///////////////////////////////////////////////////////////////////////////////
				} else if (g.getLevel() == 2){
					g.setUpLevelTwo();
				} else {
					g.setUpLevelThree();
				}
			}
		}
	} while (g.getRestart());
	return 0;
}