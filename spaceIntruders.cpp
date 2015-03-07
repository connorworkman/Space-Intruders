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
//game speed constants
const int GAMEOVER_FRAMES = 2;
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
//TODO ImageTexture explosionTexture; //explosion texture should be explosion animation
ImageTexture alienFighterTexture;
SDL_Rect alienClips[NUM_ANIMATION_FRAMES];
SDL_Rect gameOverClips[GAMEOVER_FRAMES];
SDL_Rect youWinClips[GAMEOVER_FRAMES];
SDL_Rect playerClips[NUM_ANIMATION_FRAMES];
//TODO add explosion textures, enemy lazer textures?, additional different enemy textures



ImageTexture::ImageTexture() {
	sdlTexture = NULL;
	sdlTextureWidth = 0;
	sdlTextureHeight = 0;
}

ImageTexture::~ImageTexture() {free();}
bool ImageTexture::loadFromFile(std::string path) {
	free();//free previous texture before attempting new load
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	sdlTextureWidth = loadedSurface->w;
	sdlTextureHeight = loadedSurface->h;
	SDL_FreeSurface(loadedSurface);//free old surface
	sdlTexture = newTexture;

	playerClips[0].x = 0;
	playerClips[0].y = 0;
	playerClips[0].w = Player::PLAYER_WIDTH;
	playerClips[0].h = Player::PLAYER_HEIGHT;
	playerClips[1].x = Player::PLAYER_WIDTH;
	playerClips[1].y = 0;
	playerClips[1].w = Player::PLAYER_WIDTH;
	playerClips[1].h = Player::PLAYER_HEIGHT;
	playerClips[2].x = Player::PLAYER_WIDTH*2;
	playerClips[2].y = 0;
	playerClips[2].w = Player::PLAYER_WIDTH;
	playerClips[2].h = Player::PLAYER_HEIGHT;
	playerClips[3].x = Player::PLAYER_WIDTH*3;
	playerClips[3].y = 0;
	playerClips[3].w = Player::PLAYER_WIDTH;
	playerClips[3].h = Player::PLAYER_HEIGHT;

	//alien animation clip data
	alienClips[0].x = 0;
	alienClips[0].y = 0;
	alienClips[0].w = Alien::ALIEN_WIDTH;
	alienClips[0].h = Alien::ALIEN_HEIGHT;
	alienClips[1].x = Alien::ALIEN_WIDTH;
	alienClips[1].y = 0;
	alienClips[1].w = Alien::ALIEN_WIDTH;
	alienClips[1].h = Alien::ALIEN_HEIGHT;
	alienClips[2].x = Alien::ALIEN_WIDTH*2;
	alienClips[2].y = 0;
	alienClips[2].w = Alien::ALIEN_WIDTH;
	alienClips[2].h = Alien::ALIEN_HEIGHT;
	alienClips[3].x = Alien::ALIEN_WIDTH*3;
	alienClips[3].y = 0;
	alienClips[3].w = Alien::ALIEN_WIDTH;
	alienClips[3].h = Alien::ALIEN_HEIGHT;

	gameOverClips[0].x = 0;
	gameOverClips[0].y = 0;
	gameOverClips[0].w = SCREEN_WIDTH;
	gameOverClips[0].h = SCREEN_HEIGHT;
	gameOverClips[1].x = SCREEN_WIDTH;
	gameOverClips[1].y = 0;
	gameOverClips[1].w = SCREEN_WIDTH;
	gameOverClips[1].h = SCREEN_HEIGHT;

	youWinClips[0].x = 0;
	youWinClips[0].y = 0;
	youWinClips[0].w = SCREEN_WIDTH;
	youWinClips[0].h = SCREEN_HEIGHT;
	youWinClips[1].x = SCREEN_WIDTH;
	youWinClips[1].y = 0;
	youWinClips[1].w = SCREEN_WIDTH;
	youWinClips[1].h = SCREEN_HEIGHT;

	//TODO playerAnimationClips[0].x  = 0;

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
        	switch( event.key.keysym.sym ) {
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
 * void initAlienLayout(i) ~ set the initial position for each alien in the level (square arrangement)
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

void Alien::initAlienLayout(int i) {
	int aliensPerRow = 8;
	this->positX = 60 + (i % aliensPerRow)*(20 + Alien::ALIEN_WIDTH);
	this->positY = 30 + (i / aliensPerRow)*(5 + Alien::ALIEN_HEIGHT);
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
//TODO write this class definition and don't forget declarations in alienFighter.hpp
/* AlienFighter Functions: *
 * constructor ~ default settings for the alien fighter
 * destructor is empty
 * getX ~ return X coordinate of the alien fighter
 * getY ~ return Y coordinate of the alien fighter
 * render ~ render the alien fighter at position X,Y
 * move ~ move the alien fighter horizontally until completely off the screen
*/
AlienFighter::AlienFighter() {
	this->positX = SCREEN_WIDTH + 100;
	this->positY = SCREEN_HEIGHT - 50;
	this->velocX = 5;
}
AlienFighter::~AlienFighter() {}
int AlienFighter::getX() {
	return this->positX;
}
int AlienFighter::getY() {
	return this->positY;
}
void AlienFighter::render() {
	alienFighterTexture.render(positX, positY);
}
void AlienFighter::move() {
	positX -= this->velocX;
}

Game::Game() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");//rendering quality = 1
	window = SDL_CreateWindow("Space Intruders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	//create vsynced renderer
	//the renderer uses hardware acceleration
	//present is synchronized with the refresh rate
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	imgFlags = IMG_INIT_PNG;
	IMG_Init(imgFlags);
	loadMedia();
	quit = false;
	drop = false;
	//SDL_Event event;
	//Player player;
	//PlayerLazer pLazer;
	//create vector of standard alien type
	alienVector.resize(alienCount);
	itty = alienVector.begin();
	int i = 0;
	for (itty = alienVector.begin(); itty != alienVector.end(); ++itty) {
		itty->initAlienLayout(i);
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
Game::~Game() {
	close();
}
void Game::loadMedia() {
	if(!playerAnimationTexture.loadFromFile("images/player.bmp")){std::cout << "Couldn't load player.bmp" << std::endl;}
	if(!backgroundTexture.loadFromFile("images/space.png")){std::cout << "Couldn't load space.png" << std::endl;}
	if(!gameOverTexture.loadFromFile("images/gameOver.bmp")){std::cout << "Couldn't load gameOver.bmp" << std::endl;}
	if(!playerLazerTexture.loadFromFile("images/playerLazer.bmp")){std::cout << "Couldn't load playerLazer.bmp" << std::endl;}
	if(!alienAnimationTexture.loadFromFile("images/animatedalien.bmp")){std::cout << "Couldn't load animatedalien.bmp" << std::endl;}
	if(!alienLazerTexture.loadFromFile("images/alienLazer.bmp")) {std::cout << "Couldn't load playerLazer.bmp for alienLazer" << std::endl;}
	if(!youWinTexture.loadFromFile("images/youWin.bmp")) {std::cout << "Couldn't load YOU WIN screen" << std::endl;}
}
/*Close: frees textures, destroys window and renderer, and finally quits SDL */
void Game::close() {
	//free textures
	playerAnimationTexture.free();
	backgroundTexture.free();	
	alienAnimationTexture.free();
	playerLazerTexture.free();	
	gameOverTexture.free();
	youWinTexture.free();
	alienLazerTexture.free();
	//destroy window and renderer
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	//quit
	IMG_Quit();//quit SDL_Image
	SDL_Quit();//quit SDL
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

		//update player lazer location
		if (!(pLazer.move())) {//try to move the player's lazer. If move is invalid (lazer goes off screen), reset it.
			pLazer.reset();
		}
		//check aliens for lazer collision or arrival at Earth
		for (itty = alienVector.begin(); itty != alienVector.end(); ++itty) {	
			//Check for Lazer-to-Alien Collision
			if (pLazer.getX() <= itty->getX() + Alien::ALIEN_WIDTH 
				&& pLazer.getX() + PlayerLazer::LAZER_WIDTH >= itty->getX() 
				&& pLazer.getY() >= itty->getY() - Alien::ALIEN_HEIGHT 
				&& pLazer.getY() - PlayerLazer::LAZER_HEIGHT <= itty->getY())
			{
				itty->setCollisionState(true);	
				pLazer.reset();
				//TODO itty->explode();
			}
			
			if (itty->getY() >= 430) {
					// EARTH IS DESTROYED!
					dead = true;
					return;
			}
			
		}//end (alienVector loop)
		
		//erase instances that have collided with the lazer (marked by collision flag)
		alienVector.erase( remove_if( alienVector.begin(), alienVector.end(),
			[](Alien alien) {return alien.getCollisionState();}), alienVector.end());

		//check for empty alienVector (game is won)
		if (alienVector.empty()) {
			victory = true;
			return;
		} else {
			int randomNum = rand() % alienVector.size();//random number between 0 and alienVector size
			if (randomNum % 5 == 0) {//only use randoms that are equivalent to 0 mod 5
				if (!alienVector[randomNum].getFired()) {
					alienVector[randomNum].fireLazer();
					alienVector[randomNum].setLazerX(alienVector[randomNum].getX()+(Alien::ALIEN_WIDTH/2));
					alienVector[randomNum].setLazerY(alienVector[randomNum].getY()+(Alien::ALIEN_HEIGHT));
				}
			}
		}

		//move the player according to keyboard input
		player.move();

		//move the alien according to frameCount and dropCount
		for (itty = alienVector.begin(); itty != alienVector.end(); ++itty) {
			if (frameCount < GAME_SPEED/2) {//we're moving left
				if (itty->getX() <= 0) {
					//an alien has hit the left wall, so let's change the frameCount so that we now move right
					frameCount = GAME_SPEED/2;
					wallHit = true;
					drop = true;
				}
			}
			else if (frameCount >= GAME_SPEED/2) {//we're moving right
				if (SCREEN_WIDTH - (Alien::ALIEN_WIDTH + itty->getX()) <= 0) {
					//an alien has hit the right wall, so let's change the frameCount so that we now move left
					frameCount = 0;
					wallHit = true;
					drop = true;
				}
			}
		}
		
		if (wallHit == false && drop == false) {//TODO we can make this a seperate function (many parameters, and dropCount should be passed by ref)
			for (itty = alienVector.begin(); itty != alienVector.end(); ++itty) {
				if (frameCount < GAME_SPEED/2) {//move every alien left	every few frames
					if (frameCount % 25 == 0) {
						itty->move(-(dropCount*4));
					}
				}
				else if (frameCount >= GAME_SPEED/2) {//move every alien right every few frames
					if (frameCount % 25 == 0) {
						itty->move(dropCount*4);
					}
				}
			}
		}
		wallHit = false;//reset wallHit boolean
		
		for (itty = alienVector.begin(); itty != alienVector.end(); ++itty) {
			if (itty->getFired()) {
				if (!(itty->moveLazer())) {
					itty->resetLazer();
				}
			}
		}
		//drop aliens down a row when they've reached the screen edge
		if (drop) {
			drop = false;
			for (itty = alienVector.begin(); itty != alienVector.end(); ++itty) {
				itty->drop();
			}
			dropCount++;
		}
		frameCount++;
		if (frameCount >= GAME_SPEED) {frameCount = 0;}
		--scrollingOffset;
		if(scrollingOffset < -backgroundTexture.getHeight()) {
			scrollingOffset = 0;
		}
}
void Game::drawGame() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	backgroundTexture.render(0, scrollingOffset);
	backgroundTexture.render(0, scrollingOffset + backgroundTexture.getHeight());

	while (victory) {
		//UNNEEDED //SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		backgroundTexture.render(0, scrollingOffset);
		backgroundTexture.render(0, scrollingOffset + backgroundTexture.getHeight());
		SDL_Rect* currentAlienClip = &alienClips[frame/FRAME_RATE];
		//render each alien in the vector
		for ( itty = alienVector.begin(); itty != alienVector.end(); ++ itty ) {itty -> render( itty->getX(), itty->getY(), currentAlienClip);}
		SDL_Rect* youWinClip = &youWinClips[(frame/2)/FRAME_RATE];
		youWinTexture.render(0,0,youWinClip);
		++frame;
		if (frame/FRAME_RATE == NUM_ANIMATION_FRAMES) {frame = 0;}
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				close();
				exit(0);
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				close();
				exit(0);
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
				dead = false;
				return;
			}
		}
	SDL_RenderPresent(renderer);
	}

	while (dead) {
			//UNNEEDED //SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(renderer);
			backgroundTexture.render(0, scrollingOffset);
			backgroundTexture.render(0, scrollingOffset + backgroundTexture.getHeight());
			SDL_Rect* currentAlienClip = &alienClips[frame/FRAME_RATE];
			//render each alien in the vector
			for ( itty = alienVector.begin(); itty != alienVector.end(); ++ itty ) {
				itty -> render( itty->getX(), itty->getY(), currentAlienClip);
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
				close();
				exit(0);
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
				dead = false;
				//resetGameState();
				return;
			}
		}
		SDL_RenderPresent(renderer);
	}
	if (!dead && !victory) {
		SDL_Rect* playerClip = &playerClips[frame/FRAME_RATE];
		player.render(playerClip);
		//set the alien current clip
		SDL_Rect* currentAlienClip = &alienClips[frame/FRAME_RATE];	
		//render each alien in the vector
		for ( itty = alienVector.begin(); itty != alienVector.end(); ++ itty ) {
			itty->render(itty->getX(), itty->getY(), currentAlienClip);
			if (itty->getFired()) {
				itty->renderLazer();//TODO replace with independent lazer class
			}
		}
	}
	++frame;
	if (frame/FRAME_RATE == NUM_ANIMATION_FRAMES) {
		frame = 0;
	}
	if (pLazer.getFired()) {
		pLazer.render();
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
void Game::mainMenu() {
	bool enter = false;
	while(!enter) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		backgroundTexture.render(0, scrollingOffset);
		backgroundTexture.render(0, scrollingOffset + backgroundTexture.getHeight());
		++frame;
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
	SDL_RenderPresent(renderer);
	}
}
void Game::main() {
	mainMenu();
	while(!quit) {//MAIN GAME LOOP
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

	g.main();
	return 0;
}