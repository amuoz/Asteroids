#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <string>
#include <iostream>
#include <list>

class Asteroid;

class AsteroidPool
{
private:
	std::list<Asteroid*> m_pool;

public:

	AsteroidPool();
	~AsteroidPool();

	/**
	 * Returns instance of Asteroid.
	 *
	 * New resource will be created if all the resources
	 * were used at the time of the request.
	 */
	Asteroid* getAsteroid();
	
	/**
	 * Return asteroid back to the pool.
	 *
	 * The resource must be initialized back to
	 * the default settings before someone else
	 * attempts to use it.
	 */
	void returnAsteroid(Asteroid* resource);

};


#endif