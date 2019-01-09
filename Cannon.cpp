#include "Cannon.h"
#include <iostream>

Cannon::Cannon()
{

}

Cannon::~Cannon()
{
}

void Cannon::init(int t, glm::vec2 p, float rotation, bool a, const glm::vec4 &u)
{
	type = t;
	glm::vec4 hb(p.x, p.y, CANNON_SIZE, CANNON_SIZE);
	sprite.init(p, u, CANNON_SIZE, hb);
	sprite.rotate = glm::radians(rotation);	

	alwaysActive = a;
	active = a;
	collideable = true;
}

glm::vec2 Cannon::getFireDirection()
{
	
	glm::vec2 direction = glm::rotate(fireDirection, sprite.rotate);
	return direction;
}
