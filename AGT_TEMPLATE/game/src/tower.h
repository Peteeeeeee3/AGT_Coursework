#pragma once
#include <engine.h>
#include "enemy.h"
#include "engine/utils/timer.h"

// the current state of this class is only temporary
// this class will be redeveloped to support towers in general
// in this current build it is mostly a place holder as it only handles the rotation of the menu guns

class tower : public engine::game_object
{
public:
	tower(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);
	~tower();
	virtual void init();
	virtual void update();
	virtual void attack();
	virtual void upgradeRight_lvl1();
	virtual void upgradeRight_lvl2();
	virtual void upgradeLeft_lvl1();
	virtual void upgradeLeft_lvl2();
	

	static engine::ref<tower> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

protected:
	std::vector<engine::game_object> enemies_in_range;
	float m_damage;
	float m_attack_speed;
	float m_range;
	std::vector<engine::ref<enemy>>		m_active_enemies;
	engine::timer						m_attack_timer;
};
