#include "candle.h"
#include "pch.h"
#include "engine/core/input.h"

candle::candle(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : tower(props, enemies)
{
	glm::vec3 position = props.position;
	m_flash_frequency = 3.5f;
	m_stun_duration = 2.f;
	m_damage = 10.f;
	m_attack_speed = 5.f;
	m_turret_Speed = 1.f;
	m_range = 5.f;
	init();
	init_range();
}

candle::~candle() {}

void candle::init()
{
	m_elapsed = 0.f;
	m_flame = billboard::create("assets/textures/fire1_64.png", 10, 6, 60);
	m_flame->activate(glm::vec3(position().x, position().y + 2.45f, position().z), .3f, .3f);
}

void candle::update(std::vector<engine::ref<enemy>> enemies, float dt)
{
	m_elapsed += dt;
	m_active_enemies = enemies;
	m_flame->on_update(dt);

	if (m_active_shot)
		m_shot->on_update(enemies, dt);

	if (m_elapsed >= m_attack_speed) {
		m_elapsed = 0.f;
		attack();
	}

	if (!m_flame->isActive())
	{
		m_flame->activate(glm::vec3(position().x, position().y + 2.45f, position().z), .3f, .3f);
	}
}

void candle::update_shot(engine::perspective_camera& camera, float dt)
{
	if (m_active_cam)
	{
		if (!m_active_shot && engine::input::mouse_button_pressed(0))
		{
			m_shot = std::make_shared<fire_ball>();
			m_shot->shoot(camera);
		}

		if (m_shot->object()->position().y <= 0)
		{
			m_shot->to_remove();
			m_active_shot = false;
		}
	}
}

void candle::turret_camera(engine::perspective_camera& camera, float dt)
{
	auto [mouse_delta_x, mouse_delta_y] = engine::input::mouse_position();
	camera.process_mouse(mouse_delta_x, mouse_delta_y);
	glm::vec3 cam_pos = glm::vec3(position().x, position().y + 2.5f, position().z);
	camera.set_view_matrix(cam_pos, cam_pos + glm::normalize(camera.front_vector()));
	camera.on_update(dt);
}

void candle::attack()
{
	for (auto enemy : m_active_enemies)
	{
		if (glm::length(position() - enemy->position()) <= m_range && !enemy->isStunnded())
			enemy->stun(m_stun_duration);
	}
}

//flash frequency
void candle::upgradeRight_lvl1(player& player)
{
	int cost = 250;
	if (player.score() >= cost)
	{
		m_attack_speed = 4.f;
		//subtract cost from score
		player.set_score(player.score() - cost);
	}
}

void candle::upgradeRight_lvl2(player& player)
{
	int cost = 600;
	if (player.score() >= cost)
	{
		m_attack_speed = 3.f;
		//subtract cost from score
		player.set_score(player.score() - cost);
	}
}

//flash duration
void candle::upgradeLeft_lvl1(player& player)
{
	int cost = 150;
	if (player.score() >= cost)
	{
		m_stun_duration = 2.5f;
		//subtract cost from score
		player.set_score(player.score() - cost);
	}
}

void candle::upgradeLeft_lvl2(player& player)
{
	int cost = 350;
	if (player.score() >= cost)
	{
		m_stun_duration = 3.5f;
		//subtract cost from score
		player.set_score(player.score() - cost);
	}
}

void candle::reset_shot()
{
	m_shot->~fire_ball();
	m_shot = nullptr;
}

engine::ref<candle> candle::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<candle>(props, enemies);
}
