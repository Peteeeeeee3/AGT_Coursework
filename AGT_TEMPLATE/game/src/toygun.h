#pragma once
#include <engine.h>
#include "tower.h"

class toygun : public tower
{
public:
	toygun(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);
	~toygun();
	void init() override;
	void update(float dt) override;
	void rotate(bool rotate_left, float dt);
	void attack() override;

	static engine::ref<toygun> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

private:
	bool movingUp = true;
	float m_mSpeed = 0.25f;
};
