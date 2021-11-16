#include "player.h"
#include <engine.h>
#include "engine/core/input.h"
#include <math.h>

player::player(engine::perspective_camera& camera)
{
	m_position = glm::vec3(0.f, 2.f, 10.f); // store starting position
	m_view_distance = m_position.y * (3 / 4); // calculate distance
	m_lookAt = m_position + glm::normalize(camera.front_vector()) * m_view_distance; // store initial look_at
	m_lookAt.y = 0.f;
	glm::vec3 angle_vec = m_position - m_lookAt; // calculate initial angle
	m_current_angle = glm::asin(angle_vec.x / angle_vec.z);

	// assign initial values
	camera.set_view_matrix(m_position, m_lookAt);
	camera.set_rotation_speed(m_rotation_speed);
}

void player::update_camera(engine::perspective_camera& camera, const engine::timestep& ts)
{
	// update positional and angular attributes
	m_position = camera.position();
	m_view_distance = m_position.y * 3 / 4;
	m_lookAt = m_position + glm::normalize(camera.front_vector()) * m_view_distance;
	m_lookAt.y = 1.f;
	glm::vec3 angle_vec = m_position - z_axis;
	m_current_angle = glm::asin(angle_vec.x / angle_vec.z);

	//update movement
	if (engine::input::key_pressed(engine::key_codes::KEY_A)) // left
		move(left, camera, ts);
	else if (engine::input::key_pressed(engine::key_codes::KEY_D)) // right
		move(right, camera, ts);

	if (engine::input::key_pressed(engine::key_codes::KEY_S)) // backward
		move(backward, camera, ts);
	else if (engine::input::key_pressed(engine::key_codes::KEY_W)) // forward
		move(forward, camera, ts);

	if (engine::input::key_pressed(engine::key_codes::KEY_LEFT_SHIFT)) // up
		move(up, camera, ts);
	else if (engine::input::key_pressed(engine::key_codes::KEY_LEFT_CONTROL)) // down
		move(down, camera, ts);

	//update rotation
	if (engine::input::key_pressed(engine::key_codes::KEY_Q)) // left
		rotate(left, camera, ts);
	else if (engine::input::key_pressed(engine::key_codes::KEY_E)) // right
		rotate(right, camera, ts);

	// assign values
	camera.set_view_matrix(m_position, m_lookAt);
	camera.on_update(ts);
}

void player::move(e_direction direction, engine::perspective_camera& camera, engine::timestep ts)
{
	// store in case new values are out of bounds
	glm::vec3 temp_pos = m_position;
	glm::vec3 temp_lookAt = m_lookAt;
	if (direction == forward)
	{
		float y = camera.position().y;
		temp_pos += m_mSpeed * ts * camera.front_vector();
		temp_lookAt += m_mSpeed * ts * camera.front_vector();
		temp_pos.y = y;
		temp_lookAt.y = 1.f;
	}
	else if (direction == backward)
	{
		float y = camera.position().y;
		temp_pos -= m_mSpeed * ts * camera.front_vector();
		temp_lookAt -= m_mSpeed * ts * camera.front_vector();
		temp_pos.y = y;
		temp_lookAt.y = 1.f;
	}

	if (direction == left)
	{
		float y = camera.position().y;
		temp_pos -= m_mSpeed * ts * camera.right_vector();
		temp_lookAt -= m_mSpeed * ts * camera.right_vector();
		temp_pos.y = y;
		temp_lookAt.y = 1.f;
	}
	else if (direction == right)
	{
		float y = camera.position().y;
		temp_pos += m_mSpeed * ts * camera.right_vector();
		temp_lookAt += m_mSpeed * ts * camera.right_vector();
		temp_pos.y = y;
		temp_lookAt.y = 1.f;
	}

	// check if new position is out of bounds horizontally
	if (temp_pos.x >= m_camera_bounds[0] && temp_pos.x <= m_camera_bounds[1] &&
		temp_pos.z >= m_camera_bounds[4] && temp_pos.z <= m_camera_bounds[5])
	{
		m_position = temp_pos;
		m_lookAt = temp_lookAt;
	}

	if (direction == up && m_position.y <= m_camera_bounds[3])
	{
		//only move horizontally if within bounds, else only vertically
		if (m_position.x >= m_camera_bounds[0] && m_position.x <= m_camera_bounds[1] && m_position.z >= m_camera_bounds[4]
			&& m_position.z <= m_camera_bounds[5])
			m_position += m_zSpeed / 2 * ts * (camera.up_vector() - camera.front_vector());
		else
			m_position += m_zSpeed / 2 * ts * camera.up_vector();
	}
	else if (direction == down && m_position.y >= m_camera_bounds[2])
	{
		if (m_position.x >= m_camera_bounds[0] && m_position.x <= m_camera_bounds[1] && m_position.z >= m_camera_bounds[4]
			&& m_position.z <= m_camera_bounds[5])
			m_position -= m_zSpeed / 2 * ts * (camera.up_vector() - camera.front_vector());
		else
			m_position -= m_zSpeed / 2 * ts * camera.up_vector();
	}
}

void player::rotate(e_direction direction, engine::perspective_camera& camera, engine::timestep ts)
{
 	float x;
	float z;

	// calculate new angle
	if (direction == right)
	{
		m_current_angle += m_rotation_speed * ts;
	}
	else if (direction == left)
	{
		m_current_angle -= m_rotation_speed * ts;
	}

	// keep angle within range
	if (m_current_angle < -360)
		m_current_angle += 360;

	if (m_current_angle > 360)
		m_current_angle -= 360;

	// use the same formula as for cone to get camera position
	x = m_view_distance * glm::sin(m_current_angle);
	z = m_view_distance * glm::cos(m_current_angle);
	
	// apply new position
	m_position = glm::vec3(x, m_position.y, z);
	std::cout << m_position << "\n";
	/**
	if (direction == right)
	{
		camera.rotate(engine::perspective_camera::e_rotation::anticlock_wise, engine::perspective_camera::e_axis::y, ts);
	}
	else if (direction == left)
	{
		camera.rotate(engine::perspective_camera::e_rotation::clock_wise, engine::perspective_camera::e_axis::y, ts);
	}**/
}
