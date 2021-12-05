#include "candle.h"

candle::candle(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : tower(props, enemies)
{
	glm::vec3 position = props.position;
	m_flash_frequency = 3.5f;
	m_stun_duration = 1.f;
	m_damage = 10.f;
	m_attack_speed = 1.f;
	m_range = 10.f;
	init();
	init_range();
}

candle::~candle() {}

void candle::init()
{
	m_elapsed = 0.f;
	m_flame = billboard::create("assets/textures/fire1_64.png", 64, 64, 60);
	m_flame->activate(glm::vec3(position().x, position().y + 4.5f, position().z), 30.f, 30.f);
}

void candle::update(float dt)
{
	m_elapsed += dt;

	if (m_elapsed >= 5) {
		m_elapsed = 0.f;
		attack();
	}

	//if (!m_flame->isActive())
	//{
		//m_flame->activate(glm::vec3(position().x, position().y + 4.5f, position().z), 3.f, 3.f);
	//}
}

void candle::attack()
{
	for (auto enemy : m_active_enemies)
	{
		if ((position() - enemy->position()).length() <= m_range)
			enemy->stun(m_stun_duration);
	}
}

engine::ref<candle> candle::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<candle>(props, enemies);
}
