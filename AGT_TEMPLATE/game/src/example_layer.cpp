// _____		 _____   _____   _____    _   __  _____   _____
//|  _  |		|  ___|	|  _  | |  _  |  | | / / |  _  | |  ___|
//| |_| |		| |___	| |_| | | |_| |  | |/ /  | |_| | | |___ 
//|  ___|		|  ___|	|  _  | |  _  |  |   |   |  _  | |___  | 
//| |		_   | |		| | | | | | \ \  | |\ \  | | | |  ___| |
//|_|	   |_|  |_|		|_| |_| |_|  \_\ |_| \_\ |_| |_| |_____|

#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"
#include "player.h"
#include "tower.h"
#include "toygun.h"
#include "enemy.h"
#include "engine/entities/shapes/cone.h"
#include "quad.h"
#include "candle.h"

#include <typeinfo>

#include <iterator>

template<typename Base, typename T>
inline bool instanceof(const T*) {
	return std::is_base_of<Base, T>::value;
}

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())
{
	m_raycaster = engine::raycaster::raycaster(m_3d_camera, (float)engine::application::window().width(), (float)engine::application::window().height());
	// Hide the mouse and lock it inside the window
	//engine::input::anchor_mouse(true);
	//engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/Chill Jazz Hop - Dar Golan - 85bpm - 02-47.mp3", engine::sound_type::track, "theme"); // Royalty free music from https://www.dargolan-free.com/ambient-music
	m_audio_manager->load_sound("assets/audio/Epic Battle - Dar Golan - 128bpm - 02-02.mp3", engine::sound_type::track, "boss"); // Royalty free music from https://www.dargolan-free.com/dramatic-music
	m_audio_manager->load_sound("assets/audio/bbc_electric.mp3", engine::sound_type::event, "spark"); // edited by me, original free educational and personal use, from https://sound-effects.bbcrewind.co.uk/
	m_audio_manager->load_sound("assets/audio/bbc_outdoor_short.mp3", engine::sound_type::event, "bell"); // edited by me, original free for education and personal use, from https://sound-effects.bbcrewind.co.uk/
	m_audio_manager->load_sound("assets/audio/bbc_gunshot.mp3", engine::sound_type::spatialised, "shot"); // edited by me, original free for education and personal use, from https://sound-effects.bbcrewind.co.uk/
	m_audio_manager->play("boss");
	m_audio_manager->pause("boss");

	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	m_directionalLight.Color = glm::vec3(.6f, .6f, .6f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));

	m_enemy_lead_light.Color = glm::vec3(1.f, 1.f, 1.f);
	m_enemy_lead_light.AmbientIntensity = 0.25f;
	m_enemy_lead_light.DiffuseIntensity = 0.6f;
	m_enemy_lead_light.Position = glm::vec3(-25.f, 0.1f, 0.f);

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	m_directionalLight.submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
		(float)engine::application::window().height()));
	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_lightsource_material = engine::material::create(1.0f, m_enemy_lead_light.Color,
		m_enemy_lead_light.Color, m_enemy_lead_light.Color, 1.0f);

	// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	m_menu_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/SkyboxFront.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxRight.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBack.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxLeft.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxTop.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBottom.bmp", true)
		});

	m_game_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/posz.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/posx.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/negz.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/negx.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/posy.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/negy.jpg", true)
		});

	//create player object
	m_player = player(m_3d_camera);

	m_path_material = engine::material::create(1.0f, glm::vec3(1.0f, 1.f, 1.f),
		glm::vec3(1.0f, 1.f, 1.f), glm::vec3(0.5f, 0.5f, 0.5f), .5f);

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/Wood.jpg", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.1f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(0.5f, 0.1f, 0.5f);
	terrain_props.restitution = 0.92f;
	terrain_props.position = glm::vec3(0.f, 0.0f, 0.f);
	m_terrain = engine::game_object::create(terrain_props);

	engine::ref<engine::sphere> light_source_mesh = engine::sphere::create(10, 20, 0.01f);
	engine::game_object_properties source_props;
	source_props.meshes = { light_source_mesh->mesh() };
	source_props.position = m_enemy_lead_light.Position;
	m_enemy_lead_light_source = engine::game_object::create(source_props);

	// initialize path
	init_path();

	//initialize hud
	hud_init();

	//set spider properties
	engine::ref<engine::model> spider_model = engine::model::create("assets/models/static/spider/spider.fbx");
	m_spider_props.meshes = spider_model->meshes();
	m_spider_props.textures = spider_model->textures();
	glm::vec3 spider_scale = glm::vec3(.01f);
	m_spider_props.scale = spider_scale;
	m_spider_props.position = glm::vec3(-25.f, 0.f, 0.f);
	m_spider_props.bounding_shape = glm::vec3(200.f, 160.f, 1.f);

	//set mech porperties
	engine::ref<engine::model> mech_model = engine::model::create("assets/models/static/Mech_F_432/Material/mech_f_432.obj");
	m_mech_props.meshes = mech_model->meshes();
	m_mech_props.textures = mech_model->textures();
	glm::vec3 mech_scale = glm::vec3(.7f);
	m_mech_props.scale = mech_scale;
	m_mech_props.position = glm::vec3(-25.f, 0.f, 0.f);
	m_mech_props.bounding_shape = glm::vec3(5.f, 6.f, 2.f);

	//set claptrap properties
	engine::ref<engine::model> claptrap_model = engine::model::create("assets/models/static/claptrap/Modeldatei/Claptrap3.obj");
	m_claptrap_props.meshes = claptrap_model->meshes();
	m_claptrap_props.textures = claptrap_model->textures();
	glm::vec3 claptrap_scale = glm::vec3(.3f);
	m_claptrap_props.scale = claptrap_scale;
	m_claptrap_props.position = glm::vec3(-25.f, 0.f, 0.f);
	m_claptrap_props.bounding_shape = glm::vec3(3.f, 4.f, .8f);

	//set ironman properties
	engine::ref<engine::model> ironman_model = engine::model::create("assets/models/static/IronMan/IronMan.obj");
	m_ironman_props.meshes = ironman_model->meshes();
	m_ironman_props.textures = ironman_model->textures();
	glm::vec3 ironman_scale = glm::vec3(.01f);
	m_ironman_props.scale = ironman_scale;
	m_ironman_props.position = glm::vec3(-25.f, 0.f, 0.f);
	m_ironman_props.bounding_shape = glm::vec3(100.f, 250.f, 1.f);

	// load toy gun model and create object. set its properties
	engine::ref<engine::model> toygun_model = engine::model::create("assets/models/static/Toy_Gun/handgun-lo.obj");
	m_toygun_props.meshes = toygun_model->meshes();
	m_toygun_props.textures = toygun_model->textures();
	float toygun_scale = 0.0125f;
	m_toygun_props.position = { -3.f, 3.f, 8.f };
	m_toygun_props.scale = glm::vec3(toygun_scale);
	m_menu_toygun_r = toygun::create(m_toygun_props, m_active_enemies);
	m_toygun_props.position = { 3.f, 3.f, 8.f };
	m_menu_toygun_l = toygun::create(m_toygun_props, m_active_enemies);
	m_toygun_props.position = { 0.f, 0.f, 0.f };
	m_toygun_props.bounding_shape = glm::vec3(100.f, 100.f, 1.2f);
	m_placement_gun = toygun::create(m_toygun_props, m_active_enemies);

	//initialise candle properties
	m_candle_body = engine::cylinder::create(150, 1.f, 4.f, glm::vec3(0.f, 0.f, 0.f));
	m_candle_flame = engine::pentahedron::create(.5f, .5f, glm::vec3(0.f, 4.f, 0.f));
	engine::ref<engine::texture_2d> candle_texture = engine::texture_2d::create("assets/textures/path.png", true);
	m_candle_props.position = { 0.f, 0.f, 0.f };
	m_candle_props.meshes = { m_candle_body->mesh(), m_candle_flame->mesh() };
	m_candle_props.textures = { candle_texture };
	m_candle_props.scale = glm::vec3(1.f);
	m_candle_props.bounding_shape = glm::vec3(1.f, 2.f, 1.f);
	m_placement_candle = candle::create(m_candle_props, m_active_enemies);

	//menu text 
	engine::ref<engine::cuboid> container_shape = engine::cuboid::create(glm::vec3(10.f, 4.f, 0.5f), false, false);
	engine::ref<engine::texture_2d> container_texture = engine::texture_2d::create("assets/textures/game_title.png", true);
	engine::game_object_properties container_props;
	container_props.position = { 0.f, 5.5f, 10.f };
	container_props.meshes = { container_shape->mesh() };
	float container_scale = 0.75f;
	container_props.scale = glm::vec3(container_scale);
	container_props.bounding_shape = glm::vec3(10.f, 4.f, 0.5f);
	container_props.textures = { container_texture };
	m_menu_text = engine::game_object::create(container_props);

	// controls text
	engine::ref<engine::cuboid> cont_shape = engine::cuboid::create(glm::vec3(10.f, 4.f, .5f), false, false);
	engine::ref<engine::texture_2d> cont_texture = engine::texture_2d::create("assets/textures/controls_text.png", true);
	engine::game_object_properties cont_props;
	cont_props.position = { 0.f, 5.5f, -15.f };
	cont_props.meshes = { cont_shape->mesh() };
	float cont_scale = 0.75f;
	cont_props.scale = glm::vec3(cont_scale);
	cont_props.bounding_shape = glm::vec3(10.f, 4.f, 0.5f);
	cont_props.textures = { cont_texture };
	m_menu_controls = engine::game_object::create(cont_props);

	// controls text2
	engine::ref<engine::cuboid> cont2_shape = engine::cuboid::create(glm::vec3(10.f, 4.f, .5f), false, false);
	engine::ref<engine::texture_2d> cont2_texture = engine::texture_2d::create("assets/textures/controls_text2.png", true);
	engine::game_object_properties cont2_props;
	cont2_props.position = { 0.f, 5.5f, -15.f };
	cont2_props.meshes = { cont2_shape->mesh() };
	float cont2_scale = 0.75f;
	cont2_props.scale = glm::vec3(cont2_scale);
	cont2_props.bounding_shape = glm::vec3(10.f, 4.f, 0.5f);
	cont2_props.textures = { cont2_texture };
	m_menu_controls2 = engine::game_object::create(cont2_props);

	// create cone shape and add texture to it to create a wizard hat
	engine::ref<engine::cone> cone_shape = engine::cone::create(150, 5.f, 3.f, glm::vec3(0.f, 0.f, 0.f));
	engine::ref<engine::texture_2d> cone_texture = engine::texture_2d::create("assets/textures/wizard_hat.png", true);
	m_wiz_props.position = { 0.f, 0.f, 0.f };
	m_wiz_props.meshes = { cone_shape->mesh() };
	m_wiz_props.textures = { cone_texture };
	m_wiz_props.bounding_shape = glm::vec3(2.5f, 2.5f, 1.5f);
	m_placement_cone = wizard_hat::create(m_wiz_props, m_active_enemies);

	m_game_objects.push_back(m_terrain);
	
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_3d_camera.set_view_matrix(glm::vec3(0.f, 10.f, 0.f), glm::vec3(-5.f, 0.f, -5.f));

	//play theme
	m_audio_manager->play("theme");
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{
	// update differenly when in menu
	if (inMenu)
	{
		m_directionalLight.Color = { 1.f, 1.f, 1.f };
		// lock camera is specific position and orientation 
		m_3d_camera.set_view_matrix(glm::vec3(0.f, 5.f, 0.f), glm::vec3(0.f, 5.f, 1.f));

		//update guns for rotation
		m_menu_toygun_r->rotate(true, time_step);
		m_menu_toygun_l->rotate(false, time_step);

		// position correct cuboid before player
		if (showing_title)
		{
			m_menu_controls->set_position(m_menu_inactive_pos);
			m_menu_controls2->set_position(m_menu_inactive_pos);
			m_menu_text->set_position(m_menu_active_pos);
		}
		else if (showingCtrls)
		{
			m_menu_controls->set_position(m_menu_active_pos);
			m_menu_controls2->set_position(m_menu_inactive_pos);
			m_menu_text->set_position(m_menu_inactive_pos);
		}
		else if (showingCtrls2)
		{
			m_menu_controls2->set_position(m_menu_active_pos);
			m_menu_controls->set_position(m_menu_inactive_pos);
			m_menu_text->set_position(m_menu_inactive_pos);
		}
	}
	else
	{
		m_directionalLight.Color = { 0.6f, 0.6f, 0.6f };

		//handle between waves timer
		if (m_active_enemies.size() == 0)
		{
			m_precision_timer += time_step;
			if (m_precision_timer >= 1.f)
			{
				m_display_wave_timer++;
				m_precision_timer = 0.f;
			}

			//new wave if timer is over
			if (m_display_wave_timer >= m_wave_start_time)
				new_wave();
		}

		//update enemies
		for (auto enemy : m_active_enemies)
			enemy->update(m_player, m_active_enemies, m_checkpoints, time_step);

		//update position and target of leading light
		if (m_active_enemies.size() > 0)
		{
			int target_index = lead_light_target_index();
			m_enemy_lead_light.Position = m_active_enemies.at(target_index)->position();
			m_enemy_lead_light.Position.y = 0.1f;
			m_enemy_lead_light_source->set_position(glm::vec3(m_active_enemies.at(target_index)->position().x, 0.1f, m_active_enemies.at(target_index)->position().z));
		}
		else
		{
			m_enemy_lead_light.Position = { -25.f, 0.1f, 0.f };
			m_enemy_lead_light_source->set_position(glm::vec3(-25.f, 0.1f, 0.f));
		}

		//update towers
		for (auto tower : m_towers)
		{
			tower->update(m_active_enemies, time_step);
			std::string name = typeid(*tower.get()).name();
			//specific to togyuns
			if (name == "class toygun")
			{
				auto gun_cast = std::dynamic_pointer_cast<toygun>(tower);
				//play sfx
				if (gun_cast->play_shot_sound())
				{
					m_audio_manager->play_spatialised_sound("shot", m_3d_camera.position(), gun_cast->position());
					m_audio_manager->volume("shot", m_volume);
				}
			}
			//specific to wizard hat
			if (name == "class wizard_hat")
			{
				//play sfx
				if (std::dynamic_pointer_cast<wizard_hat>(tower)->play_spark())
				{
					m_audio_manager->play("spark");
					m_audio_manager->volume("spark", m_volume);
				}
			}
			//specific to candle
			if (name == "class candle")
			{
				auto candle_cast = std::dynamic_pointer_cast<candle>(tower);
				glm::vec3 flame_pos = { candle_cast->position().x, candle_cast->position().y + 2.45f, candle_cast->position().z };
				//see if candle is the current camera
				if (candle_cast->active_cam())
				{
					//update any shots
					candle_cast->update_shot(m_3d_camera, time_step);
					if (candle_cast->active_shot())
					{
						//remove ended shots
						if (candle_cast->shot().to_remove())
						{
							candle_cast->reset_shot();
						}
					}
				}
			}
		}

		//default camera
		if (m_active_candle_cam == nullptr)
		{
			m_3d_camera.on_update(time_step);
			m_player.update_camera(m_3d_camera, time_step);
			m_raycaster.on_update(m_3d_camera);
			glm::vec3 new_pos = m_raycaster.point_on_surface();
			//update placement towers, use 1,2,3 to display when in game
			m_placement_cone->set_position(new_pos);
			m_placement_cone->range_highlight()->set_position(new_pos);
			m_placement_cone->update_bbox();
			m_placement_gun->set_position(new_pos);
			m_placement_gun->range_highlight()->set_position(new_pos);
			m_placement_gun->update_bbox();
			m_placement_candle->set_position(new_pos);
			m_placement_candle->range_highlight()->set_position(new_pos);
			m_placement_candle->update_bbox();
		}
		else
		{
			//update candle camera if selected
			m_active_candle_cam->turret_camera(m_3d_camera, time_step);
		}

		m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

		m_audio_manager->update_with_camera(m_3d_camera);
	}
} 

