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
	m_lightning_direction = glm::vec3(0.f, -1.f, 0.f);
	m_bolt.push_back(lightning_bolt::create(position(), m_lightning_direction, 1.f));
}

void wizard_hat::update(std::vector<engine::ref<enemy>> enemies, float dt)
{
	m_elapsed += dt;

	for (auto bolt : m_bolt)
		bolt->on_update(dt);

	if (m_elapsed >= m_attack_speed)
	{
		m_elapsed = 0.f;
		attack();
	}
}

void wizard_hat::attack()
{
	//generate random x value
	float x = (position().x - m_range) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / ((position().x + m_range) - (position().x - m_range))));
	//generate random z value
	float z = (position().z - m_range) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / ((position().z + m_range) - (position().z - m_range))));

	for (auto bolt : m_bolt)
	{
		bolt->setPosition(glm::vec3(x, 5.f, z));
		//bolt->activate(glm::vec3(x, 0.f, z), m_lightning_direction);
	}
}

engine::ref<wizard_hat> wizard_hat::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<wizard_hat>(props, enemies);
}
