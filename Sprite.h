#pragma once
#include <glm.hpp>
class Sprite
{
public:
	Sprite();
	~Sprite();

	void init(glm::vec2 p, const glm::vec4 &u, float s, const glm::vec4 &hb, bool a = true, float alph = 1.0f);

	glm::vec2 getPosition()
	{
		return position;
	}

	glm::vec4 getHitBox()
	{
		return hitBox;
	}

	glm::vec4 getUV()
	{
		return uv;
	}

	float getSize()
	{
		return size;
	}

	bool active;

	glm::vec4 uv;
	float rotate;
	float alpha;

	void Move(glm::vec2 movement); // add movement to position
	void SetPosition(glm::vec2 newPosition);
private:
	float size;
	glm::vec2 position;
	glm::vec4 hitBox;
	
};

