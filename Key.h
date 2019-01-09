#pragma once
#include "Sprite.h"
class Key
{
public:
	const static int SIZE = 64;
	Key();
	~Key();

	void init(glm::vec2 p, const glm::vec4 &uv);

	Sprite sprite;
};

