#pragma once
#include <engine.h>
#include "player.h"

class enemy : public engine::game_object
{
public:
	enemy(const engine::game_object_properties& props, float health, float strength, float speed);
	~enemy() {}
	void update(player& player, std::vector<glm::vec3> checkpoints, float dt);

	static engine::ref<enemy> create(const engine::game_object_properties& props, float health, float strength, float speed);

private:
	float m_health;
	float m_strength;
	float m_speed;
	bool m_isRotated = false;

	enum e_section {
		START, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, FINISH
	};
	e_section m_location_state = START;
};
