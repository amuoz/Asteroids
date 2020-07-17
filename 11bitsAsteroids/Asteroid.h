#ifndef ASTEROID_H
#define ASTEROID_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Physics.h"
#include "Config.h"
#include "Game.h"

class Mesh;
class Shader;
	
class Asteroid
{
public:

	Asteroid();
	~Asteroid();

	void Init();
	void Reset();

	void Render(Shader &shader);
	void Update(float deltaTime);

	void SetActive(bool newActive);

	float Randf(float min, float max);

	bool OutOfBounds();

private:

	glm::vec3 m_position;
	glm::vec3 m_scale;
	float m_rotAngle;
	float m_radius;
	glm::vec3 m_velocity;

	// physics pointer
	Physics::PhysicActor* m_physicsActor = 0;

	Mesh* m_mesh;
};

#endif