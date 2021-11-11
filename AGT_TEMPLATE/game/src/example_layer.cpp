#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"
#include "player.h"

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())


{
    // Hide the mouse and lock it inside the window
    //engine::input::anchor_mouse(true);
    engine::application::window().hide_mouse_cursor();

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

	/**m_mannequin_material = engine::material::create(1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);**/


	// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	/**m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/SkyboxFront.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxRight.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBack.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxLeft.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxTop.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBottom.bmp", true)
		});**/

	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/posz.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/posx.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/negz.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/negx.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/posy.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/negy.jpg", true)
		});

	/**engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/mannequin/free3Dmodel.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/walking.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/idle.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/jump.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/standard_run.dae");
	m_skinned_mesh->switch_root_movement(false);**/

	/**engine::game_object_properties mannequin_props;
	mannequin_props.animated_mesh = m_skinned_mesh;
	mannequin_props.scale = glm::vec3(1.f/ glm::max(m_skinned_mesh->size().x, glm::max(m_skinned_mesh->size().y, m_skinned_mesh->size().z)));
	mannequin_props.position = glm::vec3(3.0f, 0.5f, -5.0f);
	mannequin_props.type = 0;
	mannequin_props.bounding_shape = m_skinned_mesh->size() / 2.f * mannequin_props.scale.x;
	m_mannequin = engine::game_object::create(mannequin_props);**/

	m_player = player(/**m_3d_camera**/);

	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/Wood.jpg", false) };
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
			//std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/terrain.bmp", false) };
			engine::ref<engine::terrain> terrain_shape = engine::terrain::create(0.5f, 0.1f, 0.5f);
			engine::game_object_properties terrain_props;
			terrain_props.meshes = { terrain_shape->mesh() };
			terrain_props.textures = terrain_textures;
			terrain_props.is_static = true;
			terrain_props.type = 0;
			terrain_props.bounding_shape = glm::vec3(0.5f, 0.1f, 0.5f);
			terrain_props.restitution = 0.92f;
			m_terrain.push_back(engine::game_object::create(terrain_props));
			m_terrain[m_terrain.size() - 1]->set_position(glm::vec3(i, 0.0f, j));
		}
	}

	/**engine::ref<engine::model> gchair_model = engine::model::create("assets/models/static/G-Chair/Cadeira.obj");
	engine::game_object_properties gchair_props;
	gchair_props.meshes = gchair_model->meshes();
	gchair_props.textures = gchair_model->textures();
	float gchair_scale = 0.0125f;
	gchair_props.position = { 0.f, 0.5f, 0.f };
	gchair_props.scale = glm::vec3(gchair_scale);
	gchair_props.bounding_shape = gchair_model->size() / 2.f *gchair_scale;
	m_gchair = engine::game_object::create(gchair_props);**/

	engine::ref<engine::model> kraken_model = engine::model::create("assets/models/static/Kraken/Razer kraken.obj");
	engine::game_object_properties kraken_props;
	kraken_props.meshes = kraken_model->meshes();
	kraken_props.textures = kraken_model->textures();
	float kraken_scale = 1.f;
	kraken_props.position = { 0.f, 0.5f, 0.f };
	kraken_props.scale = glm::vec3(kraken_scale);
	kraken_props.bounding_shape = kraken_model->size() / 2.f * kraken_scale;
	m_kraken = engine::game_object::create(kraken_props);

	/**engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 5.f, -5.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	m_ball = engine::game_object::create(sphere_props);**/

	for (auto section : m_terrain) {
		m_game_objects.push_back(section);
	}
	//m_game_objects.push_back(m_ball);
	//m_game_objects.push_back(m_cow);
	//m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_pickup);
	//m_game_objects.push_back(m_kraken);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	//m_skinned_mesh->switch_animation(1);

	m_3d_camera.set_view_matrix(glm::vec3(0.f, 10.f, 0.f), glm::vec3(-5.f, 0.f, -5.f));
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{
    m_3d_camera.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	//m_mannequin->animated_mesh()->on_update(time_step);

	m_audio_manager->update_with_camera(m_3d_camera);

	//check_bounce();

	//m_player.update_camera(m_3d_camera, time_step);
} 

void example_layer::on_render() 
{
    engine::render_command::clear_color({0.2f, 0.3f, 0.3f, 1.0f}); 
    engine::render_command::clear();

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);

	for (auto section : m_terrain) {
		engine::renderer::submit(mesh_shader, section);
	}

	glm::mat4 gchair_transform(1.f);
	gchair_transform = glm::translate(gchair_transform, m_gchair->position());
	gchair_transform = glm::scale(gchair_transform, m_gchair->scale());
	//engine::renderer::submit(mesh_shader, gchair_transform, m_gchair);

	glm::mat4 kraken_transfrom(1.f);
	kraken_transfrom = glm::translate(gchair_transform, m_kraken->position());
	kraken_transfrom = glm::rotate(kraken_transfrom, glm::pi<float>() / 2, glm::vec3(0.f, 1.f, 0.f));
	kraken_transfrom = glm::scale(kraken_transfrom, m_kraken->scale());
	engine::renderer::submit(mesh_shader, kraken_transfrom, m_kraken);
	/**m_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_ball);**/

	/**m_mannequin_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_mannequin);**/

    engine::renderer::end_scene();

	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, "Orange Text", 10.f, (float)engine::application::window().height()-25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
} 

void example_layer::on_event(engine::event& event) 
{ 
    if(event.event_type() == engine::event_type_e::key_pressed) 
    { 
        auto& e = dynamic_cast<engine::key_pressed_event&>(event); 
        if(e.key_code() == engine::key_codes::KEY_TAB) 
        { 
            engine::render_command::toggle_wireframe();
        }
    } 
}

/**void example_layer::check_bounce()
{
	if (m_prev_sphere_y_vel < 0.1f && m_ball->velocity().y > 0.1f)
		//m_audio_manager->play("bounce");
		m_audio_manager->play_spatialised_sound("bounce", m_3d_camera.position(), glm::vec3(m_ball->position().x, 0.f, m_ball->position().z));
	m_prev_sphere_y_vel = m_game_objects.at(1)->velocity().y;
}**/
