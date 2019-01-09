#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>
#include "Behaviour.h"
#include "Sprite.h"
class Behaviour;
class Cannon : public BehaviourObject
{
public:
	static const int CANNON_SIZE = 64;

	Cannon();
	~Cannon();

	bool active;
	bool alwaysActive;
	bool collideable;

	glm::vec4 getUVs()
	{
		return sprite.getUV();
	}

	int getType()
	{
		return type;
	}

	void init(int t, glm::vec2 p, float rotation, bool a, const glm::vec4 &u);

	glm::vec2 fireDirection = glm::vec2(0.0f, -1.0f);
	glm::vec2 getFireDirection();

private:

	int type;


};

