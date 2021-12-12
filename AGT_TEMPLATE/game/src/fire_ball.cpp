// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include "fire_ball.h"

fire_ball::fire_ball()
{
	m_position = glm::vec3(0.f);
	m_range = 3.f;
	m_flame_damage = 10.f;
	init();
}

fire_ball::~fire_ball() {}

void fire_ball::init()
{
	engine::ref<engine::texture_2d> ball_texture = engine::texture_2d::create("assets/textures/orange.png", true);
	engine::ref<engine::sphere> ball_shape = engine::sphere::create(10, 20, .2f);
	engine::game_object_properties ball_props;
	ball_props.position = m_position;
	ball_props.meshes = { ball_shape->mesh() };
	ball_props.textures = { ball_texture };
	ball_props.mass = 2.f;
	ball_props.type = 1;
	ball_props.bounding_shape = glm::vec3(.5f);
	m_object = engine::game_object::create(ball_props);

	m_flame = billboard::create("assets/textures/fire1_64.png", 10, 6, 60);
	m_flame->activate(glm::vec3(m_object->position().x, m_object->position().y + 2.45f, m_object->position().z), .3f, .3f);
}

void fire_ball::shoot(const engine::perspective_camera& camera)
{
	//prepare for shot and apply force(s) correctly
	m_object->set_velocity(glm::vec3(0.f));
	m_object->set_acceleration(glm::vec3(0.f, -9.8f, 0.f));

	m_contact_time = 0.0f;

	m_object->set_position(camera.position());

	glm::vec3 force = camera.front_vector() * 100.f;
	m_instantaneous_acceleration = force / m_object->mass();
}

void fire_ball::on_update(std::vector<engine::ref<enemy>> enemies, float dt)
{
	//update velocity and position
	m_object->set_velocity(m_object->velocity() + (m_object->acceleration() + m_instantaneous_acceleration) * dt);
	m_object->set_position(m_object->position() + m_object->velocity() * dt);

	m_flame->on_update(dt, true, glm::vec3(m_object->position().x, m_object->position().y + 0.3f, m_object->position().z));

	// Turn off instantaneous forces if contact time is surpassed
	if (glm::length(m_instantaneous_acceleration) > 0 && m_contact_time > 0.05) {
		m_instantaneous_acceleration = glm::vec3(0.f);
		m_contact_time = 0.f;
	}

	//loop flame effect
	if (!m_flame->isActive())
	{
		m_flame->activate(glm::vec3(m_object->position().x, m_object->position().y + .3f, m_object->position().z), .3f, .3f);
	}

	m_contact_time += dt;
}

void fire_ball::on_render(const engine::ref<engine::shader>& shader)
{
	engine::renderer::submit(shader, m_object);
}
