#ifndef ASTEROID_MGR_H
#define ASTEROID_MGR_H

#include <string>
#include <iostream>
#include <vector>

// forwards
class Asteroid;
class AsteroidPool;
class Shader;

class AsteroidMgr
{
public:
	AsteroidMgr();
	~AsteroidMgr();

	void Update(float deltaTime);

	void Render(Shader &shader);

private:
	// active asteroids
	std::vector<Asteroid*> m_asteroids;	

	// pool
	AsteroidPool* m_pool;

	// time accumulation to spawn
	float m_timeAccum;

	// current spawn frequency
	float m_currentFreq;

	void SpawnAsteroid();

};

#endif
