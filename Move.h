#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>
#include "Behaviour.h"
#include "Cannon.h"
#include "BehaviourObject.h"

class Move : public Behaviour
{
public:
	Move();
	Move(BehaviourObject &object, glm::vec2 e, float s);
	~Move();

	glm::vec2 start;
	glm::vec2 end;

	float t = 0.0f;
	float speed;

	void Update(BehaviourObject &object, float delta);
};


