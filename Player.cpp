#include "Player.h"
#include "Exit.h"



Player::Player() : direction(0.0f), speed(850.0f), hasKey(false), win(false)
{
}


Player::~Player()
{
}

void Player::init(glm::vec2 p, const glm::vec4 & u)
{
  glm::vec4 hb(p.x+ 23, p.y + 23, 18, 18);

	sprite.init(p, u, SIZE, hb);
}

void Player::Update(float delta)
{
	if (cannon == nullptr)
	{
		sprite.Move(direction * speed * delta);
	}
	else
	{
		if (cannon->getType() != 0) //0 types should never change position, so no need to update it
		{
			sprite.SetPosition(cannon->sprite.getPosition()); 
		}
	}
}

bool Player::CheckColision(const glm::vec4 &otherBox)
{
	int oLeft = otherBox.x;
	int oRight = otherBox.x + otherBox.z;
	int oTop = otherBox.y;
	int oBottom = otherBox.y + otherBox.w;

	int left = sprite.getHitBox().x;
	int right = sprite.getHitBox().x + sprite.getHitBox().z;
	int top = sprite.getHitBox().y;
	int bottom = sprite.getHitBox().y + sprite.getHitBox().w;

	if (bottom >= oTop && top <= oBottom && right >= oLeft && left <= oRight)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Player::SetCannon(Cannon &c)
{
	if (c.getType() == 3)
	{
		bool locked = c.active;
		if (!locked)
		{
			sprite.active = false;
			win = true;
		}
		else if (locked && hasKey)
		{
			sprite.active = false;
			c.active = false;
			win = true;
		}
		else
		{
			previousCannon->collideable = true;
			previousCannon = &c;
			previousCannon->collideable = false;
		}
	}
	else
	{
		sprite.active = false;
		cannon = &c;
		if (cannon->getType() != 0 && !cannon->active) // 0 types should never be active
		{
			cannon->active = true;
		}
		cannon->collideable = false;
		if (previousCannon != nullptr)
		{
			previousCannon->collideable = true;
		}
		sprite.SetPosition(cannon->sprite.getPosition());

	}
}

void Player::Fire()
{
	//If Rotating cannon
	if (cannon->getType() == 2)
	{
		cannon->sprite.rotate = round(cannon->sprite.rotate / glm::radians(45.0f)) * glm::radians(45.0f);
	}
	sprite.SetPosition(cannon->sprite.getPosition());
	direction = cannon->getFireDirection();
	sprite.active = true;
	if (!cannon->alwaysActive)
	{
		cannon->active = false;
	}
	previousCannon = cannon;
	cannon = nullptr;
}

void Player::End()
{
	cannon = nullptr;
	previousCannon = nullptr;
}
