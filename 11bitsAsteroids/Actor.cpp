#include "Actor.h"

Actor::Actor()
{
	m_active = true;
}

Actor::~Actor()
{
	//delete m_physicsActor;
}

void Actor::SetActive(bool newActive)
{
	m_physicsActor->active = newActive;
}
