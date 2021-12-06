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

#include <iterator>

template<typename Base, typename T>
inline bool instanceof(const T*) {
	return std::is_base_of<Base, T>::value;
}

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())
{
    // Hide the mouse and lock it inside the window
    //engine::input::anchor_mouse(true);
    //engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.wav", engine::sound_type::spatialised, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	m_audio_manager->play("music");
	m_audio_manager->pause("music");


	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

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

	// initialize path
	init_path();

	//initialize hud
	hud_init();

	//set spider properties
	engine::ref<engine::model> spider_model = engine::model::create("assets/models/static/spider/Only_Spider_with_Animations_Export.obj");
	m_spider_props.meshes = spider_model->meshes();
	m_spider_props.textures = spider_model->textures();
	glm::vec3 spider_scale = glm::vec3(.01f);
	m_spider_props.scale = spider_scale;
	m_spider_props.position = glm::vec3(-25.f, 0.f, 0.f);
	m_spider_props.bounding_shape = glm::vec3(200.f, 80.f, 1.f);

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
	engine::game_object_properties toygun_props;
	toygun_props.meshes = toygun_model->meshes();
	toygun_props.textures = toygun_model->textures();
	float toygun_scale = 0.0125f;
	toygun_props.position = { -3.f, 3.f, 8.f };
	toygun_props.scale = glm::vec3(toygun_scale);
	toygun_props.bounding_shape = toygun_model->size() / 2.f * toygun_scale;
	m_menu_toygun_r = toygun::create(toygun_props, m_active_enemies);
	toygun_props.position = { 3.f, 3.f, 8.f };
	m_menu_toygun_l = toygun::create(toygun_props, m_active_enemies);

	engine::game_object_properties candle_props;
	candle_props.position = { 0.f, 0.f, 0.f };
	m_candle_body = engine::cylinder::create(150, 1.f, 4.f, candle_props.position);
	m_candle_flame = engine::pentahedron::create(.5f, .5f, glm::vec3(candle_props.position.x, candle_props.position.y + 4.f, candle_props.position.z));
	engine::ref<engine::texture_2d> candle_texture = engine::texture_2d::create("assets/textures/path.png", true);
	candle_props.meshes = { m_candle_body->mesh(), m_candle_flame->mesh() };
	candle_props.textures = { candle_texture };
	candle_props.scale = glm::vec3(1.f);
	m_candle = candle::create(candle_props, m_active_enemies);
	m_towers.push_back(m_candle);

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

	// create cone shape and add texture to it to create a wizard hat
	engine::ref<engine::cone> cone_shape = engine::cone::create(150, 5.f, 3.f, glm::vec3(0.f, 0.f, 0.f));
	engine::ref<engine::texture_2d> cone_texture = engine::texture_2d::create("assets/textures/wizard_hat.png", true);
	engine::game_object_properties cone_props;
	cone_props.position = { 5.f, 0.f, 0.f };
	cone_props.meshes = { cone_shape->mesh() };
	cone_props.textures = { cone_texture };
	m_cone = wizard_hat::create(cone_props, m_active_enemies);
	m_towers.push_back(m_cone);

	m_game_objects.push_back(m_terrain);
	
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_3d_camera.set_view_matrix(glm::vec3(0.f, 10.f, 0.f), glm::vec3(-5.f, 0.f, -5.f));
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{
	// update differenly when in menu
	if (inMenu)
	{
		// lock camera is specific position and orientation 
		m_3d_camera.set_view_matrix(glm::vec3(0.f, 5.f, 0.f), glm::vec3(0.f, 5.f, 1.f));

		//update guns for rotation
		m_menu_toygun_r->rotate(true, time_step);
		m_menu_toygun_l->rotate(false, time_step);

		// position correct cuboid before player
		if (showingCtrls)
		{
			m_menu_controls->set_position(m_menu_active_pos);
			m_menu_text->set_position(m_menu_inactive_pos);
		}
		else
		{
			m_menu_controls->set_position(m_menu_inactive_pos);
			m_menu_text->set_position(m_menu_active_pos);
		}
	}
	else
	{
		for (auto enemy : m_active_enemies)
			enemy->update(m_player, m_checkpoints, time_step);

		for (auto tower : m_towers)
			tower->update(m_active_enemies, time_step);
		// update camera via player class
		// this is separated from the camera class as I will need to make multiple cameras and I do not want their codes to interfere
		// the player can be imagined as a floating camera with some attributes like health, score, etc.
		m_player.update_camera(m_3d_camera, time_step);

		m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

		m_audio_manager->update_with_camera(m_3d_camera);

		for (auto enemy : m_active_enemies)
		{
			enemy->update(m_player, m_checkpoints, time_step);
		}

		for (auto tower : m_towers)
		{
			if (instanceof<candle>(tower.get()))
			{
				std::dynamic_pointer_cast<candle>(tower)->flame()->on_update(time_step);
			}
		}
	}
} 

void example_layer::on_render() 
{
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);

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
	else {
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
		
		//render towers
		for (auto tower : m_towers)
		{
			glm::mat4 tower_transform(1.f);
			tower_transform = glm::translate(tower_transform, tower->position());
			tower_transform = glm::scale(tower_transform, glm::vec3(0.5f));
			engine::renderer::submit(mesh_shader, tower_transform, tower);
			if (tower->to_render_range())
				tower->render_range(mesh_shader);
		}

		//render enemies
		if (m_active_enemies.size() != 0)
		{
			for (int i = 0; i < m_active_enemies.size(); ++i)
			{
				if (m_active_enemies.at(i)->isDead())
					m_active_enemies.erase(m_active_enemies.begin() + i);
				else
					if (m_active_enemies.at(i)->toRender())
					{
						m_active_enemies.at(i)->bounding_box().on_render(1.f, 0.f, 0.f, mesh_shader);
						engine::renderer::submit(mesh_shader, m_active_enemies.at(i));
					}
			}
		}

		//render path
		draw_path(mesh_shader);

		engine::renderer::end_scene();

		engine::renderer::begin_scene(m_3d_camera, mesh_shader);
		for (auto tower : m_towers)
		{
			if (instanceof<candle>(tower.get()))
			{
				std::dynamic_pointer_cast<candle>(tower)->flame()->on_render(m_3d_camera, mesh_shader);
			}
		}
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

		// detect player opening menu
		if (e.key_code() == engine::key_codes::KEY_BACKSPACE)
		{
			if (!inMenu)
				inMenu = true;
			else
				inMenu = false;
		}

		if (!inMenu)
		{
			if (e.key_code() == engine::key_codes::KEY_N)
			{
				new_wave();
			}
		}

		if (e.key_code() == engine::key_codes::KEY_8)
			m_player.damage(5);

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
				if (!showingCtrls)
					showingCtrls = true;
				else
					showingCtrls = false;
			}
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
		glm::vec3 size = glm::vec3(fabs(m_checkpoints[static_cast<size_t>(i) + 1].x - m_checkpoints[i].x) / 2, 1.f, fabs(m_checkpoints[static_cast<size_t>(i) + 1].z - m_checkpoints[i].z) / 2);
		if (size.x == 0.f)
			size.x += .5f;
		if (size.z == 0.f)
			size.z += .5f;
		engine::ref<engine::cuboid> path_shape = engine::cuboid::create(size, false, true);
		//engine::ref<engine::texture_2d> path_texture = engine::texture_2d::create("assets/textures/path.png", true);
		engine::game_object_properties path_props;
		path_props.position = m_pp_positions[i];
		path_props.meshes = { path_shape->mesh() };
		path_props.scale = glm::vec3(1);
		//path_props.textures = { path_texture };
		path_props.restitution = 0.92f;
		engine::ref<engine::game_object> path_piece = engine::game_object::create(path_props);
		m_path.push_back(path_piece);
	}
}

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

