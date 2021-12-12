// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include "toygun.h"

toygun::toygun(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : tower(props, enemies)
{
	m_damage = 20.f;
	m_attack_speed = 1.f;
	m_range = 8.f;
	m_ugr1_cost = 250.f;
	m_ugr2_cost = 750.f;
	m_ugl1_cost = 300.f;
	m_ugl2_cost = 1000.f;
	init();
	init_range();
}

toygun::~toygun() {}

void toygun::init()
{
	//initialise bullets that will be shot
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
	m_play_shot_sound = false;
	m_active_enemies = enemies;

	m_bounding_box.on_update(glm::vec3(position().x, position().y + .5f, position().z));

	//update bullets unless they are flagged for removal
	for (int i = 0; i < m_bullets.size(); ++i)
	{
		if (m_bullets.at(i).remove())
			//remove flagged bullet
			m_bullets.erase(m_bullets.begin() + i);
		else 
			m_bullets.at(i).on_update(enemies, dt);
	}

	//be inactive unless its an active wave
	if (m_active_enemies.size() > 0)
	{
		m_target = find_target();

		//only act if target is available
		if (m_target != nullptr)
		{
			//rotate to face target
			glm::vec3 vec_to_enemy = m_target->position() - position();
			m_forward_vec = vec_to_enemy;
			float angle = atan2(vec_to_enemy.x, vec_to_enemy.z);
			set_rotation_amount(angle);

			//shoot if applicable
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
	m_play_shot_sound = true;
}

engine::ref<enemy> toygun::find_target()
{
	if (target_index() == -1)
		return nullptr;
	else
		return m_active_enemies.at(target_index());
}

void toygun::render_bullets(engine::ref<engine::shader> shader)
{
	for (auto bullet : m_bullets)
	{
		engine::renderer::submit(shader, bullet.object());
	}
}

//range
void toygun::upgradeRight_lvl1(player& player)
{
	if (player.score() >= m_ugr1_cost)
	{
		m_range = 12.f;
		init_range();
		//subtract cost from score
		player.set_score(player.score() - m_ugr1_cost);
		m_right_level = 1;
	}
}

//range
void toygun::upgradeRight_lvl2(player& player)
{
	if (player.score() >= m_ugr2_cost)
	{
		m_range = 20.f;
		init_range();
		//subtract cost from score
		player.set_score(player.score() - m_ugr2_cost);
		m_right_level = 2;
	}
}

//fire rate
void toygun::upgradeLeft_lvl1(player& player)
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
void toygun::upgradeLeft_lvl2(player& player)
{
	if (player.score() >= m_ugl2_cost)
	{
		m_attack_speed = 0.25f;
		//subtract cost from score
		player.set_score(player.score() - m_ugl2_cost);
		m_left_level = 2;
	}
}

int toygun::target_index()
{
	//return out of bounds if none found
	int return_index = -1;
	engine::ref<enemy> current_target = nullptr;
	//find a random enemy in range
	for (auto enemy : m_active_enemies)
		if (glm::length(enemy->position() - position()) <= m_range)
		{
			current_target = enemy;
			break;
		}

	//break if non ein reange
	if (current_target == nullptr)
		return return_index;

	if (m_active_enemies.size() != 0)
	{
		current_target = m_active_enemies.at(0);
		return_index = 0;
		for (int i = 0; i < m_active_enemies.size(); ++i)
		{
			//if enemy is in range and has travelled further than current target, update target
			if (m_active_enemies.at(i)->distance_covered() > current_target->distance_covered() && glm::length(m_active_enemies.at(i)->position() - position()) <= m_range)
			{
				current_target = m_active_enemies.at(i);
				return_index = i;
			}
		}
	}
	return return_index;
}

engine::ref<toygun> toygun::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<toygun>(props, enemies);
}
