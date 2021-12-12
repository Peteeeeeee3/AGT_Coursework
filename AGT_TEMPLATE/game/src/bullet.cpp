// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include "bullet.h"

bullet::bullet(engine::game_object_properties& props, float range, glm::vec3 direction, glm::vec3 origin, float damage) : m_range(range), m_direction(direction), m_origin(origin), m_damage(damage)
{
	m_object = engine::game_object::create(props);
	m_velocity = 20.f;
	m_distance = 0.f;
	m_bounding_box = engine::bounding_box::bounding_box();
	m_bounding_box.set_box(0.2f, 0.2f, 0.2f, m_object->position());
}

void bullet::on_update(std::vector<engine::ref<enemy>>& enemies, float dt)
{
	m_active_enemies = enemies;
	m_bounding_box.on_update(m_object->position());

	if (m_distance >= m_range)
		m_to_remove = true;

	m_object->set_position(m_object->position() + glm::normalize(m_direction) * dt * m_velocity);
	m_distance = glm::length(m_origin - m_object->position());

	handle_collision();
}

void bullet::handle_collision()
{
	for (auto enemy : m_active_enemies)
	{
		if (m_bounding_box.collision(enemy->bounding_box()))
		{
			enemy->damage(m_damage);
			m_to_remove = true;
		}
	}
}
