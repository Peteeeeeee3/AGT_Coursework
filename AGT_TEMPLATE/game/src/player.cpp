#include "player.h"
#include <engine.h>
#include "engine/core/input.h"
#include <math.h>

player::player(engine::perspective_camera& camera) : m_camera(camera)
{
	m_position = m_camera.position();
	m_view_distance = m_position.y * (3 / 4);
	m_lookAt = m_position + glm::normalize(m_camera.front_vector()) * m_view_distance;
	m_lookAt.y = 0.f;
	glm::vec3 angle_vec = m_position - m_lookAt;
	m_current_angle = atan2(angle_vec.x, angle_vec.z);
	std::cout << "constructor";
}

void player::update_camera(engine::perspective_camera& camera, const engine::timestep& ts)
{
	m_position = m_camera.position();

	//update movement
	if (engine::input::key_pressed(engine::key_codes::KEY_A)) // left
		move(left, ts);
	else if (engine::input::key_pressed(engine::key_codes::KEY_D)) // right
		move(right, ts);

	if (engine::input::key_pressed(engine::key_codes::KEY_S)) // backward
		move(backward, ts);
	else if (engine::input::key_pressed(engine::key_codes::KEY_W)) // forward
		move(forward, ts);

	if (engine::input::key_pressed(engine::key_codes::KEY_LEFT_SHIFT)) // up
		move(up, ts);
	else if (engine::input::key_pressed(engine::key_codes::KEY_LEFT_CONTROL)) // down
		move(down, ts);

	m_view_distance = m_position.y * (3 / 4);
	m_lookAt = m_position + glm::normalize(m_camera.front_vector()) * m_view_distance;
	m_lookAt.y = 0.f;

	//update rotation
	if (engine::input::key_pressed(engine::key_codes::KEY_Q)) // left
		rotate(left, ts);
	else if (engine::input::key_pressed(engine::key_codes::KEY_E)) // right
		rotate(right, ts);

	m_camera.set_view_matrix(m_position, m_lookAt);
	std::cout << "update";
}

void player::move(e_direction direction, engine::timestep ts)
{
	if (direction == forward)
	{
		float y = m_camera.position().y;
		m_position += m_mSpeed * ts * m_camera.front_vector();
		m_position.y = y;
	}
	else if (direction == backward)
	{
		float y = m_camera.position().y;
		m_position -= m_mSpeed * ts * m_camera.front_vector();
		m_position.y = y;
	}

	if (direction == left)
	{
		float y = m_camera.position().y;
		m_position -= m_mSpeed * ts * m_camera.right_vector();
		m_position.y = y;
	}
	else if (direction == right)
	{
		float y = m_camera.position().y;
		m_position += m_mSpeed * ts * m_camera.right_vector();
		m_position.y = y;
	}

	if (direction == up)
	{
		m_position.y += m_mSpeed * ts;
		move(forward, ts);
	}
	else if (direction == down)
	{
		m_position.y -= m_mSpeed * ts;
		move(backward, ts);
	}
	std::cout << "move";
}

void player::rotate(e_direction direction, engine::timestep ts)
{
	float x;
	float z;
	if (direction == left)
	{
		x = m_view_distance * glm::sin(m_current_angle - m_rotation_speed);
		z = m_view_distance * glm::sin(m_current_angle - m_rotation_speed);
	}
	else if (direction == right)
	{
		x = m_view_distance * glm::sin(m_current_angle + m_rotation_speed);
		z = m_view_distance * glm::sin(m_current_angle + m_rotation_speed);
	}
	m_position = glm::vec3(x, m_position.y, z);
	std::cout << "rotate";
}