void example_layer::on_render() 
{
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);
	m_directionalLight.submit(mesh_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	///////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////menu rendering
	///////////////////////////////////////////////////////////////////////
	if (inMenu)
	{
		// Position the skybox centred on the player and render it
		glm::mat4 skybox_transform(1.0f);
		skybox_transform = glm::translate(skybox_transform, m_3d_camera.position());
		for (const auto& texture : m_menu_skybox->textures())
		{
			texture->bind();
		}
		engine::renderer::submit(mesh_shader, m_menu_skybox, skybox_transform);

		m_material->submit(mesh_shader);
		engine::renderer::submit(mesh_shader, m_menu_text);
		engine::renderer::submit(mesh_shader, m_menu_controls);
		engine::renderer::submit(mesh_shader, m_menu_controls2);

		// right rotating menu gun 
		glm::mat4 toygun_transfrom(1.f);
		toygun_transfrom = glm::translate(toygun_transfrom, m_menu_toygun_r->position());
		toygun_transfrom = glm::rotate(toygun_transfrom, m_menu_toygun_r->rotation_amount(), glm::vec3(0.f, 1.f, 0.f));
		toygun_transfrom = glm::scale(toygun_transfrom, glm::vec3(0.025f, 0.025f, 0.025f));
		engine::renderer::submit(mesh_shader, toygun_transfrom, m_menu_toygun_r);

		// left rotating menu gun
		glm::mat4 toygun_transfrom_l(1.f);
		toygun_transfrom_l = glm::translate(toygun_transfrom_l, m_menu_toygun_l->position());
		toygun_transfrom_l = glm::rotate(toygun_transfrom_l, m_menu_toygun_l->rotation_amount(), glm::vec3(0.f, 1.f, 0.f));
		toygun_transfrom_l = glm::scale(toygun_transfrom_l, glm::vec3(0.025f, 0.025f, 0.025f));
		engine::renderer::submit(mesh_shader, toygun_transfrom_l, m_menu_toygun_l);

		//render signature
		const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
		m_text_manager->render_text(text_shader, "Peter Farkas, 2021", (float)engine::application::window().width() - 210.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(0.f, 0.f, 0.f, 1.f));
		//render controls toggle text
		m_text_manager->render_text(text_shader, "Press C to toggle controls", (float)engine::application::window().width() / 2 - 120.f, 10.f, 0.5f, glm::vec4(0.302f, 0.f, 0.f, 1.f));
	}

	///////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////game rendering
	///////////////////////////////////////////////////////////////////////
	else
	{
		// Position the skybox centred on the player and render it
		glm::mat4 skybox_transform(1.0f);
		skybox_transform = glm::translate(skybox_transform, m_3d_camera.position());
		for (const auto& texture : m_game_skybox->textures())
		{
			texture->bind();
		}
		engine::renderer::submit(mesh_shader, m_game_skybox, skybox_transform);
		// render terrain
		engine::renderer::submit(mesh_shader, m_terrain);

		// check affordable upgrades
		if (m_selected_tower != nullptr)
		{
			//left level 1 
			if (m_player.score() >= m_selected_tower->left_lvl1_upgrade_cost() && m_selected_tower->left_level() == 0)
			{
				can_afford_left_upgrade = true;
			}
			//left level 2
			else if (m_player.score() >= m_selected_tower->left_lvl2_upgrade_cost() && m_selected_tower->left_level() == 1)
			{
				can_afford_left_upgrade = true;
			}
			else
			{
				can_afford_left_upgrade = false;
			}

			//right level 1
			if (m_player.score() >= m_selected_tower->right_lvl1_upgrade_cost() && m_selected_tower->right_level() == 0)
			{
				can_afford_right_upgrade = true;
			}
			//left level 2
			else if (m_player.score() >= m_selected_tower->right_lvl2_upgrade_cost() && m_selected_tower->right_level() == 1)
			{
				can_afford_right_upgrade = true;
			}
			else
			{
				can_afford_right_upgrade = false;
			}
		}

		//render path
		draw_path(mesh_shader);
		//for (auto segment : m_path_bboxes)
		//	segment.on_render(0.f, 0.f, 0.f, mesh_shader);
		
		//render towers
		for (auto tower : m_towers)
		{
			glm::mat4 tower_transform(1.f);
			
			std::string name = typeid(*tower.get()).name();
			//specific to toy guns for scaling purposes
			if (name == "class toygun")
			{
				tower_transform = glm::translate(tower_transform, glm::vec3(tower->position().x, tower->position().y + 1.f, tower->position().z));
				tower_transform = glm::rotate(tower_transform, tower->rotation_amount(), glm::vec3(0.f, 1.f, 0.f));
				tower_transform = glm::scale(tower_transform, glm::vec3(0.025f));
				std::dynamic_pointer_cast<toygun>(tower)->render_bullets(mesh_shader);
			}
			else
			{
				tower_transform = glm::translate(tower_transform, tower->position());
				tower_transform = glm::scale(tower_transform, glm::vec3(0.5f));
			}
			engine::renderer::submit(mesh_shader, tower_transform, tower);
			//render ranges, if required
			if (tower->to_render_range())
				tower->render_range(mesh_shader);

			//specific to wizard hat
			if (name == "class wizard_hat")
			{
				//render lightning of wizard hat
				std::dynamic_pointer_cast<wizard_hat>(tower)->lightning().on_render(mesh_shader);
			}

			//specific to candle
			if (name == "class candle")
			{
				//update flame, fireballs and their aoe effect
				std::dynamic_pointer_cast<candle>(tower)->fire_ring().on_render(mesh_shader);
				if (std::dynamic_pointer_cast<candle>(tower)->active_shot())
					std::dynamic_pointer_cast<candle>(tower)->shot().on_render(mesh_shader);
			}
		}

		//render enemies
		if (m_active_enemies.size() != 0)
		{
			for (int i = 0; i < m_active_enemies.size(); ++i)
			{
				if (m_active_enemies.at(i)->isDead())
				{
					//handle enemy death
					if (m_active_enemies.at(i)->state() != 8)
						m_player.set_score(m_player.score() + m_active_enemies.at(i)->strength());
					//handle enemy reaching finish
					else
						m_player.set_score(m_player.score() - m_active_enemies.at(i)->strength());
					//erase flagged enemy
					m_active_enemies.erase(m_active_enemies.begin() + i);
				}
				else
					if (m_active_enemies.at(i)->toRender())
					{
						//m_active_enemies.at(i)->bounding_box().on_render(1.f, 0.f, 0.f, mesh_shader);
						engine::renderer::submit(mesh_shader, m_active_enemies.at(i));
					}
			}
		}

		//render placement aid towers
		if (m_render_cone)
		{
			glm::mat4 transform(1.f);
			transform = glm::translate(transform, m_placement_cone->position());
			transform = glm::scale(transform, glm::vec3(0.5f));
			engine::renderer::submit(mesh_shader, transform, m_placement_cone);
			m_placement_cone->render_range(mesh_shader);
			m_placement_cone->bounding_box().on_render(0.f, 0.f, 0.f, mesh_shader);
		}
		if (m_render_candle)
		{
			glm::mat4 transform(1.f);
			transform = glm::translate(transform, m_placement_candle->position());
			transform = glm::scale(transform, glm::vec3(0.5f));
			engine::renderer::submit(mesh_shader, transform, m_placement_candle);
			m_placement_candle->render_range(mesh_shader);
			m_placement_candle->bounding_box().on_render(0.f, 0.f, 0.f, mesh_shader);
		}
		if (m_render_gun)
		{
			glm::mat4 transform(1.f);
			transform = glm::translate(transform, m_placement_gun->position());
			transform = glm::scale(transform, glm::vec3(0.025f));
			engine::renderer::submit(mesh_shader, transform, m_placement_gun);
			m_placement_gun->render_range(mesh_shader);
			m_placement_gun->bounding_box().on_render(0.f, 0.f, 0.f, mesh_shader);
		}


		engine::renderer::end_scene();

		engine::renderer::begin_scene(m_3d_camera, mesh_shader);

		//render enemy lead highlight light
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gNumPointLights", (int) m_num_point_lights);
		m_enemy_lead_light.submit(mesh_shader, 0);
		m_lightsource_material->submit(mesh_shader);
		engine::renderer::submit(mesh_shader, m_enemy_lead_light_source->meshes().at(0), glm::translate(glm::mat4(1.f), m_enemy_lead_light.Position));

		for (auto tower : m_towers)
		{
			std::string name = typeid(*tower.get()).name();
			//specific to candle
			if (name == "class candle")
			{
				//render candle light
				m_num_point_lights++;
				std::dynamic_pointer_cast<candle>(tower)->flame()->on_render(m_3d_camera, mesh_shader);
				if (std::dynamic_pointer_cast<candle>(tower)->active_shot())
					std::dynamic_pointer_cast<candle>(tower)->shot().flame()->on_render(m_3d_camera, mesh_shader);
				std::dynamic_pointer_cast<candle>(tower)->light_render(mesh_shader, m_num_point_lights);
			}
		}

		//reset number of point lights to avoid problems
		m_num_point_lights = 1;
		engine::renderer::end_scene();

		//render hud
		hud_on_render(mesh_shader);
	}
} 

