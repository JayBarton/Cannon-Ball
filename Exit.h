#pragma once
#include "Behaviour.h"
class Exit : public Behaviour
{
public:
	Exit();
	Exit(bool lock);
	~Exit();

	bool locked;
};

