#ifndef ASTEROID_MGR_H
#define ASTEROID_MGR_H

#include <string>
#include <iostream>
//#include <vector>
#include <list>

#include "AsteroidPool.h"

// forwards
class Asteroid;
class Shader;

class AsteroidMgr
{
public:
	AsteroidMgr();
	~AsteroidMgr();

	void Update(float deltaTime);

	void Render(Shader shader);

	void Reset();

private:
	// active asteroids
	std::list<Asteroid*> m_asteroids;	

	// Asteroid pool
	AsteroidPool<Asteroid>* m_pool;

	// time accumulation to spawn
	float m_timeAccum;

	// current spawn frequency
	float m_currentFreq;

	void SpawnAsteroid();

};

#endif
