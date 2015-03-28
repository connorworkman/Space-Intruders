#include "playerLazer.hpp"


PlayerLazer::PlayerLazer() {
	this->fired = false;
	this->positX = 0;
	this->positY = SCREEN_HEIGHT - PLAYER_HEIGHT;
	this->velocY = 0;
	
}

PlayerLazer::~PlayerLazer() {
	playerLazerTexture.free();
}
bool PlayerLazer::loadPlayerLazerTexture() {
	if (playerLazerTexture.loadFromFile("images/playerLazer.bmp")) {
		return true;
	} else {
		return false;
	}
}
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
	this->positY = SCREEN_HEIGHT - PLAYER_HEIGHT;
	this->positX = 0;
	this->velocY = 0;
}

void PlayerLazer::render() {
	playerLazerTexture.render(positX, positY);
}
