#pragma once
#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"

class player
{
public:
	/**player(engine::perspective_camera& camera)
	{
		cam.reset(&camera);
	}**/
	player() {}
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
	std::shared_ptr<engine::perspective_camera> cam;
};
