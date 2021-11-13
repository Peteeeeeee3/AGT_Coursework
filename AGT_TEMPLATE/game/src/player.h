#pragma once
#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"

class player
{
public:
	enum e_direction
	{
		forward = 0,
		backward,
		left,
		right,
		up,
		down
	};
	/**player(engine::perspective_camera& camera)
	{
		cam.reset(&camera);
	}**/
	player() {}
	player(engine::perspective_camera& camera);
	~player() {}
	void on_update(const engine::timestep& time_step);
	void update_camera(engine::perspective_camera& camera, const engine::timestep& ts);
	//void scroll_handling(const engine::timestep& time_step);

private:
	//movement speed
	float m_mSpeed = 1.f;
	//zoom speed
	float m_zSpeed = 1.f;
	int health = 100;
	engine::perspective_camera m_camera;
	glm::vec3 m_position;
	glm::vec3 m_lookAt;
	float m_view_distance;
	float m_current_angle;
	float m_rotation_speed = glm::pi<float>() / 2;
	void move(e_direction direction, engine::timestep ts);
	void rotate(e_direction direction, engine::timestep ts);
};
