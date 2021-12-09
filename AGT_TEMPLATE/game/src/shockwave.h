#pragma once
#include <engine.h>

class shockwave
{
public:
	shockwave();
	~shockwave();
	void initialise();
	void activate(float radius, glm::vec3 position);
	void on_render(engine::ref<engine::shader> shader);
	void on_update(const engine::timestep& timestep);
	bool is_active() { return s_active; }

private:
	engine::ref<engine::texture_2d> m_texture;
	engine::ref<engine::mesh> m_quad;
	bool s_active;
	float m_radius;
	float m_start_radius;
	float m_life;
	glm::vec3 m_position;

};
