#pragma once
#include <engine.h>
#include "tower.h"
#include "engine/utils/timer.h"
#include "enemy.h"
#include "billboard.h"
#include "fire_ball.h"

class candle : public tower
{
public:
	candle(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);
	~candle();
	void init() override;
	void update(std::vector<engine::ref<enemy>> enemies, float dt) override;
	void update_shot(engine::perspective_camera& camera, float dt);
	void turret_camera(engine::perspective_camera& camera, float dt);
	void attack() override;
	virtual void upgradeRight_lvl1(player& player) override;
	virtual void upgradeRight_lvl2(player& player) override;
	virtual void upgradeLeft_lvl1(player& player) override;
	virtual void upgradeLeft_lvl2(player& player) override;
	engine::ref<billboard> flame() { return m_flame; }
	fire_ball shot() { return *m_shot; }
	void reset_shot();

	void toggle_cam(bool toggle) { m_active_cam = toggle; }
	bool active_cam() { return m_active_cam; }

	static engine::ref<candle> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

private:
	float						m_flash_frequency;
	float						m_stun_duration;
	bool						m_inTurret = false;
	bool						m_active_cam = false;
	bool						m_active_shot = false;
	engine::ref<billboard>		m_flame{};
	std::shared_ptr<fire_ball>	m_shot;
	float						m_attack_speed;
	float						m_turret_Speed;
};
