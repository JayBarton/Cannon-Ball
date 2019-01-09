#include "Sprite.h"

Sprite::Sprite() : position(0.0f)
{
}


Sprite::~Sprite()
{
}

void Sprite::init(glm::vec2 p, const glm::vec4 & u, float s, const glm::vec4 & hb, bool a/* = true*/, float alph /* = 1.0f*/)
{
	active = a;
	alpha = alph;
	position = p;
	uv = u;
	size = s;
	hitBox = hb;
}

void Sprite::Move(glm::vec2 movement)
{
	

	glm::vec2 newPosition = position + movement;
	SetPosition(newPosition);
	/*position += movement;

	hitBox.x += movement.x;
	hitBox.y += movement.y;*/
}

void Sprite::SetPosition(glm::vec2 newPosition)
{
	float xOffset = hitBox.x - position.x;
	float yOffset = hitBox.y - position.y;
	position = newPosition;
	hitBox.x = position.x + xOffset;
	hitBox.y = position.y + yOffset;
}
