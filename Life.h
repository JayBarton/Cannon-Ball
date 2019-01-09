#pragma once
#include "Sprite.h"
class Life
{
public:
	const static int SIZE = 64;
	Life();
	~Life();

	void init(glm::vec2 p, const glm::vec4 &uv);

	Sprite sprite;
};

