#include "bullet.h"

bullet::bullet(engine::game_object_properties& props, float range, glm::vec3 direction, glm::vec3 origin) : m_range(range), m_direction(direction), m_origin(origin)
{
	m_object = engine::game_object::create(props);
	m_velocity = 10.f;
	m_distance = 0.f;
}

void bullet::on_update(float dt)
{
	if (m_distance >= m_range)
		m_range_limit = true;

	m_object->set_position(m_object->position() + dt * m_velocity);
	m_distance = glm::length(m_origin - m_object->position());
}
