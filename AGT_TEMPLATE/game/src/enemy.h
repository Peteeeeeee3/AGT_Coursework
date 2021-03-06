// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#pragma once
#include <engine.h>
#include "player.h"
#include "engine/utils/bounding_box.h"
#include "engine/utils/timer.h"

class enemy : public engine::game_object
{
public:
	enum e_type {
		SPIDER, MECH, CLAPTRAP, IRONMAN
	};

	enemy(const engine::game_object_properties& props, float health, float strength, float speed, float spawn_time, e_type type);
	~enemy() {}
	void update(player& player, std::vector<engine::ref<enemy>> enemies, std::vector<glm::vec3> checkpoints, float dt);
	e_type type() { return m_type; }
	bool isDead() { return m_isDead; }
	bool toRender() { return m_life_time >= m_spawn_time; }
	engine::bounding_box bounding_box() { return m_bounding_box; }
	void stun(float duration) { m_stun_duration = duration; m_isStunned = true; }
	bool isStunnded() { return m_isStunned; }
	void damage(float inflict) { m_health -= inflict; }
	void heal(float inflict);
	float strength() { return m_strength; }
	int state() { return m_location_state; }
	float distance_covered() { return m_distance_covered; }

	static engine::ref<enemy> create(const engine::game_object_properties& props, float health, float strength, float speed, float spawn_time, e_type type);

private:
	float m_health_cap;
	float m_health;
	float m_strength;
	float m_speed;
	e_type m_type;
	bool m_isDead = false;
	float m_spawn_time;
	float m_life_time = 0.f;
	engine::bounding_box m_bounding_box;
	float m_stun_duration;
	bool m_isStunned = false;
	bool m_dealt_damage = false;
	float m_stun_timer = 0.f;
	float m_healing_speed = 5.f;
	float m_healing_timer = 0.f;
	float m_distance_covered = 0.f;


	enum e_section {
		START, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, FINISH
	};
	e_section m_location_state = START;
};
