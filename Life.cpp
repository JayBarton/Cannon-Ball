#include "Life.h"



Life::Life()
{
}


Life::~Life()
{
}

void Life::init(glm::vec2 p, const glm::vec4 & uv)
{
//	glm::vec4 hb(p.x + 16, p.y + 4, 30, 54);
	glm::vec4 hb(p.x, p.y, SIZE, SIZE);
	sprite.init(p, uv, SIZE, hb);
}
