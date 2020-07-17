#include "Common.h"
#include "AsteroidPool.h"
#include "Asteroid.h"


AsteroidPool::AsteroidPool()
{

}

AsteroidPool::~AsteroidPool() {
	while (m_pool.size())
	{
		Asteroid* obj = m_pool.front();
		m_pool.pop_front();
		delete obj;
	}
}

Asteroid* AsteroidPool::getAsteroid()
{
	if (m_pool.empty())
	{
		Asteroid* resource = new Asteroid();
		//std::cout << "New Asteroid created!" << std::endl;
		return resource;
	}
	else
	{
		Asteroid* resource = m_pool.front();
		m_pool.pop_front();
		//std::cout << "Asteroid reused!" << std::endl;
		return resource;
	}
}

void AsteroidPool::returnAsteroid(Asteroid* resource)
{
	resource->Reset();
	m_pool.push_back(resource);
}