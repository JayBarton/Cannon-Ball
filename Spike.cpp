#include "Spike.h"



Spike::Spike() 
{
}

Spike::~Spike()
{
}

void Spike::init(glm::vec2 p, const glm::vec4 & u)
{
	glm::vec4 hb(p.x + 8, p.y + 8, 50, 50);

	sprite.init(p, u, SIZE, hb);
}
