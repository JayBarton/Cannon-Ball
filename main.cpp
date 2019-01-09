#include <string>
#include <cstdio>
#include <iostream>

#include <GL/glew.h>

#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "Timing.h"
#include "TextRenderer.h"

#include <vector>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_mixer.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

#include <sstream>
#include <fstream>

#include <algorithm>
#include <random>
#include <ctime>

#include "Cannon.h"
#include "Move.h"
#include "Rotate.h"
#include "Player.h"
#include "Spike.h"
#include "Key.h"
#include "Exit.h"
#include "Life.h"


// The Width of the screen
const GLuint SCREEN_WIDTH = 800;
// The height of the screen
const GLuint SCREEN_HEIGHT = 600;

const int LEVEL_WIDTH = 3200;
const int LEVEL_HEIGHT = 2560;

const int NUMBER_OF_LEVELS = 5;

const int NEW_LEVEL_DELAY = 1;

const int AUTO_UV = 0;
const int MOVE_UV = 1;
const int ROTATE_UV = 2;
const int PLAYER_UV = 3;
const int SPIKE_UV = 4;
const int LOCK_UV = 5;
const int EXIT_UV = 6;
const int KEY_UV = 7;

struct Particle
{
	glm::vec2 position;
	glm::vec2 direction;
	float speed;
	float timeToLive;

	bool alive()
	{
		return timeToLive > 0;
	}
};

struct TransitionQuad
{
	glm::vec2 position;
	float speed;
	float alpha;
};

TransitionQuad transition;

std::mt19937 randomEngine(time(nullptr));

float deathTime = 1.5f;
float resetTime = 1.5f;

SDL_Window *window;
Mix_Chunk *fireSoundEffect = nullptr;
Mix_Chunk *keySoundEffect = nullptr;
Mix_Chunk *deathSoundEffect = nullptr;
Mix_Chunk *lifeSoundEffect = nullptr;

Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT, LEVEL_WIDTH, LEVEL_HEIGHT);

SpriteRenderer *Renderer;
TextRenderer *Text;

void init();
void quit();
void Draw();

GLuint particleVAO;
GLuint transitionVAO;

std::vector<Cannon> cannons;
std::vector<Spike> spikes;

std::vector<Particle> cannonParticles;
std::vector<Particle> deathParticles;

Player player;
Key key;
Life life;

int startCannon = 0;
int exitCannon = 0;
int currentLevel = 1;

float resetTimer = 0.0f;

bool screenTransition = false;
//Used to tell what should be drawn
bool activeLevel;
bool buttonDown;

std::vector<glm::vec4> uvs;

enum GameState
{
	TITLESCREEN, LEVELSTART, PLAYING, DEAD, RESETLEVEL, GAMEOVER, WIN, ENDGAME, ERRORSTATE
};


void ResetLevel();

void PlayerDeath();

void CheckInput();

void FireCannonParticles();
void FireDeathParticles();

bool LoadLevel(int level);
void UnloadLevel();

GameState state = TITLESCREEN;

bool cameraLock;

