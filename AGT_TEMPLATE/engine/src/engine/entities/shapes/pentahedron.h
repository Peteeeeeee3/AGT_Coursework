#pragma once
namespace engine
{
	class mesh;

	class pentahedron
	{
	public:
		pentahedron(float height, float width, glm::vec3 center);
		~pentahedron();
		float height() { return m_height; }
		float width() { return m_width; }
		glm::vec3 center() { return m_center; }
		glm::vec3 peak() { return m_peak; }
		ref<mesh> mesh() { return m_mesh; }

		static ref<pentahedron> create(float height, float width, glm::vec3 center);

	private:
		float m_height;
		float m_width;
		glm::vec3 m_center;
		glm::vec3 m_peak;
		ref<engine::mesh> m_mesh;
	};
}
