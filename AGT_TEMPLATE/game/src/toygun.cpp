#include "toygun.h"

toygun::toygun(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : tower(props, enemies)
{
	m_damage = 20.f;
	m_attack_speed = 1.f;
	m_range = 10.f;
	init();
	init_range();
}

toygun::~toygun() {}

void toygun::init()
{
	m_elapsed = 0.f;
	engine::ref<engine::texture_2d> bullet_texture = engine::texture_2d::create("assets/textures/orange.png", true);
	engine::ref<engine::sphere> bullet_shape = engine::sphere::create(10, 20, .1f);
	m_bullet_props.position = position();
	m_bullet_props.position.y += 1.f;
	m_bullet_props.meshes = { bullet_shape->mesh() };
	m_bullet_props.textures = { bullet_texture };
	m_bullet_props.type = 1;
	m_bullet_props.bounding_shape = glm::vec3(.1f);
}

void toygun::update(std::vector<engine::ref<enemy>> enemies, float dt)
{
	m_elapsed += dt;

	m_active_enemies = enemies;

	m_bounding_box.on_update(glm::vec3(position().x, position().y + .5f, position().z));

	for (int i = 0; i < m_bullets.size(); ++i)
	{
		if (m_bullets.at(i).remove())
			m_bullets.erase(m_bullets.begin() + i);
		else 
			m_bullets.at(i).on_update(enemies, dt);
	}

	if (m_active_enemies.size() > 0)
	{
		m_target = find_target();

		if (m_target != nullptr)
		{
			glm::vec3 vec_to_enemy = m_target->position() - position();
			m_forward_vec = vec_to_enemy;
			float angle = atan2(vec_to_enemy.x, vec_to_enemy.z);
			set_rotation_amount(angle);

			if (m_elapsed >= m_attack_speed)
			{
				m_elapsed = 0.f;
				attack();
			}
		}
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
	m_bullets.push_back(bullet::bullet(m_bullet_props, m_range, m_forward_vec, glm::vec3(position().x, 1.f, position().z), m_damage));
}

engine::ref<enemy> toygun::find_target()
{
	for (auto enemy : m_active_enemies)
	{
		if (glm::length(enemy->position() - position()) <= m_range)
			return enemy;
	}
	return nullptr;
}

void toygun::render_bullets(engine::ref<engine::shader> shader)
{
	for (auto bullet : m_bullets)
	{
		engine::renderer::submit(shader, bullet.object());
	}
}

engine::ref<toygun> toygun::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<toygun>(props, enemies);
}
