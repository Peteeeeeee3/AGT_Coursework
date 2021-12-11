#pragma once
#include <engine.h>

class billboard
{
public:
	billboard(std::string sprite_filename, uint32_t frame_width, uint32_t frame_height, uint32_t num_frames);
	~billboard();

	void on_update(const engine::timestep& time_step, bool isMoving, glm::vec3 position);
	void on_render(const engine::perspective_camera& camera, engine::ref<engine::shader> shader);
	void activate(glm::vec3 position, float width, float height);
	bool isActive() { return s_active; } 

	static engine::ref<billboard> create(std::string sprite_filename, uint32_t frame_width, uint32_t frame_height, uint32_t num_frames);
private:

	bool s_active = false;

	uint32_t m_current_frame;
	uint32_t m_num_frames;

	float m_timer;

	uint32_t m_frame_width;
	uint32_t m_frame_height;

	uint32_t m_frames_x;
	uint32_t m_frames_y;

	engine::ref<engine::texture_2d> m_sprite;

	glm::vec3 m_position;

	float m_width;
	float m_height;
};
