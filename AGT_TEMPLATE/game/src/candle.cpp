// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

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
	m_ugr1_cost = 250.f;
	m_ugr2_cost = 600.f;
	m_ugl1_cost = 150.f;
	m_ugl2_cost = 350.f;
	init();
	init_range();
}

candle::~candle() {}

void candle::init()
{
	m_elapsed = 0.f;
	m_flame = billboard::create("assets/textures/fire1_64.png", 10, 6, 60);
	m_flame->activate(glm::vec3(position().x, position().y + 2.45f, position().z), .3f, .3f);
	m_fire_ring.initialise("assets/textures/fire_ring.png");

	m_pointLight.Color = glm::vec3(1.f, 0.757f, 0.459f);
	m_pointLight.AmbientIntensity = 0.25f;
	m_pointLight.DiffuseIntensity = 0.6f;
	m_pointLight.Position = glm::vec3(position().x, position().y + 2.5f, position().z);

	m_lightsource_material = engine::material::create(1.0f, m_pointLight.Color,
		m_pointLight.Color, m_pointLight.Color, 1.0f);

	engine::ref<engine::sphere> source_mesh = engine::sphere::create(10, 20, 0.01f);
	engine::game_object_properties source_props;
	source_props.meshes = { source_mesh->mesh() };
	source_props.position = m_pointLight.Position;
	m_flash_source = engine::game_object::create(source_props);
}

void candle::update(std::vector<engine::ref<enemy>> enemies, float dt)
{
	m_elapsed += dt;
	m_active_enemies = enemies;
	m_flame->on_update(dt, false, glm::vec3(0.f));
	m_pointLight.Position = glm::vec3(position().x, position().y + 2.5f, position().z);
	m_flash_source->set_position(m_pointLight.Position);

	if (m_elapsed >= m_attack_speed) {
		m_elapsed = 0.f;
		attack();
	}

	if (m_active_shot)
		m_shot->on_update(enemies, dt);

	m_fire_ring.on_update(dt);

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
			m_active_shot = true;
		}

		if (m_active_shot && m_shot->object()->position().y <= 0)
		{
			m_fire_ring.activate(m_shot->aoe_range(), glm::vec3(m_shot->object()->position().x, m_shot->object()->position().y + 0.19f, m_shot->object()->position().z));

			for (auto enemy : m_active_enemies)
				if (glm::length(enemy->position() - m_shot->object()->position()) <= 4.f)
					enemy->damage(m_shot->damage());

			m_shot->flag_remove();
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
	if (player.score() >= m_ugr1_cost)
	{
		m_attack_speed = 4.f;
		//subtract cost from score
		player.set_score(player.score() - m_ugr1_cost);
		m_right_level = 1;
	}
}

void candle::upgradeRight_lvl2(player& player)
{
	if (player.score() >= m_ugr2_cost)
	{
		m_attack_speed = 3.f;
		//subtract cost from score
		player.set_score(player.score() - m_ugr2_cost);
		m_right_level = 2;
	}
}

//flash duration
void candle::upgradeLeft_lvl1(player& player)
{
	if (player.score() >= m_ugl1_cost)
	{
		m_stun_duration = 2.5f;
		//subtract cost from score
		player.set_score(player.score() - m_ugl1_cost);
		m_left_level = 1;
	}
}

void candle::upgradeLeft_lvl2(player& player)
{
	if (player.score() >= m_ugl2_cost)
	{
		m_stun_duration = 3.5f;
		//subtract cost from score
		player.set_score(player.score() - m_ugl2_cost);
		m_left_level = 2;
	}
}

void candle::reset_shot()
{
	m_shot->~fire_ball();
	m_shot = nullptr;
}

void candle::light_render(engine::ref<engine::shader> shader, int num_point_lights)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gNumPointLights", num_point_lights);
	m_pointLight.submit(shader, num_point_lights - 1);
	m_lightsource_material->submit(shader);
	engine::renderer::submit(shader, m_flash_source->meshes().at(0), glm::translate(glm::mat4(1.f), m_pointLight.Position));
}

engine::ref<candle> candle::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<candle>(props, enemies);
}
