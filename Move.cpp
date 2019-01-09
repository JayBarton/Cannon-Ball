#include "Move.h"
#include <SDL.h>

Move::Move()
{
}


Move::~Move()
{
}

Move::Move(BehaviourObject &object, glm::vec2 e, float s)
{
	start = object.sprite.getPosition();
	end = e;
	speed = s;
}

void Move::Update(BehaviourObject &object, float delta)
{
	//std::cout << "HELP" << std::endl;
	//object.sprite.SetPosition(glm::vec2(1.0f, 0.0f) * 500.0f * delta);
	object.sprite.SetPosition(glm::mix(start, end, pow(sin(t), 2)));
	t += speed * delta;
}
