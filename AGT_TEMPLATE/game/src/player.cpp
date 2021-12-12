// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include "player.h"
#include <engine.h>
#include "engine/core/input.h"
#include <math.h>

player::player(engine::perspective_camera& camera)
{
	reset_camera(camera);
}

void player::update_camera(engine::perspective_camera& camera, const engine::timestep& ts)
{
	//reset camera to original position and orientation
	if (m_reset_cam)
	{
		m_reset_cam = false;
		reset_camera(camera);
	}

	// update positional and angular attributes
	m_position = camera.position();
	m_view_distance = m_position.y * 3 / 4;
	m_lookAt = m_position + glm::normalize(camera.front_vector()) * m_view_distance;
	m_lookAt.y = 1.f;

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

	// assign values
	camera.set_view_matrix(m_position, m_lookAt);
	camera.on_update(ts);
}

void player::reset_camera(engine::perspective_camera& camera)
{
	m_position = glm::vec3(0.f, 2.f, 0.f); // store starting position
	m_view_distance = m_position.y * (3 / 4); // calculate distance
	m_lookAt = m_position + glm::vec3(.0f, 0.f, 1.f) * m_view_distance; // store initial look_at
	m_lookAt.y = 0.f;

	// assign initial values
	camera.reset_yaw();
	camera.reset_pitch();
	camera.set_view_matrix(m_position, m_lookAt);
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
