#include "Key.h"



Key::Key()
{
}


Key::~Key()
{
}

void Key::init(glm::vec2 p, const glm::vec4 & uv)
{
	glm::vec4 hb(p.x + 16, p.y + 4, 30, 54);
	sprite.init(p, uv, SIZE, hb);
}
