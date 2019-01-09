#include "BehaviourObject.h"
#include <iostream>


BehaviourObject::BehaviourObject()
{
}


BehaviourObject::~BehaviourObject()
{
}

void BehaviourObject::Update(float delta)
{
	behaviour->Update(*this, delta);
}
