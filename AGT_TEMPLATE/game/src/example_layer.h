// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#pragma once
#include <engine.h>
#include "player.h"
#include "tower.h"
#include "toygun.h"
#include "wizard_hat.h"
#include "enemy.h"
#include "quad.h"
#include "candle.h"

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
	engine::ref<candle>					m_candle{};
	engine::ref<engine::cylinder>		m_candle_body{};
	engine::ref<engine::pentahedron>	m_candle_flame{};

	//menu componenets
	engine::ref<toygun>					m_menu_toygun_r{};
	engine::ref<toygun>					m_menu_toygun_l{};
	engine::ref<engine::game_object>	m_menu_text{};
	engine::ref<engine::game_object>	m_menu_controls{};
	bool								inMenu = true;
	bool								showingCtrls = false;
	glm::vec3							m_menu_active_pos = glm::vec3(0.f, 5.5f, 10.f);
	glm::vec3							m_menu_inactive_pos = glm::vec3(0.f, 5.5f, -15.f);

	player m_player;

	engine::ref<engine::material>		m_material{};

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

	//path
	std::vector<engine::ref<engine::game_object>>	m_path;

	//checkpoints
	std::vector<glm::vec3>				m_checkpoints =
	{
		// point 1						point 2							point 3							point 4
		glm::vec3(-20.f, 0.f, 0.f),		glm::vec3(-15.f, 0.f, 0.f),		glm::vec3(-15.f, 0.f, -5.f),	glm::vec3(0.f, 0.f, -5.f),

		// point 5						point 6							point 7							point 8
		glm::vec3(0.f, 0.f, 10.f),		glm::vec3(10.f, 0.f, 10.f),		glm::vec3(10.f, 0.f, 0.f),		glm::vec3(20.f, 0.f, 0.f)
	};

	//path piece positions
	std::vector<glm::vec3>				m_pp_positions =
	{
		// piece 1							// piece 2							// piece 3							// piece 4
		glm::vec3(-17.5f, -0.75f, 0.f),		glm::vec3(-15.f, -0.7501f, -2.5f),	glm::vec3(-7.5f, -0.75f, -5.f),		glm::vec3(0.f, -0.7501f, 2.5f),

		// piece 5							// piece 6							// piece 7
		glm::vec3(5.f, -0.75f, 10.f),		glm::vec3(10.f, -0.7501f, 5.f),		glm::vec3(15.f, -0.75, 0.f)
	};

	engine::ref<engine::material>		m_path_material{};

	void init_path();
	void draw_path(const engine::ref<engine::shader>& shader);

	////////////////
	//handling waves
	////////////////
	//mannequin
	engine::ref<engine::skinned_mesh>	m_skinned_mesh{};
	engine::ref<engine::material>		m_mannequin_material{};
	engine::game_object_properties		m_mannequin_props;
	//guard
	engine::ref<engine::skinned_mesh>	m_guardian_mesh{};
	engine::ref<engine::material>		m_guardian_material{};
	engine::game_object_properties		m_guardian_props;
	//spider
	engine::game_object_properties		m_spider_props;

	std::vector<engine::ref<enemy>>		m_active_enemies;

	void new_wave();
	int m_enemy_count = 1;
	int m_wave_number = 1;

	////////////////
	//handling HUD
	////////////////
	engine::ref<quad>					m_toygun_bkgrnd{};
	engine::ref<quad>					m_toygun_btn{};
	engine::ref<engine::texture_2d>		m_toygun_icon{};

	engine::ref<quad>					m_wizardhat_bkgrnd{};
	engine::ref<quad>					m_wizardhat_btn{};
	engine::ref<engine::texture_2d>		m_wizardhat_icon{};

	engine::ref<quad>					m_candle_bkgrnd{};
	engine::ref<quad>					m_candle_btn{};
	engine::ref<engine::texture_2d>		m_candle_icon{};

	engine::ref<engine::texture_2d>		m_button_bkgrnd{};

	engine::ref<quad>					m_health_quad{};
	engine::ref<engine::texture_2d>		m_health_txt2d{};

	void hud_on_render(engine::ref<engine::shader> shader);
	void hud_init();
};
