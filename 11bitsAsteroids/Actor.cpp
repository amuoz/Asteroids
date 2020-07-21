#include "Actor.h"

#include "Common.h"

Actor::Actor()
{
	m_active = true;
}

Actor::~Actor()
{
	std::cout << ">>>>>>>> DELETE ACTOR!!!" << std::endl;
	g_PhysicsPtr->DeleteDynamicActor(m_physicsActor);
	delete m_physicsActor;
	delete m_mesh;
}

void Actor::SetActive(bool newActive)
{
	m_physicsActor->active = newActive;
}
