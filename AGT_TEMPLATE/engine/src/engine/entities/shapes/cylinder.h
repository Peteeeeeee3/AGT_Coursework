// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#pragma once
namespace engine
{
	class mesh;

	class cylinder
	{
	public:
		cylinder(uint32_t sections, float radius, float height, glm::vec3 center);
		~cylinder();
		uint32_t sections() { return m_sections; }
		float radius() { return m_radius; }
		float height() { return m_height; }
		glm::vec3 center() { return m_center; }
		glm::vec3 peak() { return m_peak; }
		ref<mesh> mesh() { return m_mesh; }

		static ref<cylinder> create(uint32_t sections, float radius, float height, glm::vec3 center);

	private:
		uint32_t m_sections;
		float m_radius;
		float m_height;
		glm::vec3 m_center;
		glm::vec3 m_peak;
		ref<engine::mesh> m_mesh;
	};
}
