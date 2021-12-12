// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#pragma once
#include <engine.h>
#include "enemy.h"
#include "engine/utils/bounding_box.h"

class bullet
{
public:
	bullet(engine::game_object_properties& props, float range, glm::vec3 direction, glm::vec3 origin, float damage);
	~bullet() {}
	void on_update(std::vector<engine::ref<enemy>>& enemies, float dt);

	bool remove() { return m_to_remove; }

	engine::ref<engine::game_object> object() { return m_object; }

private:
	glm::vec3							m_direction;
	float								m_velocity;
	engine::ref<engine::game_object>	m_object;
	float								m_distance;
	float								m_range;
	float								m_damage;
	glm::vec3							m_origin;
	bool								m_to_remove = false;
	std::vector<engine::ref<enemy>>		m_active_enemies;
	engine::bounding_box				m_bounding_box;
	

	void handle_collision();
};
