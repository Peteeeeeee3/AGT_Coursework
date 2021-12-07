#include "toygun.h"

toygun::toygun(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : tower(props, enemies)
{
	m_damage = 10.f;
	m_attack_speed = 10.f;
	m_range = 10.f;
	init();
}

toygun::~toygun() {}

void toygun::init()
{
	m_elapsed = 0.f;
	engine::ref<engine::sphere> bullet_shape = engine::sphere::create(10, 20, 0.1f);
	m_bullet_props.position = position();
	m_bullet_props.meshes = { bullet_shape->mesh() };
	m_bullet_props.type = 1;
	m_bullet_props.bounding_shape = glm::vec3(0.1f);
}

void toygun::update(std::vector<engine::ref<enemy>> enemies, float dt)
{
	m_elapsed += dt;

	m_active_enemies = enemies;

	if (m_active_enemies.size() > 0)
	{
		m_target = find_target();

		glm::vec3 vec_to_enemy = m_target->position() - position();
		float angle = atan2(vec_to_enemy.x, vec_to_enemy.z);
		set_rotation_amount(angle);
	}

	if (m_elapsed >= m_attack_speed)
	{
		m_elapsed = 0.f;
		attack();
	}
}

void toygun::rotate(bool rotate_left, float dt)
{
	//check for correct movement
	if (position().y >= 3.5f)
	{
		movingUp = false;
	}
	else if (position().y <= 2.5f)
	{
		movingUp = true;
	}

	//adjust position
	if (movingUp)
	{
		set_position(position() + glm::vec3(0.f, m_mSpeed * dt, 0.f));
	}
	else
	{
		set_position(position() - glm::vec3(0.f, m_mSpeed * dt, 0.f));
	}

	//rotate
	if (rotate_left)
	{
		set_rotation_amount(rotation_amount() + dt * 1.5f);
	}
	else
	{
		set_rotation_amount(rotation_amount() - dt * 1.5f);
	}
}

void toygun::attack()
{
	m_bullets.push_back(engine::game_object::create(m_bullet_props));
}

engine::ref<enemy> toygun::find_target()
{
	for (auto enemy : m_active_enemies)
	{
		if (glm::length(enemy->position() - position()) <= m_range)
			return enemy;
	}
}

engine::ref<toygun> toygun::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<toygun>(props, enemies);
}
