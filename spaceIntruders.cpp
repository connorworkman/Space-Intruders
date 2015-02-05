/*	Space Intruders		*/
/*    Author: Connor Workman	*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include "imageTexture.hpp"
#include "player.hpp"
#include "alien.hpp"
#include "playerLazer.hpp"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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
	SDL_Rect renderRect = { x, y, sdlTextureWidth, sdlTextureHeight };
	SDL_RenderCopyEx(renderer, sdlTexture, clip, &renderRect, angle, center, flip);
}

int ImageTexture::getWidth() {return sdlTextureWidth;}
int ImageTexture::getHeight() {return sdlTextureHeight;}

Player::Player() {
    playerHorizontalPosit = SCREEN_WIDTH/2;
    playerVerticalPosit = SCREEN_HEIGHT-20;
    playerHorizontalVelocity = 0;
}

void Player::handleEvent( SDL_Event& e ) {
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {//keypress occurrs
        	switch( e.key.keysym.sym ) {
			case SDLK_LEFT:
				playerHorizontalVelocity -= PLAYER_VEL;
				break;
			case SDLK_RIGHT:
				playerHorizontalVelocity += PLAYER_VEL;
				break;
    		}
	}
	else if(e.type == SDL_KEYUP && e.key.repeat == 0) {//keypress is released
		switch(e.key.keysym.sym) {
			case SDLK_LEFT:
				playerHorizontalVelocity += PLAYER_VEL;
				break;
			case SDLK_RIGHT:
				playerHorizontalVelocity -= PLAYER_VEL;
				break;
        	}
    	}
}

void Player::move() {
    playerHorizontalPosit += playerHorizontalVelocity;
    //keep the player on the screen by correcting by playerHorizontalVelocity
    if((playerHorizontalPosit < 0) || (playerHorizontalPosit + PLAYER_WIDTH > SCREEN_WIDTH)) {
        playerHorizontalPosit -= playerHorizontalVelocity;
    }
}

int Player::getPlayerHorizontalPosit() {return playerHorizontalPosit;}

void Player::render() {playerTexture.render(playerHorizontalPosit, playerVerticalPosit);}

Alien::Alien() {
	alienHorizontalPosit = 0;
	alienVerticalPosit = 0;
}

void Alien::setAlienPosit(int alienNumber) {
	alienHorizontalPosit = (((alienNumber/5))*30) + 20;
	alienVerticalPosit = ((alienNumber%10)+1)*30;
}

void Alien::render() {alienTexture.render(alienHorizontalPosit, alienVerticalPosit);}

PlayerLazer::PlayerLazer() {
	playerLazerHorizontalPosit = 0;
	playerLazerVerticalPosit = 480;
	playerLazerVerticalVelocity = 0;
}

void PlayerLazer::setPlayerLazerPosit(int playerHorizontalPositWhenFiring) {playerLazerHorizontalPosit = playerHorizontalPositWhenFiring+10;}

void PlayerLazer::playerLazerMove() {playerLazerVerticalPosit -= playerLazerVerticalVelocity;}

void PlayerLazer::handleEvent(SDL_Event& e) {
	if(e.type = SDL_KEYDOWN && e.key.repeat == 0) {
		switch(e.key.keysym.sym) {
			case SDLK_UP:
				playerLazerVerticalVelocity += 2;	
				break;
		}
	}
}

void PlayerLazer::render() {playerLazerTexture.render(playerLazerHorizontalPosit, playerLazerVerticalPosit);}

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
}

void close() {
	playerTexture.free();
	backgroundTexture.free();
	alienTexture.free();
	playerLazerTexture.free();	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	IMG_Quit();//quit SDL_Image and SDL (subfunctions)
	SDL_Quit();
}

int main(int argc, char* args[]) {
	init();
	loadMedia();
	bool quit = false;
	SDL_Event event;
	Player playerInstance;
	PlayerLazer playerLazerInstance;
	int i = 0;
	int alienCount = 100;
	Alien alienArray[alienCount];
	for (i = 0; i < alienCount; i++) {
		alienArray[i].setAlienPosit(i);
	}
	int scrollingOffset = 0;
	while(!quit) {
		while(SDL_PollEvent(&event) != 0) {//remove an event from the event queue and process it
			if(event.type == SDL_QUIT) {//exit requested by user
				quit = true;
			}
			playerInstance.handleEvent(event);
			playerLazerInstance.handleEvent(event);
			playerLazerInstance.setPlayerLazerPosit(playerInstance.getPlayerHorizontalPosit());
		}
		playerInstance.move();
		playerLazerInstance.playerLazerMove();
		--scrollingOffset;
		if(scrollingOffset < -backgroundTexture.getWidth()) {
			scrollingOffset = 0;
		}
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		backgroundTexture.render(scrollingOffset, 0);
		backgroundTexture.render(scrollingOffset + backgroundTexture.getWidth(), 0);
		playerInstance.render();
		playerLazerInstance.render();
		int i = 0;
		for (i = 0; i < alienCount; i++) {
			alienArray[i].render();
		}
		SDL_RenderPresent(renderer);//refresh renderer
	}
	close();//free all textures and exit
	return 0;
}
