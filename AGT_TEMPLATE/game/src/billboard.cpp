#include "pch.h"
#include "billboard.h"

billboard::billboard(std::string sprite_filename, uint32_t frames_x, uint32_t frames_y, uint32_t num_frames)
{
	m_sprite = engine::texture_2d::create(sprite_filename, true);

	m_frames_x = frames_x;
	m_frames_y = frames_y;

	m_frame_width = m_sprite->width() / m_frames_x;
	m_frame_height = m_sprite->height() / m_frames_y;

	m_current_frame = 0;
	m_num_frames = num_frames;

	s_active = false;
}

billboard::~billboard()
{}

void billboard::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;

	// Update the frame based on dt
	float frameTime = 1.0f / 30.0f;  // Sprite running at 30 frames per second

	m_timer += (float)time_step;

	if (m_timer > frameTime) {
		m_current_frame++;
		if (m_current_frame >= m_num_frames) {
			s_active = false;
		}
		m_timer -= frameTime;
	}
}

void billboard::on_render(const engine::perspective_camera& camera, engine::ref<engine::shader> shader)
{
	if (!s_active)
		return;

	float image_width = (float)m_sprite->width();
	float image_height = (float)m_sprite->height();

	uint32_t x_index = m_current_frame % m_frames_x;
	uint32_t y_index = (m_frames_y - 1) - m_current_frame / m_frames_x;

	// Form a quad at p in the plane orthogonal to the lookAt vector
	glm::vec3 x_axis = glm::cross(camera.front_vector(), camera.up_vector());
	glm::normalize(x_axis);
	glm::vec3 y_axis = camera.up_vector();
	glm::normalize(y_axis);

	// Determine the coordinates of the billboarded quad	   	 
	std::vector<engine::mesh::vertex> quad_vertices
	{
		//                   position												normal				 tex coord       
		{ glm::vec3(m_position - m_width * x_axis - m_height * y_axis),		-camera.front_vector(),		{ x_index * (m_frame_width / image_width), y_index * (m_frame_height / image_height) } },
		{ glm::vec3(m_position + m_width * x_axis - m_height * y_axis),		-camera.front_vector(),		{ (x_index + 1) * (m_frame_width / image_width), y_index * (m_frame_height / image_height) } },
		{ glm::vec3(m_position + m_width * x_axis + m_height * y_axis),		-camera.front_vector(),		{ (x_index + 1) * (m_frame_width / image_width), (y_index + 1) * (m_frame_height / image_height) } },
		{ glm::vec3(m_position - m_width * x_axis + m_height * y_axis),		-camera.front_vector(),		{ x_index * (m_frame_width / image_width), (y_index + 1) * (m_frame_height / image_height) } },
	};

	const std::vector<uint32_t> quad_indices
	{
		 0,  1,  2,
		 0,  2,  3,
	};

	engine::ref<engine::mesh> mesh = engine::mesh::create(quad_vertices, quad_indices);


	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_sprite->bind();
	engine::renderer::submit(shader, mesh, glm::mat4(1.0f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", true);

}

void billboard::activate(glm::vec3 position, float width, float height)
{
	s_active = true;
	m_timer = 0.0f;
	m_current_frame = 0;
	m_position = position;
	m_width = width;
	m_height = height;
}

engine::ref<billboard> billboard::create(std::string sprite_filename, uint32_t frame_width, uint32_t frame_height, uint32_t num_frames)
{
	return std::make_shared<billboard>(sprite_filename, frame_width, frame_height, num_frames);
}
