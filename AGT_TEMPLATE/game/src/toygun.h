// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#pragma once
#include <engine.h>
#include "tower.h"
#include "bullet.h"

class toygun : public tower
{
public:
	toygun(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);
	~toygun();
	void init() override;
	void update(std::vector<engine::ref<enemy>> enemies, float dt) override;
	void rotate(bool rotate_left, float dt);
	void attack() override;
	virtual void upgradeRight_lvl1(player& player) override;
	virtual void upgradeRight_lvl2(player& player) override;
	virtual void upgradeLeft_lvl1(player& player) override;
	virtual void upgradeLeft_lvl2(player& player) override;
	engine::ref<enemy> find_target();
	void render_bullets(engine::ref<engine::shader> shader);
	bool play_shot_sound() { return m_play_shot_sound; }

	static engine::ref<toygun> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

private:
	bool							movingUp = true;
	float							m_mSpeed = 0.25f;

	engine::ref<enemy>				m_target;

	std::vector<bullet>				m_bullets;
	engine::game_object_properties	m_bullet_props;
	glm::vec3						m_forward_vec;

	bool							m_play_shot_sound = false;

	int target_index();
};
