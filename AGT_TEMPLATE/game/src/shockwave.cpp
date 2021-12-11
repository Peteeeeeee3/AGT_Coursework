#include "pch.h"
#include "shockwave.h"


shockwave::shockwave()
{
	s_active = false;
	m_radius = .1f;
	m_start_radius = m_radius;
}

shockwave::~shockwave()
{}

void shockwave::on_update(const engine::timestep& timestep)
{
	if (s_active == false)
		return;

	// Life decreasing on each iteration
	m_life -= 1.f * (float)timestep;
	if (m_life < 0) {
		s_active = false;
		m_is_complete = true;
		return;
	}

	float f = pow(m_life, 0.25f); // Nonlinear mapping -- makes it expand faster at first
	m_radius += 7.5f * f * (float)timestep;
}

void shockwave::initialise(std::string path)
{
	// Load the texture
	m_texture = engine::texture_2d::create(path, false);

	glm::vec3 x_axis = glm::vec3(1.f, 0.f, 0.f);
	glm::vec3 z_axis = glm::vec3(0.f, 0.f, 1.f);

	// Assume oriented in xz plane
	glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f);

	// Determine the coordinates of the quad	   	 
	std::vector<engine::mesh::vertex> quad_vertices
	{
		//							 position												normal		 tex coord       
		{ glm::vec3(-m_start_radius * x_axis - m_start_radius * z_axis),		normal,		{ 0.f, 0.f } },
		{ glm::vec3(-m_start_radius * x_axis + m_start_radius * z_axis),		normal,		{ 0.f, 1.f } },
		{ glm::vec3(m_start_radius * x_axis + m_start_radius * z_axis),		normal,		{ 1.f, 1.f } },
		{ glm::vec3(m_start_radius * x_axis - m_start_radius * z_axis),		normal,		{ 1.f, 0.f } },
	};

	const std::vector<uint32_t> quad_indices
	{
		 0,  1,  2,
		 0,  2,  3,
	};

	m_quad = engine::mesh::create(quad_vertices, quad_indices);
}

void shockwave::activate(float radius, glm::vec3 position)
{
	m_position = position;
	m_life = .1f;
	m_radius = radius;
	s_active = true;

}

void shockwave::on_render(engine::ref<engine::shader> shader)
{
	if (s_active == false)
		return;

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position);
	transform = glm::scale(transform, glm::vec3(m_radius / m_start_radius));
	engine::renderer::submit(shader, m_quad, transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", true);

}
