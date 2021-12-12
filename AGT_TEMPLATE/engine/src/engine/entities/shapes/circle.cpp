// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include <engine.h>
#include "pch.h"
#include "circle.h"

engine::circle::circle(glm::vec3 position, uint32_t triangles, float r) : m_center(position), m_triangles(triangles), m_radius(r)
{
	std::vector<engine::mesh::vertex> circle_vertices;
	std::vector<uint32_t> circle_indices;

	//define the angle of the individual triangles
	float step_size = 2 * glm::pi<float>() * m_radius / m_triangles;

	// triangles are drawn with two points along a circle and the third being the center
	glm::vec3 left = glm::vec3(m_center.x + m_radius, m_center.y, m_center.z); // left point along circle
	glm::vec3 right; // right point along circle
	glm::vec3 normal; // normal of triangle

	float x;
	float y = m_center.y;
	float z;

	for (int i = 0; i < m_triangles; ++i)
	{
		// calculate x and y values of right point
		x = m_radius * glm::sin(step_size * i);
		z = m_radius * glm::cos(step_size * i);
		//assing coordinates
		right = glm::vec3(x, y, z);
		//calculate normal
		normal = glm::cross(m_center - left, m_center - right);
		//add vertices (left, right, center)
		circle_vertices.push_back({ left, normal, { 0.f, 0.f } });
		circle_vertices.push_back({ right, normal, { 1.f, 0.f } });
		circle_vertices.push_back({ m_center, normal, { .5f, 1.f } });
		// assign current right to be the next left
		left = right;
	}

	//create indeces
	int index = 0;
	for (int i = 0; i < m_triangles; ++i)
	{
		//per triangle there are 3 vertices hence repeat 3 times, once per index
		for (int r = 0; r < 3; ++r)
		{
			circle_indices.push_back(index);
			++index;
		}
	}
	//create circle mesh
	m_mesh = engine::mesh::create(circle_vertices, circle_indices);
}

engine::ref<engine::circle> engine::circle::create(glm::vec3 position, uint32_t triangles, float r)
{
	return std::make_shared<engine::circle>(position, triangles, r);
}
