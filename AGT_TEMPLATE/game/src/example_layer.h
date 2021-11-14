#pragma once
#include <engine.h>
#include "player.h"
#include "tower.h"

class example_layer : public engine::layer
{
public:
    example_layer();
	~example_layer();

    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

private:
	//void check_bounce();

	engine::ref<engine::skybox>			m_game_skybox{};
	engine::ref<engine::skybox>			m_menu_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_cone{};

	//menu componenets
	engine::ref<tower>					m_menu_toygun_r{};
	engine::ref<tower>					m_menu_toygun_l{};
	engine::ref<engine::game_object>	m_menu_text{};
	engine::ref<engine::game_object>	m_menu_controls{};
	bool inMenu = true;
	bool showingCtrls = false;
	glm::vec3							m_menu_active_pos = glm::vec3(0.f, 5.5f, 10.f);
	glm::vec3							m_menu_inactive_pos = glm::vec3(0.f, 5.5f, -15.f);

	player m_player;

	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_mannequin_material{};

	engine::DirectionalLight            m_directionalLight;

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	float								m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::text_manager>	m_text_manager{};

    engine::orthographic_camera			m_2d_camera; 
    engine::perspective_camera			m_3d_camera;
};
