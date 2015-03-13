#ifndef IMAGETEXTURE_HPP
#define IMAGETEXTURE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>

class ImageTexture {//SDL_Texture wrapper class
	public:
		ImageTexture();
		~ImageTexture();
		bool loadFromFile(std::string path);
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
		void free();//deallocate textures
		void setColorMode(Uint8 red, Uint8 green, Uint8 blue);//set red green and blue color components
		void setBlendMode(SDL_BlendMode blending);//set blend mode
		void setAlphaMode(Uint8 alpha);//set alpha modulation
		//rendering an image requires x and y arguments
		void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
		int getWidth();
		int getHeight();
		
	private:
		SDL_Texture* sdlTexture;
		int sdlTextureWidth;
		int sdlTextureHeight;
};
#endif