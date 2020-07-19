#ifndef ACTOR_H
#define ACTOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Physics.h"
#include "Mesh.h"

class Actor
{
public:
	virtual void Render(Shader &shader) = 0;
	virtual void Update(float deltaTime) = 0;
	
	void SetActive(bool newActive);

protected:
	glm::vec3 m_position;
	glm::vec3 m_scale;
	float m_rotAngle;
	glm::vec3 m_rotAxis;
	float m_radius;
	glm::vec3 m_velocity;

	// physics pointer
	Physics::PhysicActor* m_physicsActor = 0;

	Mesh* m_mesh = 0;
};

#endif