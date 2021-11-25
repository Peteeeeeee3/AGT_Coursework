#pragma once
#include <engine.h>
#include "tower.h"

class toygun : public tower
{
public:
	toygun(const engine::game_object_properties props);
	~toygun();
	void init() override;
	void update() override;
	void rotate(bool rotate_left, float dt);
	void attack() override;

	static engine::ref<toygun> create(const engine::game_object_properties& props);

private:
	bool movingUp = true;
	float m_mSpeed = 0.25f;
};