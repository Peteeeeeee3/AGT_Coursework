#pragma once
#include <engine.h>
#include "tower.h"
#include "engine/utils/timer.h"
#include "enemy.h"
#include "billboard.h"

class candle : public tower
{
public:
	candle(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);
	~candle();
	void init() override;
	void update(std::vector<engine::ref<enemy>> enemies, float dt) override;
	void turret_camera(engine::perspective_camera& camera, float dt);
	void attack() override;
	engine::ref<billboard> flame() { return m_flame; }

	void toggle_cam(bool toggle) { m_active_cam = toggle; }
	bool active_cam() { return m_active_cam; }

	static engine::ref<candle> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

private:
	float						m_flash_frequency;
	float						m_stun_duration;
	bool						m_inTurret = false;
	bool						m_active_cam = false;
	engine::ref<billboard>		m_flame;
};
