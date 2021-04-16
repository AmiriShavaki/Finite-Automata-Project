#ifndef GRAPHICS_H //Just check if this class decleared before
#define GRAPHICS_H

//Using SDL and string
#include <SDL.h>
#include <string>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cstdio>
using namespace std;

SDL_Window* window; //The window we'll be rendering to
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

LTexture lTextureObj;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}

	//Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( renderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

class Graphics {
private:
    SDL_Surface* screenSurface; //The surface contained by the window
    SDL_Event event; //Our event reader
public:
    Graphics(){}
    Graphics(const int width, const int height, string title);
    void blit(SDL_Texture* image, const int x, const int y, const int width, const int height);
    void blitText(string text, const int x, const int y, const int width, const int height);
    void update();
    void delay(int amount);
    void endGame(); //Close
    const bool getEvents();
    const bool isClosed();
    SDL_Texture* loadImage(string fileName);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawArrow(int x1, int y1, int x2, int y2);
};

void Graphics::drawArrow(int x1, int y1, int x2, int y2) {
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x2 - 7, y2 + 10);
    drawLine(x2, y2, x2 + 7, y2 + 10);
}

void Graphics::drawLine(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

SDL_Texture* Graphics::loadImage(string fileName) {
    string address = "pictures//"; address += fileName; //Make address string in order to load the image

    //Load the image and make its texture
    SDL_Surface* image = IMG_Load(address.data());
    SDL_Texture* res = SDL_CreateTextureFromSurface(renderer, image);

    //Return pointer to loaded image
    return res;
}

const bool Graphics::isClosed() {
    return window == NULL;
}

void Graphics::endGame() {
    SDL_DestroyWindow(window);
    window = NULL;
}

const bool Graphics::getEvents() {
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                endGame();
                return false;
            break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_SPACE) {
                    return true;
                }
            break;
        }
    }
    return false;
}

void Graphics::delay(int amount) {
    SDL_Delay(amount); //Wait some seconds
}

void Graphics::update() {
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

void Graphics::blitText(string text, const int x, const int y, const int width, const int height) {
    SDL_Color BLACK = {0, 0, 0};
    lTextureObj.loadFromRenderedText(text.data(), BLACK);
    lTextureObj.render(x, y);
}

void Graphics::blit(SDL_Texture* image, const int x, const int y, const int width, const int height) {
    //Make position object
    SDL_Rect position;
    position.x = x;
    position.y = y;
    position.h = height;
    position.w = width;

    SDL_RenderCopy(renderer, image, NULL, &position); //Apply the image
}

Graphics::Graphics(const int width, const int height, string title) {
    //Create window
    window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

    //Get window surface
    screenSurface = SDL_GetWindowSurface(window);

    TTF_Init();
    font = TTF_OpenFont("times.ttf", 20);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
}

#endif //GRAPHICS_H
