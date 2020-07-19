#include "Actor.h"

void Actor::SetActive(bool newActive)
{
	m_physicsActor->active = newActive;
}