int main(int argc, char** argv)
{
	
	init();

	float deathTimer = 0.0f;
	float newLevelTimer = 0.0f;
	float transitionTimer = 0.0f;
	float gameOverTimer = 1.0f;
	
	transition.speed = 700.0f;

	camera.setPosition(glm::vec2(0.0f, 0.0f));
	camera.update();

	ResourceManager::LoadShader("Shaders/spriteVertexShader.txt", "Shaders/spriteFragmentShader.txt", nullptr, "sprite");
	ResourceManager::LoadShader("Shaders/shapeVertexShader.txt", "Shaders/shapeFragmentShader.txt", nullptr, "shape");
	ResourceManager::LoadShader("Shaders/shapeVertexShader.txt", "Shaders/shapeFragmentShader.txt", nullptr, "transition");

	ResourceManager::LoadTexture("Textures/spriteSheet.png", "sprites");
	ResourceManager::LoadTexture("Textures/background.png", "background"); 

	ResourceManager::GetShader("shape").Use().SetMatrix4("projection", camera.getCameraMatrix());
	ResourceManager::GetShader("transition").Use().SetMatrix4("projection", camera.getCameraMatrix());

	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", camera.getCameraMatrix());

	fireSoundEffect = Mix_LoadWAV("Sounds/Fire.wav");
	keySoundEffect = Mix_LoadWAV("Sounds/KeySound.wav");
	lifeSoundEffect = Mix_LoadWAV("Sounds/ExtraLife.wav");
	deathSoundEffect = Mix_LoadWAV("Sounds/DeathSound.wav");

	Shader myShader;
	myShader = ResourceManager::GetShader("sprite");
	Renderer = new SpriteRenderer(myShader);

	uvs = ResourceManager::GetTexture("sprites").GetUVs(0, 0, 64, 64, 4, 2);

	player.init(glm::vec2(0.0f, 0.0f), uvs[PLAYER_UV]);

	Text = new TextRenderer(800, 600);
	Text->Load("fonts/Teko-Light.TTF", 30);

	// DeltaTime variables
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	//Main loop flag
	bool isRunning = true;

	//Event handler
	SDL_Event event;

	int fps = 0;
	FPSLimiter fpsLimiter;
	fpsLimiter.setMaxFPS(60.0f);

	const float MS_PER_SECOND = 1000;
	const float DESIRED_FPS = 60;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAXIMUM_DELTA_TIME = 1.0f;

	const int MAXIMUM_STEPS = 6;

	float previousTicks = SDL_GetTicks();

	GLfloat vertices[] = {
		0.0f, 1.0f, // Left  
		1.0f, 0.0f, // Right 
		0.0f, 0.0f,  // Top   
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	GLuint tVBO;
	glGenVertexArrays(1, &transitionVAO);
	glGenBuffers(1, &tVBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(transitionVAO);

	glBindBuffer(GL_ARRAY_BUFFER, tVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	GLuint pVBO;
	glGenVertexArrays(1, &particleVAO);
	glGenBuffers(1, &pVBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(particleVAO);

	glBindBuffer(GL_ARRAY_BUFFER, pVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//While application is running
	while (isRunning)
	{
		GLfloat timeValue = SDL_GetTicks() / 1000.0f;
		// Calculate deltatime of current frame
		GLfloat currentFrame = timeValue;
		deltaTime = currentFrame - lastFrame;
		deltaTime = glm::clamp(deltaTime, 0.0f, 0.02f);
		lastFrame = currentFrame;

		fpsLimiter.beginFrame();

		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - previousTicks;
		previousTicks = newTicks;

		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		//Handle events on queue
		while (SDL_PollEvent(&event) != 0)
		{
			//User requests quit
			if (event.type == SDL_QUIT)
			{
				isRunning = false;
			}

			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_RETURN)
				{
					if (state == ERRORSTATE)
					{
						isRunning = false;
					}
				}
			}
		}
		CheckInput();

		camera.setPosition(player.sprite.getPosition());

		if (state == PLAYING)
		{
			camera.update();
			player.Update(deltaTime);
			if (player.sprite.getPosition().x <= 0 || player.sprite.getPosition().x >= LEVEL_WIDTH
				|| player.sprite.getPosition().y <= 0 || player.sprite.getPosition().y >= LEVEL_HEIGHT)
			{
				PlayerDeath();
			}
		}

		if (state == LEVELSTART)
		{
			player.SetCannon(cannons[startCannon]);
			camera.setPosition(player.sprite.getPosition());
			camera.update();
			activeLevel = true;
			if (newLevelTimer < NEW_LEVEL_DELAY)
			{
				newLevelTimer += deltaTime;
				cannons[startCannon].sprite.alpha += 1.0f * deltaTime;
			}
			else
			{
				cannons[startCannon].sprite.alpha = 1.0f;
				newLevelTimer = 0.0f;
				state = PLAYING;
			}
		}

		if (activeLevel)
		{
			for (size_t i = 0; i < cannons.size(); i++)
			{
				if (cannons[i].behaviour != nullptr && cannons[i].active)
				{
					cannons[i].Update(deltaTime);
				}
			}

			for (size_t i = 0; i < spikes.size(); i++)
			{
				if (spikes[i].behaviour != nullptr)
				{
					spikes[i].Update(deltaTime);
				}
			}

			for (size_t i = 0; i < cannonParticles.size(); i++)
			{
				if (cannonParticles[i].alive())
				{
					cannonParticles[i].position += cannonParticles[i].direction * cannonParticles[i].speed * deltaTime;
					if (cannonParticles[i].speed > 0)
					{
						cannonParticles[i].speed -= 1600 * deltaTime;
					}
					cannonParticles[i].timeToLive -= deltaTime;
				}
			}


			for (size_t i = 0; i < deathParticles.size(); i++)
			{
				if (deathParticles[i].alive())
				{
					deathParticles[i].position += deathParticles[i].direction * deathParticles[i].speed * deltaTime;
					deathParticles[i].timeToLive -= deltaTime;
				}
			}
		}

		if (state == PLAYING)
		{
			//No colision testing if the player is inside of a cannon
			if (player.cannon == nullptr)
			{
				//Check colisions with cannons
				for (size_t i = 0; i < cannons.size(); i++)
				{
					if (cannons[i].collideable && player.CheckColision(cannons[i].sprite.getHitBox()))
					{
						player.SetCannon(cannons[i]);
						if (player.win)
						{
							currentLevel++;
							state = WIN;
						}
					}
				}

				if (life.sprite.active && player.CheckColision(life.sprite.getHitBox()))
				{
					player.lives++;
					life.sprite.active = false;
					Mix_PlayChannel(-1, lifeSoundEffect, 0);
				}

				//Check colisions with spikes
				for (size_t i = 0; i < spikes.size(); i++)
				{
					if (player.CheckColision(spikes[i].sprite.getHitBox()))
					{
						PlayerDeath();
					}
				}

				if (key.sprite.active && player.CheckColision(key.sprite.getHitBox()))
				{
					player.hasKey = true;
					key.sprite.active = false;
					Mix_PlayChannel(-1, keySoundEffect, 0);
				}
			}
		}

		if (state == WIN)
		{
			//Fade effect
			camera.setPosition(player.sprite.getPosition());
			camera.update();
			if (!cannons[exitCannon].active)
			{
				cannons[exitCannon].sprite.uv = uvs[EXIT_UV];
			}
			if (newLevelTimer < NEW_LEVEL_DELAY)
			{
				newLevelTimer += deltaTime;
				cannons[exitCannon].sprite.alpha -= 1.0f * deltaTime;
			}
			else
			{
				cannons[exitCannon].sprite.alpha = 0.0f;
				newLevelTimer = 0.0f;
				if (currentLevel <= NUMBER_OF_LEVELS)
				{
					activeLevel = false;
					UnloadLevel();
					if (LoadLevel(currentLevel))
					{
						state = LEVELSTART;
					}
					else
					{
						state = ERRORSTATE;
					}
				}
				else
				{
					state = ENDGAME;
					UnloadLevel();
				}
				player.win = false;
				player.hasKey = false;
			}
		}
		else if (state == DEAD)
		{
			deathTimer += deltaTime;
			if (deathTimer >= deathTime)
			{
				glm::vec2 start = glm::vec2(-832.0f, -32.0f);
				glm::vec2 end = glm::vec2(0.0f, 0.0f);
				transition.position = glm::mix(start, end, transitionTimer);

				screenTransition = true;
				transitionTimer += deltaTime;
				if (transitionTimer >= 1.0f)
				{
					//screenTransition = false;
					transitionTimer = 0.0f;
					deathTimer = 0.0f;
					player.lives--;
					if (player.lives < 0)
					{
						state = GAMEOVER;
					}
					else
					{
						state = RESETLEVEL;
					}
					activeLevel = false;
				}
			}
		}
		else if (state == GAMEOVER)
		{
			resetTimer += deltaTime;
			if (resetTimer >= resetTime)
			{
				resetTimer = 0.0f;
				UnloadLevel();
				state = TITLESCREEN;
				screenTransition = false;
			}
		}

		else if (state == RESETLEVEL)
		{
			resetTimer += deltaTime;
			if (resetTimer >= resetTime)
			{

				if (!activeLevel)
				{
					ResetLevel();
					activeLevel = true;
				}

				glm::vec2 start = glm::vec2(0.0f, 0.0f);
				glm::vec2 end = glm::vec2(start.x + 800, start.y);

				transition.position = glm::mix(start, end, transitionTimer);
				transitionTimer += deltaTime;
				if (transitionTimer >= 1.0f)
				{
					transitionTimer = 0.0f;
					resetTimer = 0.0f;
					state = PLAYING;
					screenTransition = false;
				}
			}
		}
		Draw();
		fps = fpsLimiter.end();
	}

	quit();
	return 0;
}


void init()
{
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    SDL_Init(SDL_INIT_EVERYTHING);

    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        printf( "SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError() );
    }


	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}

    SDL_GLContext context; //check if succesfully created later

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,8 );
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32 );
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //For multisampling
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow("Cannon Ball", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, flags);

    context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return;
    }

    SDL_GL_SetSwapInterval(1);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void quit()
{
	UnloadLevel();
	delete Renderer;

	Mix_FreeChunk(fireSoundEffect);
	fireSoundEffect = nullptr;

	Mix_FreeChunk(keySoundEffect);
	keySoundEffect = nullptr;

	Mix_FreeChunk(lifeSoundEffect);
	lifeSoundEffect = nullptr;

	Mix_FreeChunk(deathSoundEffect);
	deathSoundEffect = nullptr;


	SDL_DestroyWindow(window);
	window = nullptr;

	SDL_Quit();
	IMG_Quit();
	Mix_Quit();
}


