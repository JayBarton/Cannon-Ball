#pragma once
#include <glm.hpp>
#include "Cannon.h"
#include "Sprite.h"
#include <SDL.h>
class Player
{
public:
	const static int SIZE = 64; 
	const static int STARTING_LIVES = 3;

	Player();
	~Player();

	int lives;

	glm::vec2 direction;

	float speed;

	Cannon *cannon = nullptr;
	Cannon *previousCannon = nullptr;

	Sprite sprite;


	void init(glm::vec2 p, const glm::vec4 &u);

	void Update(float delta);
	bool CheckColision(const glm::vec4 &otherBox); 

	void SetCannon(Cannon &c);

	void Fire();

	void End();
	bool win;
	bool hasKey;
	
private:


	
};

