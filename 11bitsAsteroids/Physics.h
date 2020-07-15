#ifndef PHYSICS_H
#define PHYSICS_H

#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MAX_DYNAMICS 1000

class Physics
{
public:
	
	struct PhysicActor
	{
		glm::vec3 vel;
		glm::vec3 pos;
	};

	// physics sphere geometry
	struct sDynamicGeometryCircle
	{
		float radius;
		PhysicActor actorInfo;
	};

	Physics(const glm::vec3 &gravity);
	~Physics();

	void Update(float deltaTime);
	void UpdateDymanicPos(sDynamicGeometryCircle &geom, float deltaTime);

	// sphere collision
	bool CheckCircleCircleCollision(const glm::vec3 &circle1Pos, float circle1Radius, const glm::vec3 &circle2Pos, float circle2Radius,
		glm::vec3 &col, glm::vec3 &normal);

	PhysicActor* AddDynamicActor(const glm::vec3 &pos, const glm::vec3 &vel, float radius);

private:

	glm::vec3 m_gravityForce;

	unsigned int m_numDynamic;
	sDynamicGeometryCircle *m_dynamicActors[MAX_DYNAMICS];
};

#endif // !PHYSICS_H
