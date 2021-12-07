#pragma once
#include <engine.h>
#include "tower.h"
#include "lightning_bolt.h"

class wizard_hat : public tower
{
public:
	wizard_hat(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);
	~wizard_hat();
	void init() override;
	void update(std::vector<engine::ref<enemy>> enemies, float dt) override;
	void attack() override;

	std::vector<engine::ref<lightning_bolt>> bolt() { return m_bolt; }

	static engine::ref<wizard_hat> create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies);

private:
	glm::vec3 m_lightning_direction;
	std::vector<engine::ref<lightning_bolt>> m_bolt;
};
