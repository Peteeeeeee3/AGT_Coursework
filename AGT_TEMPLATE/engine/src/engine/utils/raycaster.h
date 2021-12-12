// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#pragma once
#include <engine.h>
#include "engine/core/timestep.h"
#define GLM_FORCE_CTOR_INIT
#include "glm/glm.hpp"

namespace engine
{
	class raycaster {
	public:
		raycaster() {}
		raycaster(engine::perspective_camera& camera, float width, float height);
		~raycaster();
		glm::vec3& current_ray();
		glm::vec3 point_on_surface();
		void on_update(engine::perspective_camera& camera);

	private:
		glm::vec3	m_current_ray;
		glm::mat4	m_projection_matrix;
		glm::mat4	m_view_matrix;
		glm::vec3	m_camera_position;

		float		m_width;
		float		m_height;

		float		m_ray_range = 600.f;

		glm::vec3& calculate_mouse_ray();
		glm::vec3& to_world_coords(glm::vec4& eyeCoords);
		glm::vec4& to_eye_coords(glm::vec4& clipCoords);
		glm::vec3& normalised_device_coords(float& mouse_x, float& mouse_y, float& width, float& height);
		glm::vec3& point_on_ray(glm::vec3& ray, float distance, engine::perspective_camera& camera);
	};
}