void Draw()
{
	glClearColor(0.17f, 0.22f, 0.27f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	if (state == ERRORSTATE)
	{
		Text->RenderText("Unable to load next level", 330, 300, 1.0f);
		Text->RenderText("Press the enter key to exit the game", 330, 330, 1.0f);
	}
	else
	{
		ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", camera.getCameraMatrix());

		//Draw background
		Renderer->setUVs();
		for (size_t i = 0; i < 5; i++)
		{
			for (size_t c = 0; c < 7; c++)
			{
				Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(c * 512, i * 512), 0.0f, 512, glm::vec4(0.17f, 0.22f, 0.27f, 1.0f));
			}
		}

		if (state == TITLESCREEN)
		{
			Text->RenderText("PRESS THE ENTER KEY", 330, 300, 1.0f);
		}
		else if (state == ENDGAME)
		{
			Text->RenderText("You have completed every level!", 330, 300, 1.0f);
			Text->RenderText("Congratulations!", 370, 330, 1.0f);
			Text->RenderText("Thanks for playing!", 370, 360, 1.0f);
			Text->RenderText("Press the enter key to return to the title", 300, 390, 1.0f);
		}

		else if (state != GAMEOVER)
		{

			//	Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), 0.0f, 512);
			ResourceManager::GetShader("shape").Use().SetMatrix4("projection", camera.getCameraMatrix());

			ResourceManager::GetShader("shape").SetVector3f("shapeColor", glm::vec3(1.0f, 1.0f, 1.0f));
			for (size_t i = 0; i < cannonParticles.size(); i++)
			{
				if (cannonParticles[i].alive())
				{
					glm::mat4 model;

					model = glm::translate(model, glm::vec3(cannonParticles[i].position, 0.0f));
					model = glm::scale(model, glm::vec3(8.0f, 8.0f, 0.0f));

					ResourceManager::GetShader("shape").Use().SetMatrix4("model", model);
					glBindVertexArray(particleVAO);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glBindVertexArray(0);
				}
			}
			ResourceManager::GetShader("shape").SetVector3f("shapeColor", glm::vec3(0.0f, 0.074, 0.49f));
			for (size_t i = 0; i < deathParticles.size(); i++)
			{
				if (deathParticles[i].alive())
				{
					glm::mat4 model;

					model = glm::translate(model, glm::vec3(deathParticles[i].position, 0.0f));
					model = glm::scale(model, glm::vec3(16.0f, 16.0f, 0.0f));

					ResourceManager::GetShader("shape").Use().SetMatrix4("model", model);
					glBindVertexArray(particleVAO);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glBindVertexArray(0);
				}
			}

			if (player.sprite.active)
			{
				Renderer->setUVs(player.sprite.getUV());
				Renderer->DrawSprite(ResourceManager::GetTexture("sprites"), player.sprite.getPosition(), 0.0f, player.SIZE);
			}

			if (activeLevel)
			{
				if (life.sprite.active)
				{
					Renderer->setUVs(life.sprite.getUV());
					Renderer->DrawSprite(ResourceManager::GetTexture("sprites"), life.sprite.getPosition(), 0.0f, life.SIZE);
				}
				if (key.sprite.active)
				{
					Renderer->setUVs(key.sprite.getUV());
					Renderer->DrawSprite(ResourceManager::GetTexture("sprites"), key.sprite.getPosition(), 0.0f, key.SIZE);
				}
				for (size_t i = 0; i < cannons.size(); i++)
				{
					Renderer->setUVs(cannons[i].getUVs());
					Renderer->DrawSprite(ResourceManager::GetTexture("sprites"),
						cannons[i].sprite.getPosition(), cannons[i].sprite.rotate,
						cannons[i].CANNON_SIZE, glm::vec4(glm::vec3(1.0f), cannons[i].sprite.alpha));
				}

				for (size_t i = 0; i < spikes.size(); i++)
				{
					Renderer->setUVs(spikes[i].getUVs());
					Renderer->DrawSprite(ResourceManager::GetTexture("sprites"), spikes[i].sprite.getPosition(), 0.0f, spikes[i].SIZE);
				}

				std::stringstream lives;
				lives << "Cannon Balls:  " << player.lives;
				Text->RenderText(lives.str(), 650, 30, 1);
			}

			if (state == LEVELSTART)
			{
				//timer to get rid of text
				std::stringstream level;
				level << "Level : " << currentLevel;
				Text->RenderText(level.str(), 400, 300, 1);
			}

		}

		ResourceManager::GetShader("transition").Use();
		if (screenTransition)
		{
			ResourceManager::GetShader("transition").SetVector3f("shapeColor", glm::vec3(0.0f, 0.0f, 0.0f));
			glm::mat4 model;

			model = glm::translate(model, glm::vec3(transition.position, 0.0f));
			model = glm::scale(model, glm::vec3(SCREEN_WIDTH + 64, SCREEN_HEIGHT + 64, 0.0f));

			ResourceManager::GetShader("transition").Use().SetMatrix4("model", model);
			glBindVertexArray(transitionVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}

		if (state == RESETLEVEL)
		{
			if (resetTimer < resetTime)
			{
				std::stringstream remainingLives;
				remainingLives << "Remaining Cannon Balls: " << player.lives;
				Text->RenderText(remainingLives.str(), 310, 300, 1);
			}
		}
		else if (state == GAMEOVER)
		{
			Text->RenderText("GAME OVER", 380, 300, 1);
		}
	}
	SDL_GL_SwapWindow(window); 
}

void ResetLevel()
{
	for (size_t i = 0; i < cannons.size(); i++)
	{
		if (cannons[i].getType() == 1)
		{
			cannons[i].sprite.SetPosition(static_cast<Move*>(cannons[i].behaviour)->start);
			static_cast<Move*>(cannons[i].behaviour)->t = 0.0f;
		}
		else if (cannons[i].getType() == 2)
		{
			cannons[i].sprite.rotate = static_cast<Rotate*>(cannons[i].behaviour)->start;
			cannons[i].sprite.rotate = glm::radians(float(cannons[i].sprite.rotate));
		}
	}

	for (size_t i = 0; i < spikes.size(); i++)
	{
		if (spikes[i].behaviour != nullptr)
		{
			spikes[i].sprite.SetPosition(static_cast<Move*>(spikes[i].behaviour)->start);
			static_cast<Move*>(spikes[i].behaviour)->t = 0.0f;
		}
	}
	player.sprite.active = true;
	if (player.previousCannon != nullptr)
	{
		player.previousCannon->collideable = true;
	}
	player.cannon = nullptr;
	player.previousCannon = nullptr;
	player.SetCannon(cannons[startCannon]);
	camera.setPosition(player.sprite.getPosition());
	camera.update();
	player.hasKey = false;
	key.sprite.active = true;
}

void PlayerDeath()
{

	player.sprite.active = false;
	FireDeathParticles();
	Mix_PlayChannel(-1, deathSoundEffect, 0);
	state = DEAD;
}

void CheckInput()
{
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

	if (state == PLAYING && player.cannon != nullptr)
	{
		if (currentKeyStates[SDL_SCANCODE_RETURN])
		{
			if (!buttonDown)
			{
				player.Fire();
				buttonDown = true;
				cameraLock = false;
				Mix_PlayChannel(-1, fireSoundEffect, 0);
				FireCannonParticles();
			}
		}
	}
	else if (state == TITLESCREEN)
	{
		if (currentKeyStates[SDL_SCANCODE_RETURN])
		{
			if (!buttonDown)
			{
				currentLevel = 1;
				if (LoadLevel(currentLevel))
				{
					cannonParticles.resize(60);
					deathParticles.resize(16);
					player.SetCannon(cannons[startCannon]);
					player.lives = player.STARTING_LIVES;
					player.hasKey = false;
					camera.setPosition(player.sprite.getPosition());
					state = LEVELSTART;
					buttonDown = true;
				}
				else
				{
					state = ERRORSTATE;
				}
			}
		}
	}
	else if (state == ENDGAME)
	{
		if (!buttonDown)
		{
			if (currentKeyStates[SDL_SCANCODE_RETURN])
			{
				buttonDown = true;
				state = TITLESCREEN;
			}
		}
	}


	if (!currentKeyStates[SDL_SCANCODE_RETURN])
	{
		buttonDown = false;
	}
}

void FireCannonParticles()
{
	int particle = 0;
	int liveParticle = 0;
	glm::vec2 cannonPosition = player.previousCannon->sprite.getPosition();
	float cannonSize = player.previousCannon->sprite.getSize();
	std::uniform_int_distribution<int> xPosition(cannonPosition.x, cannonPosition.x + cannonSize);
	std::uniform_int_distribution<int> yPosition(cannonPosition.y, cannonPosition.y + cannonSize);
	while (liveParticle < 20 && particle < cannonParticles.size())
	{
		if (!cannonParticles[particle].alive())
		{
			cannonParticles[particle].position = glm::vec2(xPosition(randomEngine), yPosition(randomEngine));
			cannonParticles[particle].direction = player.previousCannon->getFireDirection();
			cannonParticles[particle].speed = 500.0f;
			cannonParticles[particle].timeToLive = 0.5f;
			liveParticle++;
		}
		particle++;
	}
}

void FireDeathParticles()
{
	glm::vec2 direction(0.0f, -1.0f);
	glm::vec2 playerCenter = glm::vec2(player.sprite.getPosition().x + player.SIZE * 0.5f, player.sprite.getPosition().y + player.SIZE * 0.5f);
	int liveParticles = 0;
	int currentDirection = 0;
	int numberOfDirections = 8;
	float rotationChange = 0.785f;

	
	while (currentDirection < numberOfDirections && liveParticles < deathParticles.size())
	{
		deathParticles[liveParticles].direction = direction;
		deathParticles[liveParticles].position = playerCenter + float(player.SIZE) * direction;
		deathParticles[liveParticles].speed = 200.0f;
		deathParticles[liveParticles].timeToLive = 2.0f;

		liveParticles++;

		deathParticles[liveParticles].direction = direction;
		deathParticles[liveParticles].position = playerCenter;
		deathParticles[liveParticles].speed = 200.0f;
		deathParticles[liveParticles].timeToLive = 2.0f;

		liveParticles++;
	
		direction = glm::rotate(direction, rotationChange);

		currentDirection++;
	}
}

bool LoadLevel(int level)
{
	//load file
	std::stringstream levelStream;
	levelStream << "Levels/map" << level << ".txt";
	std::string something = levelStream.str();
	std::ifstream map(levelStream.str());
	int currentCannon = 0;
	int currentSpike = 0;

	bool levelGood = true;
	bool levelHasKey = false;
	bool levelHasLive = false;
	bool levelHasStart = false;
	bool levelHasExit = false;

	while (map.good())
	{
		std::string object;

		map >> object;

		if (object == "Key")
		{
			//load key
			if (!levelHasKey)
			{
				std::string line;
				map.ignore();
				std::getline(map, line);
				if (!line.empty())
				{
					std::stringstream stream(line);
					glm::vec2 position;
					stream >> position.x >> position.y;
					key.init(position, uvs[KEY_UV]);
					levelHasKey = true;
				}
			}
		}
		else if (object == "Live")
		{
			//load extra live
			if (!levelHasLive)
			{
				std::string line;
				map.ignore();
				std::getline(map, line);
				if (!line.empty())
				{
					std::stringstream stream(line);
					glm::vec2 position;
					stream >> position.x >> position.y;
					life.init(position, uvs[PLAYER_UV]);
					levelHasLive = true;
				}
			}

		}
		else if (object == "Start")
		{
			//load an autofire cannon and save the location of it in the vector to the variable startCannon;
			if (!levelHasStart)
			{
				glm::vec2 position;
				float rotation;
				map >> position.x >> position.y >> rotation;

				Cannon newCannon;
				newCannon.init(0, position, rotation, false, uvs[AUTO_UV]);
				newCannon.sprite.alpha = 0.0f;
				cannons.push_back(newCannon);
				startCannon = cannons.size() - 1;
				levelHasStart = true;
			}

			//cannons[currentCannon].sprite.alpha = 0.0f;
			currentCannon++;
		}
		else if (object == "Exit")
		{
			//load an exit cannon as shown below and save itfs location in the vector to the variable exitCannon;
			if (!levelHasExit)
			{
				glm::vec2 position;
				float rotation;
				bool locked;
				map >> position.x >> position.y >> rotation >> locked;

				Cannon newCannon;
				glm::vec4 uv;
				if (locked)
				{
					uv = uvs[LOCK_UV];
				}
				else
				{
					uv = uvs[EXIT_UV];
				}
				newCannon.init(3, position, rotation, locked, uv);
				cannons.push_back(newCannon);
				exitCannon = cannons.size() - 1;
				levelHasExit = true;
			}
			currentCannon++;
		}

		else if (object == "Cannons")
		{
			int numberOfCannons;
			map >> numberOfCannons;
			int start = cannons.size();
			numberOfCannons += start;
			cannons.reserve(numberOfCannons);
			map.ignore();
			//see below, loop by number of cannons
			for (size_t i = start; i < numberOfCannons; i++)
			{
				std::string line;

				std::getline(map, line);
				if (!line.empty())
				{
					std::stringstream stream(line);

					int type, xPosition, yPosition, rotation; //rotation in degrees
					stream >> type >> xPosition >> yPosition >> rotation;

					if (type != 0)
					{
						if (type == 1)
						{
							bool active;
							int xEnd, yEnd;
							float speed;
							stream >> active >> xEnd >> yEnd >> speed;
							//create cannon and move behaviour from loaded variables
							Cannon newCannon;
							newCannon.init(type, glm::vec2(xPosition, yPosition), rotation, active, uvs[MOVE_UV]);
							newCannon.behaviour = new Move(newCannon, glm::vec2(xEnd, yEnd), speed);
							cannons.push_back(newCannon);

							currentCannon++;
						}
						else if (type == 2)
						{
							bool active;
							int endRotation;
							float speed;
							stream >> active >> endRotation >> speed;
							//create cannon and rotate behaviour from loaded variables
							Cannon newCannon;
							newCannon.init(2, glm::vec2(xPosition, yPosition), rotation, active, uvs[ROTATE_UV]);
							newCannon.behaviour = new Rotate(newCannon, endRotation, speed);
							cannons.push_back(newCannon);

							currentCannon++;
						}
					}
					else
					{
						//create autofire cannon using above ints
						Cannon newCannon;
						newCannon.init(0, glm::vec2(xPosition, yPosition), rotation, false, uvs[AUTO_UV]);
						cannons.push_back(newCannon);

						currentCannon++;
					}
				}
				else
				{
					//More cannons are listed than are actually there. Just exit this loop
					numberOfCannons = 0;
				}
			}
		}
		else if (object == "Spikes")
		{
			//same process as with cannons
			int numberOfSpikes;
			map >> numberOfSpikes;
			spikes.reserve(numberOfSpikes);
			map.ignore();
			for (size_t i = 0; i < numberOfSpikes; i++)
			{
				std::string line;

				std::getline(map, line);
				if (!line.empty())
				{
					std::stringstream stream(line);
					std::vector<int> attribuites;

					int attrib;

					while (stream >> attrib)
					{
						attribuites.push_back(attrib);
					}
					Spike newSpike;

					//If there are only two it means that only the one position is there which means this spike is stationary
					newSpike.init(glm::vec2(attribuites[0], attribuites[1]), uvs[SPIKE_UV]);
					if (attribuites.size() >= 5)
					{
						//create spikes with myNumbers[0] and myNumbers[1] being the x and y positions	
						newSpike.behaviour = new Move(newSpike, glm::vec2(attribuites[2], attribuites[3]), attribuites[4]);
					}
					spikes.push_back(newSpike);
					currentSpike++;
				}
				else
				{
					//More spikes are listed than are actually there. Just exit this loop
					numberOfSpikes = 0;
				}
			}
		}
	}

	if (!levelHasStart || !levelHasExit || (levelHasExit && cannons[exitCannon].active && !levelHasKey))
	{
		levelGood = false;
	}
	return levelGood;
}

void UnloadLevel()
{
	player.End();
	key.sprite.active = false;
	life.sprite.active = false;
	for (size_t i = 0; i < cannons.size(); i++)
	{
		if (cannons[i].behaviour != nullptr)
		{
			delete cannons[i].behaviour;
		}
	}
	for (size_t i = 0; i < spikes.size(); i++)
	{
		if (spikes[i].behaviour != nullptr)
		{
			delete spikes[i].behaviour;
		}
	}

	cannons.clear();
	spikes.clear();
}


