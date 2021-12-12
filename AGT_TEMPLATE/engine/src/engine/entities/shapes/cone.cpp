// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include "cone.h"
#include "pch.h"
#include <engine.h>

engine::cone::cone(uint32_t triangles, float height, float radius, glm::vec3 center) : m_triangles(triangles), m_height(height), m_radius(radius), m_center(center)
{
	// set peak point
	m_peak = m_center;
	m_peak.y += m_height;
	
	std::vector<engine::mesh::vertex> cone_vertices;
	std::vector<uint32_t> cone_indices;

	// define the angle of the individual triangles
	float step_size = 2 * glm::pi<float>() * m_radius / (m_triangles / 2);

	// triangles are drawn with two points along a circle and the third being the center or peak
	glm::vec3 left = glm::vec3(m_center.x + m_radius, m_center.y, m_center.z); // left point along circle
	glm::vec3 right; // right point along peak
	glm::vec3 normal; // normal of triangle

	// coords
	float x;
	float y = m_center.y;
	float z;

	// base
	for (int i = 0; i < m_triangles / 2; ++i) // only half of the triangles will be in the base
	{
		// calculate x and y values of right point (see report for formula)
		x = m_radius * glm::sin(step_size * i);
		z = m_radius * glm::cos(step_size * i);
		// assign coordinates
		right = glm::vec3(x, y, z);
		// calculate base triangle normal
		normal = glm::cross(m_center - left, m_center - right);
		// add base triangle (left, right, center)
		cone_vertices.push_back({ m_center, normal, {0.5f, 1.f} });
		cone_vertices.push_back({ right, normal, {1.f, 0.f} });
		cone_vertices.push_back({ left, normal, { 0.f, 0.f } });
		// calculate side triangle normal
		normal = glm::cross(m_peak - left, m_peak - right);
		// add side triangle (left, right, peak)
		cone_vertices.push_back({ left, normal, { 0.f, 0.f } });
		cone_vertices.push_back({ right, normal, {1.f, 0.f} });
		cone_vertices.push_back({ m_peak, normal, {0.5f, 1.f} });
		// assign current right to be the next left
		left = right;
	}

	// create indeces
	int index = 0;
	for (int i = 0; i < m_triangles; ++i)
	{
		//per triangle there are 3 vertices hence repeat 3 times, once per index
		for (int r = 0; r < 3; ++r)
		{
			cone_indices.push_back(index);
			++index;
		}
	}
	// create cone mesh
	m_mesh = engine::mesh::create(cone_vertices, cone_indices);
}

engine::cone::~cone() {}

engine::ref<engine::cone> engine::cone::create(uint32_t triangles, float height, float radius, glm::vec3 center)
{
	return std::make_shared<engine::cone>(triangles, height, radius, center);
}
