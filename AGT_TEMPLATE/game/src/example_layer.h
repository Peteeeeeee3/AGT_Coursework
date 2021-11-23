#pragma once
#include <engine.h>
#include "player.h"
#include "tower.h"
#include "toygun.h"
#include "wizard_hat.h"

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

	//path
	std::vector<engine::ref<engine::game_object>>	m_path;

	//menu componenets
	engine::ref<toygun>					m_menu_toygun_r{};
	engine::ref<toygun>					m_menu_toygun_l{};
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

	/////////////////
	//path of enemies
	/////////////////

	//checkpoints
	std::vector<glm::vec3> m_checkpoints =
	{
		// point 1						point 2							point 3							point 4
		glm::vec3(-20.f, 0.f, 0.f),		glm::vec3(-15.f, 0.f, 0.f),		glm::vec3(-15.f, 0.f, -5.f),	glm::vec3(0.f, 0.f, -5.f),

		// point 5						point 6							point 7							point 8
		glm::vec3(0.f, 0.f, 15.f),		glm::vec3(10.f, 0.f, 15.f),		glm::vec3(10.f, 0.f, 0.f),		glm::vec3(20.f, 0.f, 0.f)
	};

	//path piece positions
	std::vector<glm::vec3> m_pp_positions =
	{
		// piece 1							// piece 2							// piece 3							// piece 4
		glm::vec3(-17.5f, -0.75f, 0.f),		glm::vec3(-15.f, -0.75f, -2.5f),	glm::vec3(-7.5f, -0.75f, -5.f),		glm::vec3(0.f, -0.75f, 5.f),

		// piece 5							// piece 6							// piece 7
		glm::vec3(5.f, -0.75f, 15.f),		glm::vec3(10.f, -0.75f, 7.5f),		glm::vec3(15.f, -0.75, 0.f)
	};

	engine::ref<engine::material>		m_path_material{};

	void init_path();
	void draw_path(const engine::ref<engine::shader>& shader);
};
