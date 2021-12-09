#include "wizard_hat.h"

wizard_hat::wizard_hat(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : tower(props, enemies)
{
	m_damage = 10.f;
	m_attack_speed = 1.f;
	m_range = 3.f;
	init();
	init_range();
}

wizard_hat::~wizard_hat() {}

void wizard_hat::init()
{
	m_elapsed = 0.f;
	m_lightning.initialise();
}

void wizard_hat::update(std::vector<engine::ref<enemy>> enemies, float dt)
{
	m_elapsed += dt;

	m_active_enemies = enemies;

	m_lightning.on_update(dt);

	if (m_elapsed >= m_attack_speed)
	{
		m_elapsed = 0.f;
		if (m_active_enemies.size() > 0)
			attack();
	}
}

void wizard_hat::attack()
{
	m_lightning.activate(m_range, glm::vec3(position().x, position().y + 0.18f, position().z));

	for (auto enemy : m_active_enemies)
	{
		if (glm::length(enemy->position() - position()) <= m_range)
		{
			enemy->damage(m_damage);
		}
	}
}

engine::ref<wizard_hat> wizard_hat::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<wizard_hat>(props, enemies);
}
