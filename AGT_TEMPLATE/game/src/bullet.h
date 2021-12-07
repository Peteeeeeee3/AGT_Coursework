#pragma once
#include <engine.h>

class bullet
{
public:
	bullet(engine::game_object_properties& props, float range, glm::vec3 direction, glm::vec3 origin);
	~bullet() {}
	void on_update(float dt);

private:
	glm::vec3							m_direction;
	float								m_velocity;
	engine::ref<engine::game_object>	m_object;
	float								m_distance;
	float								m_range;
	glm::vec3							m_origin;
	bool								m_range_limit = false;
};
