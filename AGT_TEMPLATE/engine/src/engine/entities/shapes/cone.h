#pragma once

namespace engine
{
	class mesh;

	class cone
	{
	public:
		cone(uint32_t triangles, float height, float radius, glm::vec3 center);
		~cone();
		uint32_t triangles() const { return m_triangles; }
		float radius() const { return m_radius; }
		float height() const { return m_height; }
		ref<mesh> mesh() const { return m_mesh; }

		static ref<cone> create(uint32_t triangles, float height, float radius, glm::vec3 center);

	private:
		uint32_t m_triangles;
		float m_radius;
		float m_height;
		glm::vec3 m_center;
		glm::vec3 m_peak;
		ref<engine::mesh> m_mesh;
	};
}
