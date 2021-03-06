#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

int const WIDTH = 620;
int const HEIGHT = 720;
int const SPEED = 9;
int const FONT_SIZE = 32;
int const BALL_SPEED = 12;
int const SIZE = 16;
int const COL = 13;
int const ROW = 6;
float const PI = 3.1415926;
int const SPACING = 0;

SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;

bool running;

int frameCount, timerFPS, lastFrame, fps;   

SDL_Color color;

SDL_Rect paddle, ball, lives, brick;
float velY, velX;
int liveCount;
bool bricks[ROW * COL];

void resetBricks()
{
	for (int i = 0; i < COL * ROW; i++) {
		bricks[i] = 1;
	}

	liveCount = 3;
	paddle.x = (WIDTH / 2) - (paddle.w / 2);
	ball.y = paddle.y - (paddle.h * 4);
	velY = BALL_SPEED / 2;
	velX = 0;
	ball.x = WIDTH / 2 - (SIZE / 2);
}

void setBricks(int i)
{
	brick.x = (((i % COL) + 1) * SPACING) + ((i % COL) * brick.w) - (SPACING / 2);
	brick.y = brick.h * 3 + (((i % ROW) + 1) * SPACING) + ((i % ROW) * brick.w) - (SPACING / 2);
}

void write(std::string text, int x, int y)
{
	SDL_Surface* surface;
	SDL_Texture* texture;
	const char* t = text.c_str();
	surface = TTF_RenderText_Solid(font, t, color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	lives.w = surface->w;
	lives.h = surface->h;
	lives.x = x - lives.w;
	lives.y = y - lives.h;
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, NULL, &lives);
	SDL_DestroyTexture(texture);
}

void update()
{
	if (liveCount <= 0) {
		resetBricks();
	}

	if (SDL_HasIntersection(&ball, &paddle)) {
		double rel = (paddle.x + (paddle.w / 2)) - (ball.x + (SIZE / 2));
		double norm = rel / (paddle.w / 2);
		double bounce = norm * (5 * PI / 12);
		velY = -BALL_SPEED * cos(bounce);
		velX = BALL_SPEED * -sin(bounce);
	}
	if (ball.y <= 0) {
		velY = -velY;
	}
	if (ball.y + SIZE >= HEIGHT) {
		ball.x = paddle.x + (WIDTH / 8);
		ball.y = paddle.y - 50;
		velY = 1;
		velX = 0;
		--liveCount;
	}
	if (ball.x < -0 || ball.x + SIZE >= WIDTH) {
		velX = -velX;
	}
	ball.x += velX;
	ball.y += velY;
	if (paddle.x < 0) {
		paddle.x = 0;
	}
	if (paddle.x + paddle.w > WIDTH) {
		paddle.x = WIDTH - paddle.w;
	}

	bool reset = 1;
	for (size_t i = 0; i < COL * ROW; i++)
	{
		setBricks(i);
		if (SDL_HasIntersection(&ball, &brick) && bricks[i]) {
			bricks[i] = 0;
			if (ball.x >= brick.x) {
				velX = -velX;
				ball.x -= 20;
			}
			if (ball.x <= brick.x) {
				velX = -velX;
				ball.x += 20;
			}
			if (ball.y <= brick.y) {
				velY = -velY;
				ball.y -= 20;
			}
			if (ball.y >= brick.y) {
				velY = -velY;
				ball.y += 20;
			}
		}
		if (bricks[i]) {
			reset = 0;
		}
	}
	if (reset) {
		resetBricks();
	}
}

void input()
{
	SDL_Event e;
	const Uint8 *keystates = SDL_GetKeyboardState(NULL);
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			running = false;
		}
	}
	if (keystates[SDL_SCANCODE_ESCAPE]) {
		running = false;
	}
	if (keystates[SDL_SCANCODE_LEFT]) {
		paddle.x -= SPEED;
	}
	if (keystates[SDL_SCANCODE_RIGHT]) {
		paddle.x += SPEED;
	}
}
void render()
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
	SDL_RenderClear(renderer);

	++frameCount;
	timerFPS - SDL_GetTicks() - lastFrame;
	if (timerFPS < (1000 / 60)) {
		SDL_Delay((1000 / 60) - timerFPS);
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &paddle);
	SDL_RenderFillRect(renderer, &ball);
	write(std::to_string(liveCount), WIDTH / 2 + FONT_SIZE / 2, FONT_SIZE * 1.5);
	
	for (int i = 0; i < COL * ROW; i++)
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		if (i % 2 == 0) {
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		}
		if (bricks[i]) {
			setBricks(i);
			SDL_RenderFillRect(renderer, &brick);
		}
	}

	SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) 
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Failed at SDL_Init() \n Error: " << SDL_GetError << std::endl;
	}
	if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) {
		std::cout << "Failed at SDL_CreateWindowAndRenderer \n Error: " << SDL_GetError << std::endl;
	}
	SDL_SetWindowTitle(window, "Breakout");
	TTF_Init();
	font = TTF_OpenFont("pxlxxl.ttf", FONT_SIZE);
	running = 1;
	static int lastTime = 0;

	color.r = 255;
	color.g = 255;
	color.b = 255;

	paddle.h = 22;
	paddle.w = WIDTH / 4;
	paddle.y = HEIGHT - paddle.h - 32;

	ball.w = SIZE;
	ball.h = SIZE;

	brick.w = (WIDTH - (SPACING * COL)) / COL;
	brick.h = 22;

	resetBricks();

	while(running) 
	{
		lastFrame = SDL_GetTicks();
		if (lastFrame >= lastTime + 1000) {
			lastTime = lastFrame;
			fps = frameCount;
			frameCount - 0;
		}
		update();
		input();
		render();
	}

	return 0;
}
