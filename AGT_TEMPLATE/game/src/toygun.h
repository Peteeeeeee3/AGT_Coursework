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
	engine::ref<enemy> find_target();
	void render_bullets(engine::ref<engine::shader> shader);

	static engine::ref<toygun> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

private:
	bool							movingUp = true;
	float							m_mSpeed = 0.25f;

	engine::ref<enemy>				m_target;

	std::vector<bullet>				m_bullets;
	engine::game_object_properties	m_bullet_props;
	glm::vec3						m_forward_vec;
};
