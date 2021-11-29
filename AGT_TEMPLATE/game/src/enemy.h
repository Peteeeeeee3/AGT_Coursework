#pragma once
#include <engine.h>
#include "player.h"

class enemy : public engine::game_object
{
public:
	enum e_type {
		REG, HEAVY, GUARD, SPEC
	};

	enemy(const engine::game_object_properties& props, float health, float strength, float speed, e_type type);
	~enemy() {}
	void update(player& player, std::vector<glm::vec3> checkpoints, float dt);
	e_type type() { return m_type; }

	static engine::ref<enemy> create(const engine::game_object_properties& props, float health, float strength, float speed, e_type type);

private:
	float m_health;
	float m_strength;
	float m_speed;
	bool m_isRotated = false;
	e_type m_type;

	enum e_section {
		START, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, FINISH
	};
	e_section m_location_state = START;
};
