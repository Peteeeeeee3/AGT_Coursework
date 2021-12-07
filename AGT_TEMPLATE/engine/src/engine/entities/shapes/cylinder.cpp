#include "cylinder.h"
#include "pch.h"
#include <engine.h>

engine::cylinder::cylinder(uint32_t sections, float radius, float height, glm::vec3 center) : m_sections(sections), m_radius(radius), m_height(height), m_center(center)
{
	//define peak
	m_peak = m_center;
	m_peak.y += m_height;

	std::vector<engine::mesh::vertex> cyl_vertices;
	std::vector<uint32_t> cyl_indices;

	//define the angle of the individual triangles
	float step_size = 2 * glm::pi<float>() * m_radius / m_sections;

	//triangles are drawn with two points along a circle and the third being the center or peak
	//whilst rectangles for the sides are drawn between the left and right points along both ceiling and base

	//ceiling
	glm::vec3 c_left = glm::vec3(m_peak.x + m_radius, m_peak.y, m_peak.z); // left point along circle
	glm::vec3 c_right; // right point along peak
	glm::vec3 c_normal; // normal of triangle

	//walls
	glm::vec3 w_normal;

	//base
	glm::vec3 b_left = glm::vec3(m_center.x + m_radius, m_center.y, m_center.z); // left point along circle
	glm::vec3 b_right; // right point along peak
	glm::vec3 b_normal; // normal of triangle

	//point along top circle
	float x;
	float c_y = m_peak.y;
	float b_y = m_center.y; //point along bottom circle only differs in the y-coordinate
	float z;

	//track correct index
	int index = 0;

	for (int i = 0; i < m_sections + 1; ++i)
	{
		// calculate x and y values of right point
		x = m_radius * glm::sin(step_size * i);
		z = m_radius * glm::cos(step_size * i);
		//assign coordinates
		c_right = glm::vec3(x, c_y, z);
		b_right = glm::vec3(x, b_y, z);
		std::cout << "ceiling right: " << c_right << " peak: " << m_peak << "\n";
		//std::cout << "c: " << c_right << " b: " << b_right << "\n";
		//ceiling normal
		c_normal = glm::cross(m_peak - c_left, m_peak - c_right);
		//ceiling vertices
		cyl_vertices.push_back({ c_left, c_normal, {0.f, 0.f} });
		cyl_vertices.push_back({ c_right, c_normal, {1.f, 0.f} });
		cyl_vertices.push_back({ m_peak, c_normal, {0.5f, 1.f} });
		for (int j = 0; j < 3; ++j)
		{
			cyl_indices.push_back(index);
			++index;
		}

		//wall normal
		w_normal = glm::cross(b_left - c_left, b_left - b_right);
		//wall vertices - handle as two triangles
		cyl_vertices.push_back({ b_left, w_normal, {0.f, 0.f} });
		cyl_indices.push_back(index);
		uint32_t bl_index = index;
		++index;
		cyl_vertices.push_back({ b_right, w_normal, {1.f, 0.f} });
		cyl_indices.push_back(index);
		++index;
		cyl_vertices.push_back({ c_right, w_normal, {1.f, 1.f} });
		cyl_indices.push_back(index);
		uint32_t cr_index = index;
		++index;
		cyl_vertices.push_back({ c_left, w_normal, { 0.f, 1.f } });
		cyl_indices.push_back(bl_index);
		cyl_indices.push_back(cr_index);
		cyl_indices.push_back(index);
		++index;
		
		//base normal
		b_normal = glm::cross(m_center - b_left, m_center - b_right);
		//base vertices
		cyl_vertices.push_back({ m_center, b_normal, {0.5f, 1.f} });
		cyl_vertices.push_back({ b_right, b_normal, {1.f, 0.f} });
		cyl_vertices.push_back({ b_left, b_normal, {0.f, 0.f} });
		for (int j = 0; j < 3; ++j)
		{
			cyl_indices.push_back(index);
			++index;
		}
		//assigne current right to be next left
		c_left = c_right;
		b_left = b_right;
	}

	m_mesh = engine::mesh::create(cyl_vertices, cyl_indices);
	for (auto entry : cyl_vertices)
		std::cout << "Cylinder:\n" << entry.position << "\n";
}

engine::cylinder::~cylinder() {}

engine::ref<engine::cylinder> engine::cylinder::create(uint32_t sections, float radius, float height, glm::vec3 center)
{
	return std::make_shared<engine::cylinder>(sections, radius, height, center);
}
