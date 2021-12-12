// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

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
		ref<mesh> mesh() { return m_mesh; }

		static ref<circle> create(glm::vec3 position, uint32_t triangles, float r);

	private:
		float m_radius;
		uint32_t m_triangles;
		glm::vec3 m_center;
		ref<engine::mesh> m_mesh;
	};
}
