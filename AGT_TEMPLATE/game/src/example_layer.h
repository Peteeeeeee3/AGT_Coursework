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
#include "engine/utils/bounding_box.h"
#include "engine/utils/raycaster.h"
#include "engine/utils/bounding_box.h"


class example_layer : public engine::layer
{
public:
    example_layer();
	~example_layer();

    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

private:

	/////////////////
	//misc
	/////////////////

	engine::raycaster					m_raycaster;

	engine::ref<engine::skybox>			m_game_skybox{};
	engine::ref<engine::skybox>			m_menu_skybox{};
	engine::ref<engine::game_object>	m_terrain{};

	player m_player;

	engine::ref<engine::material>		m_material{};

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	float								m_prev_sphere_y_vel = 0.f;

	/////////////////
	//selection and placement
	/////////////////

	engine::ref<wizard_hat>				m_placement_cone{};
	engine::ref<candle>					m_placement_candle{};
	engine::ref<engine::cylinder>		m_candle_body{};
	engine::ref<engine::pentahedron>	m_candle_flame{};
	engine::ref<toygun>					m_placement_gun{};

	bool								m_render_cone = false;
	bool								m_render_candle = false;
	bool								m_render_gun = false;

	engine::game_object_properties m_wiz_props;
	engine::game_object_properties m_candle_props;
	engine::game_object_properties m_toygun_props;

	bool can_afford_left_upgrade = false;
	bool can_afford_right_upgrade = false;

	void tower_select();
	void tower_unselect();

	/////////////////
	//audio
	/////////////////

	engine::ref<engine::audio_manager>  m_audio_manager{};
	float								m_volume = 1.f;

	/////////////////
	//lighting
	/////////////////

	engine::DirectionalLight            m_directionalLight;
	engine::PointLight					m_enemy_lead_light;
	engine::ref<engine::material>		m_lightsource_material{};
	engine::ref<engine::game_object>	m_enemy_lead_light_source;
	uint32_t							m_num_point_lights = 1;

	int lead_light_target_index();

	/////////////////
	//canmera handling
	/////////////////

    engine::orthographic_camera			m_2d_camera; 
    engine::perspective_camera			m_3d_camera;
	engine::ref<candle>					m_active_candle_cam;

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
	float m_path_length = 65.f;

	void init_path();
	void draw_path(const engine::ref<engine::shader>& shader);
	std::vector<engine::bounding_box>	m_path_bboxes;

	bool placement_possible(engine::bounding_box test_box);

	////////////////
	//handling waves
	////////////////
	//spider
	engine::game_object_properties		m_spider_props;
	//mech
	engine::game_object_properties		m_mech_props;
	//claptrap
	engine::game_object_properties		m_claptrap_props;
	//ironman
	engine::game_object_properties		m_ironman_props;

	std::vector<engine::ref<enemy>>		m_active_enemies;

	void new_wave();
	int m_enemy_count = 5;
	int m_wave_number = 0 ;
	float m_precision_timer = 0.f;
	int m_display_wave_timer = 0;
	int m_wave_start_time = 30;

	////////////////
	//handling towers
	////////////////

	std::vector<engine::ref<tower>>		m_towers;

	std::shared_ptr<tower>				m_selected_tower;

	////////////////
	//handling HUD
	////////////////
	engine::ref<engine::text_manager>	m_text_manager{};

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

	engine::ref<quad>					m_upgrade_zone{};
	engine::ref<quad>					m_ugl1_indicator{};
	engine::ref<quad>					m_ugl2_indicator{};
	engine::ref<quad>					m_ugr1_indicator{};
	engine::ref<quad>					m_ugr2_indicator{};
	engine::ref<quad>					m_right_upgrade{};
	engine::ref<quad>					m_left_upgrade{};
	engine::ref<engine::texture_2d>		m_upgrade_level_green_texture;
	engine::ref<engine::texture_2d>		m_upgrade_background_texture;
	engine::ref<engine::texture_2d>		m_upgrade_bkgrnd_txtr_green;


	void hud_on_render(engine::ref<engine::shader> shader);
	void hud_init();

	////////////////
	//handling menu
	////////////////

	//menu componenets
	engine::ref<toygun>					m_menu_toygun_r{};
	engine::ref<toygun>					m_menu_toygun_l{};
	engine::ref<engine::game_object>	m_menu_text{};
	engine::ref<engine::game_object>	m_menu_controls{};
	engine::ref<engine::game_object>	m_menu_controls2{};

	bool								inMenu = true;
	bool								showing_title = true;
	bool								showingCtrls = false;
	bool								showingCtrls2 = false;
	glm::vec3							m_menu_active_pos = glm::vec3(0.f, 5.5f, 10.f);
	glm::vec3							m_menu_inactive_pos = glm::vec3(0.f, 5.5f, -15.f);
};