//figure out why it crashes upon pressing n and also why nothing renders

void example_layer::new_wave()
{
	if (m_wave_number % 2 == 0)
	{
		for (int i = 0; i < m_wave_number / 2; ++i)
		{
			m_active_enemies.push_back(enemy::create(m_claptrap_props, 100.f, 5.f, 1.f, 1.f * i, enemy::e_type::CLAPTRAP));
		}
	}

	if (m_wave_number % 5 == 0)
	{
		for (int i = 0; i < m_wave_number / 5; ++i)
		{
			m_active_enemies.push_back(enemy::create(m_mech_props, 150.f, 20.f, 0.65f, 8.f * i, enemy::e_type::MECH));
		}
	}

	if (m_wave_number % 10 == 0)
	{
		for (int i = 0; i < m_wave_number / 10; ++i)
		{
			m_active_enemies.push_back(enemy::create(m_ironman_props, 300.f, 50.f, 0.4f, 5.f * i, enemy::e_type::IRONMAN));
		}
	}

	for (int i = 0; i < m_enemy_count; ++i)
	{	
		m_active_enemies.push_back(enemy::create(m_spider_props, 50.f, 5.f, 3.f, 1.f * i, enemy::e_type::SPIDER));
		std::cout << 5.f * i << "\n";
	}

	++m_wave_number;
	++m_enemy_count;
	std::cout << "actual lenght of enemies vec: " << m_active_enemies.size() << "\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HUD
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void example_layer::hud_on_render(engine::ref<engine::shader> shader)
{
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	//render score
	m_text_manager->render_text(text_shader, "Score: " + std::to_string((int) m_player.score()), (float) engine::application::window().width() / 2.f - 90.f, (float)engine::application::window().height() - 40.f, .75f, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
	//render health
	m_text_manager->render_text(text_shader, std::to_string((int) m_player.health()), 135.f, (float) engine::application::window().height() - 65.f, .55f, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));

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
	if (m_player.health() > 0)
		health_transform = glm::scale(health_transform, glm::vec3(0.01f * m_player.health() + 0.3f));
	else
		health_transform = glm::scale(health_transform, glm::vec3(0.3f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_health_txt2d->bind();
	engine::renderer::submit(shader, m_health_quad->mesh(), health_transform);

	engine::renderer::end_scene();
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
}
