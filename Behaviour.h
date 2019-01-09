#pragma once
#include <glm.hpp>
#include "BehaviourObject.h"
class BehaviourObject;
class Behaviour
{
public:
	Behaviour();
	~Behaviour();


	virtual void Update(BehaviourObject &object, float delta)
	{

	}
};

