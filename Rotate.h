#pragma once

#include <iostream>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>
#include "Behaviour.h"
#include "Cannon.h"
#include "BehaviourObject.h"
class Rotate : public Behaviour
{
public:
	Rotate();
	~Rotate();

	int start;
	int end;

	float speed;
	float rotate;
	float rotateTime = 0.0f;
	Rotate(BehaviourObject& object, int endRotation, float sRotation);
	void Update(BehaviourObject &object, float delta);
};

