#include "Rotate.h"

Rotate::Rotate()
{
}


Rotate::~Rotate()
{
}

Rotate::Rotate(BehaviourObject & object, int endRotation, float sRotation)
{
	start = int(glm::degrees(object.sprite.rotate));
	end = endRotation;
	
	speed = sRotation;
	if (start != end && speed < 0)
	{
		speed *= -1;
	}
}

static float t = 0.0f;

void Rotate::Update(BehaviourObject & object, float delta)
{
	if (start != end)
	{
		rotateTime += delta;
		if (rotateTime >= 1.0f / speed)
		{
			t += 5.0f * delta;

			object.sprite.rotate = glm::radians(float(glm::mix(start, end, t)));
			if (t > 1.0f)
			{
				rotateTime = 0.0f;
				int temp = end;
				end = start;
				start = temp;
				t = 0.0f;
			}
		}
	}
	else
	{
		object.sprite.rotate +=  speed * delta;
	}

	if (glm::degrees(object.sprite.rotate) >= 360 || glm::degrees(object.sprite.rotate) <= -360)
	{
		object.sprite.rotate = 0;
	}

}