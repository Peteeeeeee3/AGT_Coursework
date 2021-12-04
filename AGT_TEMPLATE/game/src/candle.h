#pragma once
#include <engine.h>
#include "tower.h"
#include "engine/utils/timer.h"
#include "enemy.h"

class candle : public tower
{
public:
	candle(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);
	~candle();
	void init() override;
	void update() override;
	void attack() override;

	static engine::ref<candle> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

private:
	float								m_flash_frequency;
	float								m_stun_duration;
	bool								m_inTurret = false;
};
