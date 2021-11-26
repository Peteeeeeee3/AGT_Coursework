#pragma once
#include <engine.h>

namespace engine
{
	class mesh;

	class circle
	{
	public:
		circle(glm::vec3 position, uint32_t triangles, float r);
		~circle() {}
		float radius() { return m_radius; }
		uint32_t triangles() { return m_triangles; }
		glm::vec3 center() { return m_center; }

		static ref<circle> create(uint32_t triangles, float r);

	private:
		float m_radius;
		uint32_t m_triangles;
		glm::vec3 m_center;
		ref<engine::mesh> m_mesh;
	};
}
