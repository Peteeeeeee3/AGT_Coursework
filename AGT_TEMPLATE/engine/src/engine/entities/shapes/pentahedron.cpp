// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include "pentahedron.h"
#include "pch.h"
#include <engine.h>

engine::pentahedron::pentahedron(float height, float width, glm::vec3 center) : m_height(height), m_width(width), m_center(center)
{
	//set peak
	m_peak = m_center;
	m_peak.y += m_height;

	std::cout << "pentahedron position: " << m_center << "\n";

	//calculate the vertices
	std::vector<glm::vec3> pent_vec_vertices;
	pent_vec_vertices.push_back(glm::vec3(m_center.x - width / 2, m_center.y, m_center.z - width / 2)); //front left
	pent_vec_vertices.push_back(glm::vec3(m_center.x + width / 2, m_center.y, m_center.z - width / 2)); //front right
	pent_vec_vertices.push_back(glm::vec3(m_center.x + width / 2, m_center.y, m_center.z + width / 2)); //back right
	pent_vec_vertices.push_back(glm::vec3(m_center.x - width / 2, m_center.y, m_center.z + width / 2)); //back left
	pent_vec_vertices.push_back(m_peak);																//peak (top)

	//calculate normals
	std::vector<glm::vec3> pent_normals;
	pent_normals.push_back(glm::cross(pent_vec_vertices[0] - pent_vec_vertices[1], pent_vec_vertices[0] - pent_vec_vertices[3])); //base
	pent_normals.push_back(glm::cross(m_peak - pent_vec_vertices[0], m_peak - pent_vec_vertices[1])); //front
	pent_normals.push_back(glm::cross(m_peak - pent_vec_vertices[1], m_peak - pent_vec_vertices[2])); //right
	pent_normals.push_back(glm::cross(m_peak - pent_vec_vertices[2], m_peak - pent_vec_vertices[3])); //back
	pent_normals.push_back(glm::cross(m_peak - pent_vec_vertices[3], m_peak - pent_vec_vertices[4])); //left

	std::vector<engine::mesh::vertex> pent_vertices
	{
		//base
		{pent_vec_vertices[0], pent_normals[0], {0.f, 0.f}},
		{pent_vec_vertices[1], pent_normals[0], {1.f, 0.f}},
		{pent_vec_vertices[2], pent_normals[0], {1.f, 1.f}},
		{pent_vec_vertices[3], pent_normals[0], {0.f, 1.f}},

		//front
		{pent_vec_vertices[0], pent_normals[1], {0.f, 0.f}},
		{pent_vec_vertices[1], pent_normals[1], {1.f, 0.f}},
		{pent_vec_vertices[4], pent_normals[1], {0.5f, 1.f}},

		//right
		{pent_vec_vertices[1], pent_normals[2], {0.f, 0.f}},
		{pent_vec_vertices[2], pent_normals[2], {1.f, 0.f}},
		{pent_vec_vertices[4], pent_normals[2], {0.5f, 1.f}},

		//back
		{pent_vec_vertices[4], pent_normals[3], {0.5f, 1.f}},
		{pent_vec_vertices[3], pent_normals[3], {1.f, 0.f}},
		{pent_vec_vertices[2], pent_normals[3], {0.f, 0.f}},

		//left
		{pent_vec_vertices[3], pent_normals[4], {0.f, 0.f}},
		{pent_vec_vertices[0], pent_normals[4], {1.f, 0.f}},
		{pent_vec_vertices[4], pent_normals[4], {0.5f, 1.f}}
	};

	std::vector<uint32_t> pent_indices
	{
		0, 1, 2,	2, 3, 0,	//base 
		6, 5, 4,				//front
		9, 8, 7,				//right
		10, 11, 12,				//back
		15, 14, 13				//left
	};

	m_mesh = engine::mesh::create(pent_vertices, pent_indices);
}

engine::pentahedron::~pentahedron() {}

engine::ref<engine::pentahedron> engine::pentahedron::create(float height, float width, glm::vec3 center)
{
	return std::make_shared<engine::pentahedron>(height, width, center);
}
