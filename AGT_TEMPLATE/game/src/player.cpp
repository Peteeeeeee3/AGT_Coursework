#include "player.h"
#include <engine.h>
#include "engine/core/input.h"

void player::update_camera(engine::perspective_camera& camera, const engine::timestep& ts)
{
	/**glm::vec3 new_pos = camera.position();
	float y = camera.position().y;
	glm::vec3 new_look_at;
	//move left
	if (engine::input::key_pressed(engine::key_codes::KEY_A));
	{
		new_pos -= m_mSpeed * ts * camera.right_vector();
		//reset height to original
		new_pos.y = y;
	}
	//move right
	if (engine::input::key_pressed(engine::key_codes::KEY_A));
	{
		new_pos -= m_mSpeed * ts * camera.right_vector();
		//reset height to original
		new_pos.y = y;
	}
	//move forward
	if (engine::input::key_pressed(engine::key_codes::KEY_A));
	{
		new_pos -= m_mSpeed * ts * camera.front_vector();
		//reset height to original
		new_pos.y = y;
	}
	//move backward
	if (engine::input::key_pressed(engine::key_codes::KEY_A));
	{
		new_pos -= m_mSpeed * ts * camera.front_vector();
		//reset height to original
		new_pos.y = y;
	}**/
	//camera.set_view_matrix(new_pos, new_look_at);
	//camera.set_view_matrix(camera.position(), glm::vec3(camera.front_vector().x, 0.f, camera.front_vector().z));
	//glm::vec3 look_at = camera.position() + glm::normalize(camera.front_vector()) * 7.f;
	//look_at.y = 0.f;

	//camera.set_view_matrix(camera.position(), look_at);

	//if(engine::input::)
}
