/*	Space Intruders		*/
/*    Author: Connor Workman	*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "imageTexture.hpp"
#include "player.hpp"
#include "alien.hpp"
#include "playerLazer.hpp"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int ALIEN_ANIMATION_FRAMES = 4;
//TODO create additional alien class with ability to fire back?
//TODO create enemy lazer class

void init();
void loadMedia();
void close();

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

//instantiate all available textures
ImageTexture playerTexture;
ImageTexture backgroundTexture;
ImageTexture alienTexture;
ImageTexture playerLazerTexture;
////////////////////////////////////////////////////////
ImageTexture alienAnimationTexture;
SDL_Rect alienAnimationClips[ALIEN_ANIMATION_FRAMES];
///////////////////////////////////////////////////////
//TODO add explosion textures, enemy lazer textures, additional enemy textures
ImageTexture::ImageTexture() {
	sdlTexture = NULL;
	sdlTextureWidth = 0;
	sdlTextureHeight = 0;
}
ImageTexture::~ImageTexture() {free();}
bool ImageTexture::loadFromFile(std::string path) {
	free();//free previous load before attempting new load
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
       	newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	sdlTextureWidth = loadedSurface->w;
	sdlTextureHeight = loadedSurface->h;
	SDL_FreeSurface(loadedSurface);//free old surface
	sdlTexture = newTexture;
	alienAnimationClips[0].x = 0;
	alienAnimationClips[0].y = 0;
	alienAnimationClips[0].w = 20;
	alienAnimationClips[0].h = 14;
	alienAnimationClips[1].x = 20;
	alienAnimationClips[1].y = 0;
	alienAnimationClips[1].w = 20;
	alienAnimationClips[1].h = 14;
	alienAnimationClips[2].x = 40;
	alienAnimationClips[2].y = 0;
	alienAnimationClips[2].w = 20;
	alienAnimationClips[2].h = 14;
	alienAnimationClips[3].x = 60;
	alienAnimationClips[3].y = 0;
	alienAnimationClips[3].w = 20;
	alienAnimationClips[3].h = 14;
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
void ImageTexture::setColorMode(Uint8 red, Uint8 green, Uint8 blue) {SDL_SetTextureColorMod(sdlTexture, red, green, blue);}
void ImageTexture::setBlendMode(SDL_BlendMode blending) {SDL_SetTextureBlendMode(sdlTexture, blending);}
void ImageTexture::setAlphaMode(Uint8 alpha){SDL_SetTextureAlphaMod(sdlTexture, alpha);}
void ImageTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderRect = {x, y, sdlTextureWidth, sdlTextureHeight};
	if (clip!= NULL) {
		renderRect.w = clip->w;
		renderRect.h = clip->h;
	}
	SDL_RenderCopyEx(renderer, sdlTexture, clip, &renderRect, angle, center, flip);
}
int ImageTexture::getWidth() {return sdlTextureWidth;}
int ImageTexture::getHeight() {return sdlTextureHeight;}

Player::Player() {
    playerPositX = (SCREEN_WIDTH/2)-20;
    playerPositY = SCREEN_HEIGHT-20;
    playerVelocX = 0;
}
void Player::handleEvent( SDL_Event& event ) {
	if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {//keypress occurrs
        	switch( event.key.keysym.sym ) {
			case SDLK_LEFT:
				playerVelocX -= PLAYER_VEL;
				break;
			case SDLK_RIGHT:
				playerVelocX += PLAYER_VEL;
				break;
    		}
	}
	else if(event.type == SDL_KEYUP && event.key.repeat == 0) {//keypress is released
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				playerVelocX += PLAYER_VEL;
				break;
			case SDLK_RIGHT:
				playerVelocX -= PLAYER_VEL;
				break;
        	}
    	}
}
void Player::move() {
    playerPositX += playerVelocX;
    //keep the player on the screen by correcting by playerVelocX
    if((playerPositX < 0) || (playerPositX + PLAYER_WIDTH > SCREEN_WIDTH)) {
        playerPositX -= playerVelocX;
    }
}

int Player::getPlayerPositX() {
	return playerPositX;
}

void Player::render() {
	playerTexture.render(playerPositX, playerPositY);
}

Alien::Alien() {
	alienHorizontalPosit = 0;
	alienVerticalPosit = 0;
}

void Alien::setAlienPosit(int alienNumber) {
	alienHorizontalPosit = (((alienNumber/10))*30) + (SCREEN_WIDTH)/4;
	alienVerticalPosit = ((alienNumber%10)+1)*30;
}
int Alien::getAlienHorizontalPosit() {return this->alienHorizontalPosit;}
int Alien::getAlienVerticalPosit() {return this->alienVerticalPosit;}
void Alien::render(int w, int h, SDL_Rect* clip) {
	alienAnimationTexture.render(w, h, clip);
}

PlayerLazer::PlayerLazer() {
	this->fired = false;
	this->playerLazerPositX = 0;
	this->playerLazerPositY = 480;
	this->playerLazerVelocY = 0;
}

PlayerLazer::~PlayerLazer() {
	//empty destructor for PlayerLazer
}

void PlayerLazer::setPlayerLazerPositX(int playerPositXWhenFiring) {
	this->playerLazerPositX = playerPositXWhenFiring+5;
}

bool PlayerLazer::move() {
	playerLazerPositY -= playerLazerVelocY;
	if (playerLazerPositY < 0) {//if lazer goes off screen
		return false;//return false so we can reset it
	}
	return true;
}

bool PlayerLazer::getFired() {
	return this->fired;
}

void PlayerLazer::setFired(bool x) {
	this->fired = x;
}

void PlayerLazer::setVelocY(int newVelocity) {
	this->playerLazerVelocY = newVelocity;
}

void PlayerLazer::reset() {
	this->fired = false;
	this->playerLazerPositY = 480;
	this->playerLazerPositX = 0;
	this->playerLazerVelocY = 0;
}

void PlayerLazer::render() {
	playerLazerTexture.render(playerLazerPositX, playerLazerPositY);
}

void init() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");//rendering quality = 1
	window = SDL_CreateWindow("Space Intruders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	//create vsynced renderer
	//the renderer uses hardware acceleration
	//present is synchronized with the refresh rate
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	int imgFlags = IMG_INIT_PNG;
	IMG_Init(imgFlags);
}

void loadMedia() {
	playerTexture.loadFromFile("images/player.bmp");
	backgroundTexture.loadFromFile("images/space.png");
	alienTexture.loadFromFile("images/alien.bmp");
	playerLazerTexture.loadFromFile("images/playerLazer.bmp");
	alienAnimationTexture.loadFromFile("images/animatedalien.bmp");
}

void close() {
	playerTexture.free();
	backgroundTexture.free();
	alienTexture.free();
	alienAnimationTexture.free();
	playerLazerTexture.free();	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	IMG_Quit();//quit SDL_Image
	SDL_Quit();//quit SDL
}

int main(int argc, char* args[]) {
	init();
	loadMedia();
	bool quit = false;
	SDL_Event event;
	Player playerInstance;
	PlayerLazer playerLazerInstance;
	PlayerLazer playerLazerInstance2;
	PlayerLazer playerLazerInstance3;
	PlayerLazer playerLazerInstance4;
	PlayerLazer playerLazerInstance5;
	PlayerLazer playerLazerInstance6;
	PlayerLazer playerLazerInstance7;
	PlayerLazer playerLazerInstance8;
	PlayerLazer playerLazerInstance9;
	PlayerLazer playerLazerInstance10;
	int i = 0;
	int alienCount = 100;
	Alien alienArray[alienCount];
	for (i = 0; i < alienCount; i++) {
		alienArray[i].setAlienPosit(i);
	}
	int scrollingOffset = 0;
	int frame = 0;
	while(!quit) {//MAIN GAME LOOP
		while(SDL_PollEvent(&event) != 0) {//remove an event from the event queue and process it
			if(event.type == SDL_QUIT) {//exit requested by user
				quit = true;
			}
			playerInstance.handleEvent(event);
			if (event.type == SDL_KEYUP &&
			event.key.repeat == 0 && 
			playerLazerInstance.getFired() == false && 
			event.key.keysym.sym == SDLK_UP) {//&& SDL_KEY_UP_PRESSED
				playerLazerInstance.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance.setFired(true);
				playerLazerInstance.setVelocY(7);
			}
			else if (event.type == SDL_KEYUP && 
			event.key.repeat == 0 && 
			playerLazerInstance2.getFired() == false && 
			event.key.keysym.sym == SDLK_UP) {
				playerLazerInstance2.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance2.setFired(true);
				playerLazerInstance2.setVelocY(7);
			}
			else if (event.type == SDL_KEYUP &&
			event.key.repeat == 0 &&
			playerLazerInstance3.getFired() == false &&
			event.key.keysym.sym == SDLK_UP) {
				playerLazerInstance3.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance3.setFired(true);
				playerLazerInstance3.setVelocY(7);
			}
			else if (event.type == SDL_KEYUP &&
			event.key.repeat == 0 &&
			playerLazerInstance4.getFired() == false &&
			event.key.keysym.sym == SDLK_UP) {
				playerLazerInstance4.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance4.setFired(true);
				playerLazerInstance4.setVelocY(7);
			}
			else if (event.type == SDL_KEYUP &&
			event.key.repeat == 0 &&
			playerLazerInstance5.getFired() == false &&
			event.key.keysym.sym == SDLK_UP) {
				playerLazerInstance5.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance5.setFired(true);
				playerLazerInstance5.setVelocY(7);
			}
			else if (event.type == SDL_KEYUP &&
			event.key.repeat == 0 &&
			playerLazerInstance6.getFired() == false &&
			event.key.keysym.sym == SDLK_UP) {
				playerLazerInstance6.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance6.setFired(true);
				playerLazerInstance6.setVelocY(7);
			}
			else if (event.type == SDL_KEYUP &&
			event.key.repeat == 0 &&
			playerLazerInstance7.getFired() == false &&
			event.key.keysym.sym == SDLK_UP) {
				playerLazerInstance7.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance7.setFired(true);
				playerLazerInstance7.setVelocY(7);
			}
			else if (event.type == SDL_KEYUP &&
			event.key.repeat == 0 &&
			playerLazerInstance8.getFired() == false &&
			event.key.keysym.sym == SDLK_UP) {
				playerLazerInstance8.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance8.setFired(true);
				playerLazerInstance8.setVelocY(7);
			}
			else if (event.type == SDL_KEYUP &&
			event.key.repeat == 0 &&
			playerLazerInstance9.getFired() == false &&
			event.key.keysym.sym == SDLK_UP) {
				playerLazerInstance9.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance9.setFired(true);
				playerLazerInstance9.setVelocY(7);
			}
			else if (event.type == SDL_KEYUP &&
			event.key.repeat == 0 &&
			playerLazerInstance10.getFired() == false &&
			event.key.keysym.sym == SDLK_UP) {
				playerLazerInstance10.setPlayerLazerPositX(playerInstance.getPlayerPositX());
				playerLazerInstance10.setFired(true);
				playerLazerInstance10.setVelocY(7);
			}
		}
		playerInstance.move();
		if (!playerLazerInstance.move()) {
			playerLazerInstance.reset();
		}
		if (!playerLazerInstance2.move()) {
			playerLazerInstance2.reset();
		}
		if (!playerLazerInstance3.move()) {
			playerLazerInstance3.reset();
		}
		if (!playerLazerInstance4.move()) {
			playerLazerInstance4.reset();
		}
		if (!playerLazerInstance5.move()) {
			playerLazerInstance5.reset();
		}
		if (!playerLazerInstance6.move()) {
			playerLazerInstance6.reset();
		}
		if (!playerLazerInstance7.move()) {
			playerLazerInstance7.reset();
		}
		if (!playerLazerInstance8.move()) {
			playerLazerInstance8.reset();
		}
		if (!playerLazerInstance9.move()) {
			playerLazerInstance9.reset();
		}
		if (!playerLazerInstance10.move()) {
			playerLazerInstance10.reset();
		}
		--scrollingOffset;
		if(scrollingOffset < -backgroundTexture.getWidth()) {
			scrollingOffset = 0;
		}
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		backgroundTexture.render(scrollingOffset, 0);
		backgroundTexture.render(scrollingOffset + backgroundTexture.getWidth(), 0);
		playerInstance.render();
///////////////////////////////////////////////////
		SDL_Rect* currentClip = &alienAnimationClips[frame/10];
		int i = 0;
		for (i = 0; i < alienCount; i++) {
			alienArray[i].render((alienArray[i].getAlienHorizontalPosit()), (alienArray[i].getAlienVerticalPosit()), currentClip);
		}
		++frame;
		if (frame/10 >= ALIEN_ANIMATION_FRAMES) {
			frame = 0;
		}
///////////////////////////////////////////////////
		playerLazerInstance.render();
		playerLazerInstance2.render();
		playerLazerInstance3.render();
		playerLazerInstance4.render();
		playerLazerInstance5.render();
		playerLazerInstance6.render();
		playerLazerInstance7.render();
		playerLazerInstance8.render();
		playerLazerInstance9.render();
		playerLazerInstance10.render();
		//int i = 0;
		//for (i = 0; i < alienCount; i++) {
		//	alienArray[i].render();
		//}
		SDL_RenderPresent(renderer);//refresh renderer
	}
	close();//free all textures and exit
	return 0;
}
