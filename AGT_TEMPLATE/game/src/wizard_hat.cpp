#include "wizard_hat.h"

wizard_hat::wizard_hat(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : tower(props, enemies)
{
	m_damage = 10.f;
	m_attack_speed = 10.f;
	m_range = 10.f;
	init_range();
}

wizard_hat::~wizard_hat() {}

void wizard_hat::init()
{
	m_elapsed = 0.f;
}

void wizard_hat::update(float dt) {}

void wizard_hat::attack() {}

engine::ref<wizard_hat> wizard_hat::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<wizard_hat>(props, enemies);
}
