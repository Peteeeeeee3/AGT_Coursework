// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include "wizard_hat.h"

wizard_hat::wizard_hat(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : tower(props, enemies)
{
	m_damage = 10.f;
	m_attack_speed = 1.f;
	m_range = 3.f;
	m_ugr1_cost = 150.f;
	m_ugr2_cost = 350.f;
	m_ugl1_cost = 350.f;
	m_ugl2_cost = 1200.f;
	init();
	init_range();
}

wizard_hat::~wizard_hat() {}

void wizard_hat::init()
{
	m_elapsed = 0.f;
	m_lightning.initialise("assets/textures/lighting.png");
}

void wizard_hat::update(std::vector<engine::ref<enemy>> enemies, float dt)
{
	m_elapsed += dt;
	m_play_spark_sound = false;
	m_active_enemies = enemies;

	m_lightning.on_update(dt);

	//attack if applicable
	if (m_elapsed >= m_attack_speed)
	{
		m_elapsed = 0.f;
		if (m_active_enemies.size() > 0)
			attack();
	}
}

void wizard_hat::attack()
{
	//activate effect
	m_lightning.activate(m_range, glm::vec3(position().x, position().y + 0.18f, position().z));

	//damage applicable enemies
	for (auto enemy : m_active_enemies)
	{
		if (glm::length(enemy->position() - position()) <= m_range)
		{
			enemy->damage(m_damage);
		}
	}

	//play zap sound
	m_play_spark_sound = true;
}

//range
void wizard_hat::upgradeRight_lvl1(player& player)
{
	if (player.score() >= m_ugr1_cost)
	{
		m_range = 4.f;
		init_range();
		//subtract cost from score
		player.set_score(player.score() - m_ugr1_cost);
		m_right_level = 1;
	}
}

//range
void wizard_hat::upgradeRight_lvl2(player& player)
{
	if (player.score() >= m_ugr2_cost)
	{
		m_range = 7.f;
		init_range();
		//subtract cost from score
		player.set_score(player.score() - m_ugr2_cost);
		m_right_level = 2;
	}
}

//fire rate
void wizard_hat::upgradeLeft_lvl1(player& player)
{
	if (player.score() >= m_ugl1_cost)
	{
		m_attack_speed = 0.8f;
		//subtract cost from score
		player.set_score(player.score() - m_ugl1_cost);
		m_left_level = 1;
	}
}

//fire rate
void wizard_hat::upgradeLeft_lvl2(player& player)
{
	if (player.score() >= m_ugl2_cost)
	{
		m_attack_speed = 0.25f;
		//subtract cost from score
		player.set_score(player.score() - m_ugl2_cost);
		m_left_level = 2;
	}
}

engine::ref<wizard_hat> wizard_hat::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<wizard_hat>(props, enemies);
}
