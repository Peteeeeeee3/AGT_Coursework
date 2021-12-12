// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include "raycaster.h"
#include "pch.h"
#include <engine.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

engine::raycaster::raycaster(engine::perspective_camera& camera, float x, float y)
	:m_width(x), m_height(y)
{
	m_projection_matrix = camera.projection_matrix();
	m_view_matrix = camera.view_matrix();
	m_camera_position = camera.position();
}

engine::raycaster::~raycaster() {}

glm::vec3& engine::raycaster::current_ray() {
	return m_current_ray;
}

void engine::raycaster::on_update(engine::perspective_camera& camera) {
	m_camera_position = camera.position();
	m_view_matrix = camera.view_matrix();
	m_projection_matrix = camera.projection_matrix();
	m_current_ray = calculate_mouse_ray();
}

/// <summary>
/// calculates the ray coming from the mouse going into the screen (in world coordinates)
/// </summary>
glm::vec3& engine::raycaster::calculate_mouse_ray() {
	auto [mouse_x, mouse_y] = engine::input::mouse_position();//screen coords
	glm::vec3 normalizedCoords = normalised_device_coords(mouse_x, mouse_y, m_width, m_height);//normalized coords
	glm::vec4 clipCoords = glm::vec4(normalizedCoords.x, normalizedCoords.y, -1.f, 1.f);
	glm::vec4 eyeCoords = to_eye_coords(clipCoords);
	glm::vec3 worldRay = to_world_coords(eyeCoords); // world coords
	return worldRay;
}

glm::vec3& engine::raycaster::to_world_coords(glm::vec4& eyeCoords) {
	glm::mat4 invertedView = inverse(m_view_matrix);
	glm::vec4 rayWorld = invertedView * eyeCoords;
	glm::vec3 mouseRay = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);
	return normalize(mouseRay);
}

glm::vec4& engine::raycaster::to_eye_coords(glm::vec4& clipCoords) {
	glm::mat4 invertedProjection = inverse(m_projection_matrix);
	glm::vec4 eyeCoords = invertedProjection * clipCoords;
	return glm::vec4(eyeCoords.x, eyeCoords.y, -1.f, 0.f);
}

glm::vec3& engine::raycaster::normalised_device_coords(float& mouse_x, float& mouse_y, float& width, float& height) {
	//float x = (2.f * mouse_x) / width - 1.f;
	//float y = 1.f - (2.f * mouse_y) / height;
	//return glm::vec2(x,y);

	float x = (2.0f * mouse_x) / width - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / height;
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	return ray_nds;
}

glm::vec3& engine::raycaster::point_on_ray(glm::vec3& ray, float distance, engine::perspective_camera& camera) {
	glm::vec3 camPos = camera.position();
	glm::vec3 start = glm::vec3(camPos.x, camPos.y, camPos.z);
	glm::vec3 scaledRay = glm::vec3(ray.x * distance, ray.y * distance, ray.z * distance);
	return start + scaledRay;
}

glm::vec3 engine::raycaster::point_on_surface()
{
	glm::vec3 start = m_camera_position;
	glm::vec3 end = m_camera_position + m_current_ray * m_ray_range;
	glm::vec3 point = start;

	//loop until point has a suffieciently small inacuracy
	while (!(point.y <= 0.1f && point.y >= -0.1f))
	{
		//point is in the middle of start and finish
		point = start + m_current_ray * (glm::length(end - start) / 2.f);

		if (point.y < -0.1f)
			end = point;
		else if (point.y > 0.1f)
			start = point;
		else
			break;
	}
	return glm::vec3(point.x, 0.f, point.z);
}
