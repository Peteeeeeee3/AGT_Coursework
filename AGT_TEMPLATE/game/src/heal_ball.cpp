#include "heal_ball.h"

heal_ball::heal_ball(/*std::vector<engine::ref<enemy>> enemies*/)/* : m_active_enemies(enemies)*/
{
	m_position = glm::vec3(0.f, 0.f, 0.f);
	m_range = 2.f;
	m_healing_strength = 10.f;
	init();
}

void heal_ball::init()
{
	//engine::ref<engine::texture_2d> bullet_texture = engine::texture_2d::create("assets/textures/orange.png", true);
	engine::ref<engine::sphere> bullet_shape = engine::sphere::create(10, 20, .5f);
	engine::game_object_properties hball_props;
	hball_props.position = m_position;
	hball_props.position.y += 2.f;
	hball_props.meshes = { bullet_shape->mesh() };
	//hball_props.textures = { bullet_texture };
	hball_props.mass = 2.f;
	hball_props.type = 1;
	hball_props.bounding_shape = glm::vec3(.5f);
	m_ball = engine::game_object::create(hball_props);

	m_rotational_inertia = (2.f / 3.f) * m_ball->mass() * (m_ball->bounding_shape().y / 2.f) * (m_ball->bounding_shape().y / 2.f);
	m_contact_time = 0.05f;
}

void heal_ball::launch(glm::vec3 position, glm::vec3 direction)
{
	glm::vec3 n_direction = glm::normalize(direction);
	m_ball->set_velocity(glm::vec3(0.f));
	m_ball->set_acceleration(glm::vec3(0.f, -9.8f, 0.f));
	m_ball->set_torque(glm::vec3(0.f));
	m_ball->set_rotation_amount(0.f);
	m_ball->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_ball->set_angular_velocity(glm::vec3(0.f));
	m_contact_time = 0.0f;

	float force_factor = 40.f;
	glm::vec3 force = n_direction * force_factor;
	m_instantaneous_acceleration = force / m_ball->mass();

	glm::vec3 torque = glm::vec3(1.f, 0.f, 0.f);
	torque *= force_factor / 90.f;
	m_instantaneous_angular_acceleration = torque / m_rotational_inertia;

	m_theta = engine::PI / 2.f - acos(n_direction.y);
	m_phi = atan2(n_direction.x, n_direction.z);

	m_is_active = true;
}

void heal_ball::on_update(/*std::vector<engine::ref<enemy>> enemies, */float dt)
{
	//m_active_enemies = enemies;

	m_ball->set_velocity(m_ball->velocity() + (m_ball->acceleration() + m_instantaneous_acceleration) * dt);
	m_ball->set_position(m_ball->position() + m_ball->velocity() * dt);

	glm::vec3 angle = m_ball->rotation_axis() * m_ball->rotation_amount();
	angle += m_ball->angular_velocity() * dt;
	m_ball->set_rotation_amount(glm::length(angle));
	if (glm::length(angle) > 0.f)
	{
		m_ball->set_rotation_axis(glm::normalize(angle));
	}

	m_ball->set_angular_velocity(m_ball->angular_velocity() + (m_ball->torque() + m_instantaneous_angular_acceleration) * dt);

	// Turn off instantaneous forces if contact time is surpassed
	if (glm::length(m_instantaneous_acceleration) > 0 && m_contact_time > 0.05) {
		m_instantaneous_acceleration = glm::vec3(0.f);
		m_instantaneous_angular_acceleration = glm::vec3(0.f);
		m_contact_time = 0.f;
	}
	m_contact_time += dt;

	if (m_position.y <= 0.f)
	{
		heal();
		m_to_remove = true;
	}
}

void heal_ball::heal()
{
	/*for (auto enemy : m_active_enemies)
	{
		if (glm::length(enemy->position() - m_position) <= m_range)
		{
			enemy->heal(m_healing_strength);
		}
	}*/
}
