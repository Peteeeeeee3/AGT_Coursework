#pragma once
#include <engine.h>

class lightning_bolt
{
public:
	lightning_bolt(glm::vec3 p0, glm::vec3 dir, float max_time);
	~lightning_bolt();


	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void activate(glm::vec3 position, glm::vec3 direction);

	void setPosition(glm::vec3 position) { m_position = position; }

	static engine::ref<lightning_bolt> create(glm::vec3 p0, glm::vec3 dir, float max_time);

private:
	void render_line_segment(glm::vec3 p0, glm::vec3 p1, float alpha, glm::vec3 colour);
	void compute_path();

	engine::ref<engine::mesh> m_line;

	bool s_active;
	float m_timer;
	float m_max_time;

	glm::vec3 m_position;

	glm::vec3 m_direction;
};
