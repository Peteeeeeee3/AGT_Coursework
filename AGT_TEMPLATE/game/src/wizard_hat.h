// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#pragma once
#include <engine.h>
#include "tower.h"
#include "shockwave.h"

class wizard_hat : public tower
{
public:
	wizard_hat(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);
	~wizard_hat();
	void init() override;
	void update(std::vector<engine::ref<enemy>> enemies, float dt) override;
	void attack() override;
	virtual void upgradeRight_lvl1(player& player) override;
	virtual void upgradeRight_lvl2(player& player) override;
	virtual void upgradeLeft_lvl1(player& player) override;
	virtual void upgradeLeft_lvl2(player& player) override;
	shockwave lightning() { return m_lightning; }
	bool play_spark() { return m_play_spark_sound; }

	static engine::ref<wizard_hat> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

private:
	glm::vec3	m_lightning_direction;
	shockwave	m_lightning;

	bool		m_play_spark_sound = false;
};
