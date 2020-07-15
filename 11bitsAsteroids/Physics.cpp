#include "Physics.h"

Physics::Physics(const glm::vec3 &gravity)
{
	m_gravityForce = gravity;
	m_numDynamic = 0;
}

Physics::~Physics()
{
	for (unsigned int i = 0; i < m_numDynamic; ++i) 
	{
		delete m_dynamicActors[i];
	}
}

void Physics::Update(float deltaTime)
{
	for (unsigned int i = 0; i < m_numDynamic; ++i) 
	{
		UpdateDymanicPos(*m_dynamicActors[i], deltaTime);
	}
}

void Physics::UpdateDymanicPos(sDynamicGeometryCircle &geom, float deltaTime)
{
	// V = V0 + a * t
	// P = Po + V * t
	geom.actorInfo.vel += m_gravityForce * deltaTime;
	glm::vec3 prevPos = geom.actorInfo.pos;
	glm::vec3 newPos = geom.actorInfo.pos + geom.actorInfo.vel * deltaTime;
	glm::vec3 desplDir = newPos - prevPos;
	desplDir = glm::normalize(desplDir);
	geom.actorInfo.pos = newPos;

	glm::vec3 col, normal;
	for (unsigned int i = 0; i < m_numDynamic; ++i)
	{
		if (&geom != m_dynamicActors[i])
		{
			if (CheckCircleCircleCollision(geom.actorInfo.pos, geom.radius, m_dynamicActors[i]->actorInfo.pos, m_dynamicActors[i]->radius, col, normal))
			{
				geom.actorInfo.vel = normal * glm::length(geom.actorInfo.vel);
				geom.actorInfo.pos = col;
			}
		}
	}

}

bool Physics::CheckCircleCircleCollision(const glm::vec3& circle1Pos, float circle1Radius, const glm::vec3& circle2Pos,
	float circle2Radius, glm::vec3& col, glm::vec3& normal)
{
	// Collision if distance between sphere centers is less than radius sum
	float centerDist = glm::distance(circle1Pos, circle2Pos);
	float radiusSum = circle1Radius + circle2Radius;
	if (centerDist < radiusSum) {
		normal = circle1Pos - circle2Pos;
		normal = glm::normalize(normal);

		// Desplazar la esfera A que ha colisionado en la direccion de colision lo suficiente para que los dos radios estén separados
		col = circle2Pos + normal * (circle1Radius + circle2Radius);
		return true;
	}

	return false;
}

Physics::PhysicActor* Physics::AddDynamicActor(const glm::vec3 &pos, const glm::vec3 &vel, float radius)
{
	if (m_numDynamic < MAX_DYNAMICS) 
	{
		sDynamicGeometryCircle *geom = new sDynamicGeometryCircle;
		geom->actorInfo.pos = pos;
		geom->actorInfo.vel = vel;
		geom->radius = radius;
		m_dynamicActors[m_numDynamic++] = geom;
		return &geom->actorInfo;
	}
	return 0;
}