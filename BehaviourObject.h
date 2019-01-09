#pragma once
#include "Behaviour.h"
#include "Sprite.h"
class Behaviour;
class BehaviourObject
{
public:
	BehaviourObject();
	~BehaviourObject();

	Sprite sprite;
	Behaviour *behaviour = nullptr;

	void Update(float delta);

};

