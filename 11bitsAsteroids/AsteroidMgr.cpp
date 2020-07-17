#include "AsteroidMgr.h"
#include "Asteroid.h"
#include "AsteroidPool.h"
#include "Common.h"

#include <algorithm>

#define POOL_SIZE 5
#define MAX_FREQ 0.1f

AsteroidMgr::AsteroidMgr()
{
	m_timeAccum = 0.0f;
	m_currentFreq = g_Config->m_freq;
	m_pool = new AsteroidPool();
}

AsteroidMgr::~AsteroidMgr()
{
	delete m_pool;
	m_asteroids.clear();
	// deallocating the memory
	std::vector<Asteroid*>().swap(m_asteroids);
}

void AsteroidMgr::Update(float deltaTime)
{
	m_timeAccum += deltaTime;
	// spawn freq increases over time
	m_currentFreq -= g_Config->m_freqIncrease * deltaTime;
	// clam max freq
	m_currentFreq = std::max(MAX_FREQ, std::min(m_currentFreq, g_Config->m_freq));
	//std::cout << "Frequency: " << m_currentFreq << std::endl;

	for (std::vector<Asteroid*>::iterator it = m_asteroids.begin(); it != m_asteroids.end();)
	{
		Asteroid* asteroid = (*it);
		asteroid->Update(deltaTime);
		
		// if out-of-bounds recycle asteroid
		if (asteroid->OutOfBounds())
		{
			it = m_asteroids.erase(it);
			m_pool->returnAsteroid(asteroid);
		}
		else
		{
			++it;
		}
	}

	// spawn asteriod by frequency
	if (m_timeAccum >= m_currentFreq)
	{
		m_timeAccum -= m_currentFreq;
		SpawnAsteroid();
	}

}

void AsteroidMgr::Render(Shader &shader)
{
	for (std::vector<Asteroid*>::iterator it = m_asteroids.begin(); it != m_asteroids.end(); ++it)
	{	
		Asteroid* asteroid = (*it);
		asteroid->Render(shader);
	}
}

void AsteroidMgr::SpawnAsteroid()
{
	Asteroid* asteroid = m_pool->getAsteroid();
	// activate actor
	asteroid->SetActive(true);
	m_asteroids.push_back(asteroid);
}

