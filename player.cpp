
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include "player.hpp"

Player::Player() {
    positX = (SCREEN_WIDTH/2)-this->PLAYER_WIDTH;
    positY = SCREEN_HEIGHT-this->PLAYER_HEIGHT;
    velocX = 0;

    playerClips[0].x = 0;                      playerClips[0].y = 0; playerClips[0].w = Player::PLAYER_WIDTH; playerClips[0].h = Player::PLAYER_HEIGHT;
	playerClips[1].x = Player::PLAYER_WIDTH;   playerClips[1].y = 0; playerClips[1].w = Player::PLAYER_WIDTH; playerClips[1].h = Player::PLAYER_HEIGHT;
	playerClips[2].x = Player::PLAYER_WIDTH*2; playerClips[2].y = 0; playerClips[2].w = Player::PLAYER_WIDTH; playerClips[2].h = Player::PLAYER_HEIGHT;
	playerClips[3].x = Player::PLAYER_WIDTH*3; playerClips[3].y = 0; playerClips[3].w = Player::PLAYER_WIDTH; playerClips[3].h = Player::PLAYER_HEIGHT;
	
}

Player::~Player() {
	playerAnimationTexture.free();
}

bool Player::loadPlayerTexture() {
	if(!playerAnimationTexture.loadFromFile("images/player.bmp")) {
		return false;
	} else {
		return true;
	}
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

SDL_Rect* Player::getPlayerClip(int frame, int FRAME_RATE) {
	return &playerClips[frame/FRAME_RATE];
}