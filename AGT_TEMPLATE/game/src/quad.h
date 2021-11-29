#pragma once
#include <engine.h>

class quad
{
public:
	/// \brief Constructor
	quad(glm::vec2 half_extents);

	/// \brief Destructor
	~quad();

	/// \brief Getter methods
	glm::vec2 half_extents() const { return m_half_extents; }
	engine::ref<engine::mesh> mesh() const { return m_mesh; }

	static engine::ref<quad> create(glm::vec2 half_extents);
private:
	/// \brief Fields
	// Dimensions of the quad specified by a vector of half-extents. Cuboid is centered on the origin in local coordinates.
	glm::vec2 m_half_extents;

	engine::ref<engine::mesh> m_mesh;
};

