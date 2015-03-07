#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "imageTexture.hpp"
#include "alien.hpp"

ImageTexture
::ImageTexture() {
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