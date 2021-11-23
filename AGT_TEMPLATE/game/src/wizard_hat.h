#pragma once
#include <engine.h>
#include "tower.h"

class wizard_hat : public tower
{
public:
	wizard_hat(const engine::game_object_properties props);
	~wizard_hat();
	void init() override;
	void update() override;
	void attack() override;

	static engine::ref<wizard_hat> create(const engine::game_object_properties& props);
};
