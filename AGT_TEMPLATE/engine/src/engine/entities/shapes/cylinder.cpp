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
	float c_x;
	float c_y = m_peak.y;
	float c_z;

	//point along bottom circle
	float b_x;
	float b_y = m_center.y;
	float b_z;
}
