#pragma once
#include <glm.hpp>
#include "Sprite.h"
#include "BehaviourObject.h"
class Spike : public BehaviourObject
{
public:
	const static int SIZE = 64;
	Spike();
	
	~Spike();


	glm::vec4 getUVs()
	{
		return sprite.getUV();
	}


	void init(glm::vec2 p, const glm::vec4 &u);

private:
	
};

