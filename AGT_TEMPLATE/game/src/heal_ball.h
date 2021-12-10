#pragma once
#include <engine.h>
#include "enemy.h"

class heal_ball
{
public:
	heal_ball(/*std::vector<engine::ref<enemy>> enemies*/);
	~heal_ball() {}
	void init();
	void launch(glm::vec3 position, glm::vec3 direction);
	void on_update(/*std::vector<engine::ref<enemy>> enemies, */float dt);
	bool to_remove() { return m_to_remove; }
	bool is_active() { return m_is_active; }

private:
	engine::ref<engine::game_object>	m_ball;
	//std::vector<engine::ref<enemy>>		m_active_enemies;
	glm::vec3							m_position;
	float								m_healing_strength;
	float								m_range;
	float								m_rotational_inertia = 0.f;
	float								m_contact_time = 0.f;
	float								m_phi, m_theta;
	glm::vec3							m_instantaneous_acceleration{ 0.f };
	glm::vec3							m_instantaneous_angular_acceleration{ 0.f };
	bool								m_to_remove = false;
	bool								m_is_active = false;

	void heal();
};
