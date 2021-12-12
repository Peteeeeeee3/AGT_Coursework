// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

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
public:
	player() {}
	player(engine::perspective_camera& camera);
	~player() {}
	void on_update(const engine::timestep& time_step);
	void update_camera(engine::perspective_camera& camera, const engine::timestep& ts);
	void damage(float dmg) { m_health -= dmg; }
	float health() { return m_health; }
	float score() { return m_score; }
	void set_score(float ns) { m_score = ns; }
	void cam_reset() { m_reset_cam = true; }

	//void scroll_handling(const engine::timestep& time_step);

private:
	//movement speed
	float m_mSpeed = 8.f;
	//zoom speed
	float m_zSpeed = m_mSpeed * 3 / 4;
	//player health
	float m_health = 100;
	//player score
	float m_score = 1000;

	bool m_reset_cam = false;

	glm::vec3 m_position;
	glm::vec3 m_lookAt;
	glm::vec3 z_axis = glm::vec3(0.f, 0.f, 1.f); //stored for convenience

	float m_view_distance; // distance from player to look_at

	// defines playable and traversable area
	std::vector<float> m_camera_bounds{
		//min	//max
		-20.f,	20.f,	//x
		1.f,	20.f,	//y
		-20.f,	20.f	//z
	};

	void reset_camera(engine::perspective_camera& camera);
	void move(e_direction direction, engine::perspective_camera& camera, engine::timestep ts);
};
