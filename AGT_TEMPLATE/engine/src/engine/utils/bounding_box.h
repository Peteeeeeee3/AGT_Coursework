#pragma once
#include <engine.h>

namespace engine
{
	class bounding_box
	{
	public:
		bounding_box();
		~bounding_box();
		void get(glm::vec3& bottom, float& width, float& height, float& depth);
		void set_box(float width, float height, float depth, glm::vec3 bottom);
		void on_update(glm::vec3 position);
		void on_render(float r, float g, float b, const engine::ref<engine::shader>& shader);
		bool collision(bounding_box other_box);
	private:
		glm::vec3 m_bottom;
		float m_width;
		float m_height;
		float m_depth;
		engine::ref<engine::mesh> m_mesh;

		float m_rotation_amount;
	};
}
