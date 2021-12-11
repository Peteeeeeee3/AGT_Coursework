#pragma once
#include <engine.h>
#include "enemy.h"
#include "engine/utils/bounding_box.h"
#include "billboard.h"
#include "shockwave.h"

class fire_ball
{
public:
	fire_ball();
	~fire_ball();
	void shoot(const engine::perspective_camera& camera);
	void on_update(std::vector<engine::ref<enemy>> enemies, float dt);
	void on_render(const engine::ref<engine::shader>& shader);
	bool physics_bound() { return m_physics_bound; }
	void physics_applied() { m_physics_bound = true; }
	engine::ref<engine::game_object> object() { return m_object; }
	bool to_remove() { return m_to_remove; }
	void flag_remove() { m_to_remove = true; }
	engine::ref<billboard> flame() { return m_flame; }
	float damage() { return m_flame_damage; }
	float aoe_range() { return m_range; }

private:
	engine::ref<engine::game_object>	m_object;
	std::vector<engine::ref<enemy>>		m_active_enemies;
	glm::vec3							m_position;
	glm::vec3							m_instantaneous_acceleration{ 0.f };
	engine::ref<billboard>				m_flame{};
	float								m_contact_time = 0.05f;
	float								m_range;
	float								m_flame_damage;
	bool								m_physics_bound = false;
	bool								m_to_remove = false;

	void init();
};