void example_layer::on_event(engine::event& event) 
{ 
    if(event.event_type() == engine::event_type_e::key_pressed) 
    { 
        auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		// wireframe
		if (e.key_code() == engine::key_codes::KEY_TAB)
		{	
			engine::render_command::toggle_wireframe();
		}

		//volume up
		if (e.key_code() == engine::key_codes::KEY_UP)
		{
			if (m_volume <= 2.9f)
				m_volume += 0.1f;
			m_audio_manager->volume("theme", m_volume);
			m_audio_manager->volume("boss", m_volume);
		}

		//volume down
		if (e.key_code() == engine::key_codes::KEY_DOWN)
		{
			if (m_volume >= 0.1f)
				m_volume -= 0.1f;
			m_audio_manager->volume("theme", m_volume);
			m_audio_manager->volume("boss", m_volume);
		}

		// detect player opening menu
		if (e.key_code() == engine::key_codes::KEY_BACKSPACE)
		{
			if (!inMenu)
				inMenu = true;
			else
				inMenu = false;
		}

		//during game time
		if (!inMenu)
		{
			//start new wave
			if (e.key_code() == engine::key_codes::KEY_N)
			{
				new_wave();
			}

			//turret cam
			if (e.key_code() == engine::key_codes::KEY_C)
			{
				if (m_selected_tower != nullptr)
				{
					std::string name = typeid(*m_selected_tower.get()).name();
					if (name == "class candle")
					{
						//enter camera
						if (!std::dynamic_pointer_cast<candle>(m_selected_tower)->active_cam())
						{
							std::dynamic_pointer_cast<candle>(m_selected_tower)->toggle_cam(true);
							m_active_candle_cam = std::dynamic_pointer_cast<candle>(m_selected_tower);
							engine::application::window().hide_mouse_cursor();
						}
					}
				}
				else if (m_active_candle_cam != nullptr)
				{
					//exit camera
					if (m_active_candle_cam->active_cam())
					{
						m_active_candle_cam->toggle_cam(false);
						m_active_candle_cam = nullptr;
						engine::application::window().show_mouse_cursor();
						m_player.cam_reset();
					}
				}
			}

			//activate placement aid toy gun
			if (e.key_code() == engine::key_codes::KEY_1)
			{
				if (m_render_gun)
				{
					m_render_gun = false;
					m_placement_gun->set_to_render_range(false);
				}
				else
				{
					m_render_gun = true;
					m_placement_gun->set_to_render_range(true);
					m_render_cone = false;
					m_placement_cone->set_to_render_range(false);
					m_render_candle = false;
					m_placement_candle->set_to_render_range(false);
				}
			}

			//activate placement aid wizard hat
			if (e.key_code() == engine::key_codes::KEY_2)
			{
				if (m_render_cone)
				{
					m_render_cone = false;
					m_placement_cone->set_to_render_range(false);
				}
				else
				{
					m_render_cone = true;
					m_placement_cone->set_to_render_range(true);
					m_render_candle = false;
					m_placement_candle->set_to_render_range(false);
					m_render_gun = false;
					m_placement_gun->set_to_render_range(false);
				}
			}

			//activate placement aid candle
			if (e.key_code() == engine::key_codes::KEY_3)
			{
				if (m_render_candle)
				{
					m_render_candle = false;
					m_placement_candle->set_to_render_range(false);
				}
				else
				{
					m_render_candle = true;
					m_placement_candle->set_to_render_range(true);
					m_render_gun = false;
					m_placement_gun->set_to_render_range(false);
					m_render_cone = false;
					m_placement_cone->set_to_render_range(false);
				}
			}

			//close upgrade tab
			if (e.key_code() == engine::key_codes::KEY_X)
			{
				if (m_selected_tower != nullptr)
				{
					tower_unselect();
				}
			}

			if (m_selected_tower != nullptr)
			{
				//upgrade tower left branch
				if (e.key_code() == engine::key_codes::KEY_K)
				{
					if (m_selected_tower->left_level() == 0)
					{
						m_selected_tower->upgradeLeft_lvl1(m_player);
					}
					else if (m_selected_tower->left_level() == 1)
					{
						m_selected_tower->upgradeLeft_lvl2(m_player);
					}
				}

				//upgrade tower right branch
				if (e.key_code() == engine::key_codes::KEY_L)
				{
					if (m_selected_tower->right_level() == 0)
					{
						m_selected_tower->upgradeRight_lvl1(m_player);
					}
					else if (m_selected_tower->right_level() == 1)
					{
						m_selected_tower->upgradeRight_lvl2(m_player);
					}
				}
			}
		}

		// menu controls
		if(inMenu)
		{
			// start game
			if (e.key_code() == engine::key_codes::KEY_SPACE)
			{
				inMenu = false;
			}
			// toggle controls
			if (e.key_code() == engine::key_codes::KEY_C)
			{
				if (showing_title)
				{
					showingCtrls = true;
					showing_title = false;
				}
				else if (showingCtrls)
				{
					showingCtrls = false;
					showingCtrls2 = true;
				}
				else
				{
					showing_title = true;
					showingCtrls2 = false;
				}
			}
		}
    }

	//tower placement
	if (engine::input::mouse_button_pressed(0))
	{
		//place wizard hat
		if (m_render_cone)
		{
			float cost = 600.f;
			if (m_player.score() >= cost && placement_possible(m_placement_cone->bounding_box()))
			{
				m_wiz_props.position = m_placement_cone->position();
				//add to towers
				m_towers.push_back(wizard_hat::create(m_wiz_props, m_active_enemies));
				m_render_cone = false;
				m_player.set_score(m_player.score() - cost);
			}
		}

		//place candle
		if (m_render_candle)
		{
			float cost = 800.f;
			if (m_player.score() >= cost && placement_possible(m_placement_candle->bounding_box()))
			{
				m_candle_props.position = m_placement_candle->position();
				//add to towers
				m_towers.push_back(candle::create(m_candle_props, m_active_enemies));
				m_render_candle = false;
				m_player.set_score(m_player.score() - cost);
			}
		}

		//place toy gun
		if (m_render_gun)
		{
			float cost = 650.f;
			if (m_player.score() >= cost && placement_possible(m_placement_gun->bounding_box()))
			{
				m_toygun_props.position = m_placement_gun->position();
				//add to towers
				m_towers.push_back(toygun::create(m_toygun_props, m_active_enemies));
				m_render_gun = false;
				m_player.set_score(m_player.score() - cost);
			}
		}

		//unselect tower if mouse is clicked away 
		if (!(m_render_cone || m_render_candle || m_render_gun))
		{
			if (m_selected_tower != nullptr)
				tower_unselect();
			else if (m_active_candle_cam == nullptr)
				tower_select();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//path functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void example_layer::init_path()
{
	for (int i = 0; i < m_pp_positions.size(); ++i)
	{
		//cast the iterater as it was complaining and this solve it. something about 4 byte and 8 byte values
		glm::vec3 size = glm::vec3(fabs(m_checkpoints[static_cast<size_t>(i) + 1].x - m_checkpoints[i].x) / 2 + 0.5f, 1.f, fabs(m_checkpoints[static_cast<size_t>(i) + 1].z - m_checkpoints[i].z) / 2 + 0.5f);
		if (size.x == 0.f)
			size.x += .5f;
		if (size.z == 0.f)
			size.z += .5f;
		//initialise path highlights
		engine::ref<engine::cuboid> path_shape = engine::cuboid::create(size, false, true);
		engine::game_object_properties path_props;
		path_props.position = m_pp_positions[i];
		path_props.meshes = { path_shape->mesh() };
		path_props.scale = glm::vec3(1);
		path_props.restitution = 0.92f;
		path_props.bounding_shape = { size.x, size.y, size.z };
		engine::ref<engine::game_object> path_piece = engine::game_object::create(path_props);
		m_path.push_back(path_piece);
		//initialise path bounding boxes
		m_path_bboxes.push_back(engine::bounding_box());
		m_path_bboxes.at(m_path_bboxes.size() - 1).set_box(size.x * 2, size.y * 2, size.z * 2, m_pp_positions[i]);
	}
}

//render path
void example_layer::draw_path(const engine::ref<engine::shader>& shader)
{
	for (auto piece : m_path)
	{
		m_path_material->submit(shader);
		engine::renderer::submit(shader, piece);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//wave and enemy handling functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void example_layer::new_wave()
{
	//only start new wave if there is no active wave
	if (m_active_enemies.size() == 0)
	{
		//handle audio
		m_audio_manager->pause("boss");
		m_audio_manager->pause("theme");
		++m_wave_number;
		//spawn claptraps
		if (m_wave_number % 2 == 0)
		{
			for (int i = 0; i < m_wave_number / 2; ++i)
			{
				m_active_enemies.push_back(enemy::create(m_claptrap_props, 100.f, 5.f, 1.f, 1.f * i, enemy::e_type::CLAPTRAP));
			}
		}

		//spawn mech
		if (m_wave_number % 5 == 0)
		{
			for (int i = 0; i < m_wave_number / 5; ++i)
			{
				m_active_enemies.push_back(enemy::create(m_mech_props, 500.f, 20.f, 0.65f, 8.f * i, enemy::e_type::MECH));
			}
		}

		//spawn ironman
		if (m_wave_number % 10 == 0)
		{
			for (int i = 0; i < m_wave_number / 10; ++i)
			{
				m_active_enemies.push_back(enemy::create(m_ironman_props, 1000.f, 50.f, 0.4f, 5.f * i, enemy::e_type::IRONMAN));
			}
			//start boss music
			m_audio_manager->unpause("boss");
			m_audio_manager->volume("boss", m_volume);
		}
		else
		{
			//start theme music
			m_audio_manager->unpause("theme");
			m_audio_manager->volume("theme", m_volume);
		}

		//spawn spiders
		for (int i = 0; i < m_enemy_count; ++i)
		{
			m_active_enemies.push_back(enemy::create(m_spider_props, 50.f, 5.f, 3.f, 1.f * i, enemy::e_type::SPIDER));
		}

		++m_enemy_count;
		m_display_wave_timer = 0;
		m_precision_timer = 0.f;
		//play bell sound to start wave
		m_audio_manager->play("bell");
		m_audio_manager->volume("bell", m_volume);
	}
}

int example_layer::lead_light_target_index()
{
	int return_index = NULL;
	engine::ref<enemy> current_target;
	//only compute if there are active enemies
	if (m_active_enemies.size() != 0)
	{
		//initialize current target to be the first entry
		current_target = m_active_enemies.at(0);
		for (int i = 0; i < m_active_enemies.size(); ++i)
		{
			if (m_active_enemies.at(i)->distance_covered() > current_target->distance_covered())
			{
				//override current target if a more suitable one is found
				current_target = m_active_enemies.at(i);
				return_index = i;
			}
		}
	}
	return return_index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HUD
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void example_layer::hud_on_render(engine::ref<engine::shader> shader)
{
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	//render score
	m_text_manager->render_text(text_shader, "Score: " + std::to_string((int) m_player.score()), (float) engine::application::window().width() / 2.f - 90.f, (float)engine::application::window().height() - 65.f, .75f, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
	//render health
	m_text_manager->render_text(text_shader, std::to_string((int) m_player.health()), 135.f, (float) engine::application::window().height() - 65.f, .55f, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
	//wave number
	m_text_manager->render_text(text_shader, "Wave: " + std::to_string((int)m_wave_number), (float) engine::application::window().width() - 135.f, (float) engine::application::window().height() - 65.f, .55f, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
	//tower prices
	//gun
	m_text_manager->render_text(text_shader, "650", (float)engine::application::window().width() - 95.f, (float)engine::application::window().height() / 2 + 33.f, .4f, glm::vec4(0.f, 0.f, 0.f, 1.f));
	//wiz hat
	m_text_manager->render_text(text_shader, "600", (float)engine::application::window().width() - 95.f, (float)engine::application::window().height() / 2 - 46.f, .4f, glm::vec4(0.f, 0.f, 0.f, 1.f));
	//candle
	m_text_manager->render_text(text_shader, "800", (float)engine::application::window().width() - 95.f, (float)engine::application::window().height() / 2 - 128.f, .4f, glm::vec4(0.f, 0.f, 0.f, 1.f));

	//only render when there is no active wave
	if (m_active_enemies.size() == 0)
	{
		m_text_manager->render_text(text_shader, "Next wave in: " + std::to_string(m_wave_start_time - m_display_wave_timer) + "s", (float)engine::application::window().width() / 2.f - 135.f, (float)engine::application::window().height() - 130.f, .75f, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
		m_text_manager->render_text(text_shader, "Press \"N\" to start wave now", (float)engine::application::window().width() / 2.f - 210.f, (float)engine::application::window().height() - 180.f, .75f, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
	}

	engine::renderer::begin_scene(m_2d_camera, shader);
	////////////////
	//render buttons
	////////////////

	//toy gun
	glm::mat4 tg_transform(1.f);
	tg_transform = glm::translate(tg_transform, glm::vec3(1.4f, 0.2f, 0.1f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_button_bkgrnd->bind();
	engine::renderer::submit(shader, m_toygun_bkgrnd->mesh(), tg_transform);

	//wizard hat
	glm::mat4 wh_transform(1.f);
	wh_transform = glm::translate(wh_transform, glm::vec3(1.4f, 0.f, 0.1f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_button_bkgrnd->bind();
	engine::renderer::submit(shader, m_wizardhat_bkgrnd->mesh(), wh_transform);

	//candle
	glm::mat4 c_transform(1.f);
	c_transform = glm::translate(c_transform, glm::vec3(1.4f, -0.2f, 0.1f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_button_bkgrnd->bind();
	engine::renderer::submit(shader, m_candle_bkgrnd->mesh(), c_transform);

	////////////////
	//render icons
	////////////////

	//toy gun
	glm::mat4 icon_tg_tranform(1.f);
	icon_tg_tranform = glm::translate(icon_tg_tranform, glm::vec3(1.4f, 0.2f, 0.2f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_toygun_icon->bind();
	engine::renderer::submit(shader, m_toygun_btn->mesh(), icon_tg_tranform);

	//wizard hat
	glm::mat4 icon_wh_transform(1.f);
	icon_wh_transform = glm::translate(icon_wh_transform, glm::vec3(1.4f, 0.f, 0.2f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_wizardhat_icon->bind();
	engine::renderer::submit(shader, m_wizardhat_btn->mesh(), icon_wh_transform);

	//candle
	glm::mat4 icon_c_transform(1.f);
	icon_c_transform = glm::translate(icon_c_transform, glm::vec3(1.4f, -0.2f, 0.2f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_candle_icon->bind();
	engine::renderer::submit(shader, m_candle_btn->mesh(), icon_c_transform);

	//health
	glm::mat4 health_transform(1.f);
	health_transform = glm::translate(health_transform, glm::vec3(-1.4f, 0.75f, 0.2f));
	//scale heart appropriately
	if (m_player.health() > 0)
		health_transform = glm::scale(health_transform, glm::vec3(0.01f * m_player.health() + 0.3f));
	else
		health_transform = glm::scale(health_transform, glm::vec3(0.3f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_health_txt2d->bind();
	engine::renderer::submit(shader, m_health_quad->mesh(), health_transform);

	//only render if a tower is selected
	if (m_selected_tower != nullptr)
	{
		//upgrades
		glm::mat4 upgrade_transform(1.f);
		upgrade_transform = glm::translate(upgrade_transform, glm::vec3(0.f, -0.75f, 0.2f));
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
		m_button_bkgrnd->bind();
		engine::renderer::submit(shader, m_upgrade_zone->mesh(), upgrade_transform);

		if (m_selected_tower->left_level() >= 1)
		{
			//left level 1
			glm::mat4 l1_left_transform(1.f);
			l1_left_transform = glm::translate(l1_left_transform, glm::vec3(-0.4f, -0.65f, 0.21f));
			std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
			std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
			m_upgrade_level_green_texture->bind();
			engine::renderer::submit(shader, m_ugl1_indicator->mesh(), l1_left_transform);
		}

		if (m_selected_tower->left_level() >= 2)
		{
			//left level 2
			glm::mat4 l2_left_transform(1.f);
			l2_left_transform = glm::translate(l2_left_transform, glm::vec3(-0.3f, -0.65f, 0.21f));
			std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
			std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
			m_upgrade_level_green_texture->bind();
			engine::renderer::submit(shader, m_ugl2_indicator->mesh(), l2_left_transform);
		}

		if (m_selected_tower->right_level() >= 1)
		{
			//right level 1
			glm::mat4 l1_right_transform(1.f);
			l1_right_transform = glm::translate(l1_right_transform, glm::vec3(0.3f, -0.65f, 0.21f));
			std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
			std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
			m_upgrade_level_green_texture->bind();
			engine::renderer::submit(shader, m_ugr1_indicator->mesh(), l1_right_transform);
		}

		if (m_selected_tower->right_level() >= 2)
		{
			//right level 2
			glm::mat4 l2_right_transform(1.f);
			l2_right_transform = glm::translate(l2_right_transform, glm::vec3(0.4f, -0.65f, 0.21f));
			std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
			std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
			m_upgrade_level_green_texture->bind();
			engine::renderer::submit(shader, m_ugr2_indicator->mesh(), l2_right_transform);
		}

		//left upgrade slot
		glm::mat4 lugs_transfrom(1.f);
		lugs_transfrom = glm::translate(lugs_transfrom, glm::vec3(-0.3f, -0.8f, 0.21f));
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
		if (can_afford_left_upgrade)
			m_upgrade_level_green_texture->bind();
		else
			m_upgrade_background_texture->bind();
		engine::renderer::submit(shader, m_left_upgrade->mesh(), lugs_transfrom);

		//right upgrade slot
		glm::mat4 rugs_transfrom(1.f);
		rugs_transfrom = glm::translate(rugs_transfrom, glm::vec3(0.3f, -0.8f, 0.21f));
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", .3f);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
		if (can_afford_right_upgrade)
			m_upgrade_level_green_texture->bind();
		else
			m_upgrade_background_texture->bind();
		engine::renderer::submit(shader, m_left_upgrade->mesh(), rugs_transfrom);

		//render upgrading text
		m_text_manager->render_text(text_shader, "Press \"K\" to upgrade", (float)engine::application::window().width() / 2 - 250.f, 140.f, .5f, glm::vec4(0.f, 0.f, 0.f, 1.f));
		m_text_manager->render_text(text_shader, "Press \"L\" to upgrade", (float)engine::application::window().width() / 2 + 30.f, 140.f, .5f, glm::vec4(0.f, 0.f, 0.f, 1.f));
	}
}

void example_layer::hud_init()
{
	//intialize button background
	m_button_bkgrnd = engine::texture_2d::create("assets/textures/button_background.png", true);

	//initialize button icons
	m_wizardhat_icon = engine::texture_2d::create("assets/textures/wizhat_icon.png", true);
	m_toygun_icon = engine::texture_2d::create("assets/textures/toygun_icon.png", true);
	m_candle_icon = engine::texture_2d::create("assets/textures/candle_icon.png", true);

	//create buttons
	m_wizardhat_btn = quad::create(glm::vec2(0.08f, 0.08f));
	m_toygun_btn = quad::create(glm::vec2(0.08f, 0.08f));
	m_candle_btn = quad::create(glm::vec2(0.08f, 0.08f));

	//create backgrounds
	m_wizardhat_bkgrnd = quad::create(glm::vec2(0.08f, 0.08f));
	m_toygun_bkgrnd = quad::create(glm::vec2(0.08f, 0.08f));
	m_candle_bkgrnd = quad::create(glm::vec2(0.08f, 0.08f));

	//create health icon
	m_health_txt2d = engine::texture_2d::create("assets/textures/health.png", true);
	m_health_quad = quad::create(glm::vec2(0.08f, 0.08f));

	//create upgrade tab
	m_upgrade_zone = quad::create(glm::vec2(0.8f, 0.15f));
	m_ugl1_indicator = quad::create(glm::vec2(0.04f, 0.04f));
	m_ugl2_indicator = quad::create(glm::vec2(0.04f, 0.04f));
	m_ugr1_indicator = quad::create(glm::vec2(0.04f, 0.04f));
	m_ugr2_indicator = quad::create(glm::vec2(0.04f, 0.04f));
	m_left_upgrade = quad::create(glm::vec2(0.15f, 0.08f));
	m_right_upgrade = quad::create(glm::vec2(0.15f, 0.08f));
	m_upgrade_level_green_texture = engine::texture_2d::create("assets/textures/green.png", true);
	m_upgrade_background_texture = engine::texture_2d::create("assets/textures/grey.png", true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//tower selection and placement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool example_layer::placement_possible(engine::bounding_box test_box)
{
	//placement is possible unless there is a collision
	bool test_result = true;

	//check against other towers
	for (auto tower : m_towers)
		if (test_box.collision(tower->bounding_box()))
			test_result = false;

	//check against path segments
	for (auto segment : m_path_bboxes)
		if (test_box.collision(segment))
			test_result = false;

	return test_result;
}

void example_layer::tower_select()
{
	for (auto tower : m_towers)
	{
		//select first tower with colliding bounding boxes
		if (m_placement_gun->bounding_box().collision(tower->bounding_box()))
		{
			m_selected_tower = tower;
			m_selected_tower->set_to_render_range(true);
			break;
		}
	}
}

void example_layer::tower_unselect()
{
	m_selected_tower->set_to_render_range(false);
	m_selected_tower = nullptr;
}
