#include "candle.h"

candle::candle(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : tower(props, enemies)
{
	glm::vec3 position = props.position;
	m_flash_frequency = 3.5f;
	m_stun_duration = 1.f;
	m_damage = 10.f;
	m_attack_speed = 1.f;
	m_range = 10.f;
}

candle::~candle() {}

void candle::init()
{
	m_attack_timer.start();
}

void candle::update()
{
	attack();
}

void candle::attack()
{
	if (m_attack_timer.elapsed() >= m_attack_speed)
	{
		m_attack_timer.start();
		for (auto enemy : m_active_enemies)
		{
			enemy->stun(m_stun_duration);
		}
	}
}

engine::ref<candle> candle::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<candle>(props, enemies);
}
