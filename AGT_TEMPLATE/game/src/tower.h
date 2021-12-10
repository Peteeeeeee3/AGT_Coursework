#pragma once
#include <engine.h>
#include "enemy.h"

// the current state of this class is only temporary
// this class will be redeveloped to support towers in general
// in this current build it is mostly a place holder as it only handles the rotation of the menu guns

class tower : public engine::game_object
{
public:
	tower(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);
	~tower();
	virtual void init();
	virtual void update(std::vector<engine::ref<enemy>> enemies, float dt);
	virtual void attack();
	virtual void upgradeRight_lvl1(player& player);
	virtual void upgradeRight_lvl2(player& player);
	virtual void upgradeLeft_lvl1(player& player);
	virtual void upgradeLeft_lvl2(player& player);
	void render_range(engine::ref<engine::shader> shader);
	bool to_render_range() { return m_to_render_range; }
	void set_to_render_range(bool to_render_range) { m_to_render_range = to_render_range; }
	engine::bounding_box bounding_box() { return m_bounding_box; }

	static engine::ref<tower> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

protected:
	void init_range();

	float									m_damage;
	float									m_attack_speed;
	float									m_range;
	std::vector<engine::ref<enemy>>			m_active_enemies;
	float									m_elapsed;
	engine::ref<engine::game_object>		m_range_highlight;
	bool									m_to_render_range = false;
	engine::ref<engine::material>			m_range_material{};
	engine::bounding_box					m_bounding_box;
};
