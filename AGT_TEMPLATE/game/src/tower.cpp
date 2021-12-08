#include "tower.h"

tower::tower(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : engine::game_object(props), m_active_enemies(enemies)
{
	m_bounding_box = engine::bounding_box::bounding_box();
	m_bounding_box.set_box(props.bounding_shape.x * props.scale.x, props.bounding_shape.y * props.scale.y, props.bounding_shape.z * props.bounding_shape.z, position());
}

tower::~tower() {}

void tower::init() {}

void tower::update(std::vector<engine::ref<enemy>> enemies, float dt)
{
	m_bounding_box.on_update(glm::vec3(position().x, position().y, position().z));
}

void tower::attack()
{

}

void tower::init_range()
{
	m_range_material = engine::material::create(0.0f, glm::vec3(0.2f, 0.f, 0.f),
		glm::vec3(0.0f, 0.f, 0.f), glm::vec3(0.5f, 0.5f, 0.5f), .5f);
	engine::ref<engine::circle> circle_shape = engine::circle::create(glm::vec3(0.f, 0.15f, 0.f) , 300, m_range);
	engine::game_object_properties circle_props;
	circle_props.position = position();
	circle_props.meshes = { circle_shape->mesh() };
	m_range_highlight = engine::game_object::create(circle_props);
}

void tower::render_range(engine::ref<engine::shader> shader)
{
	m_range_material->submit(shader);
	engine::renderer::submit(shader, m_range_highlight);
}

void tower::upgradeLeft_lvl1() {}
void tower::upgradeLeft_lvl2() {}
void tower::upgradeRight_lvl1() {}
void tower::upgradeRight_lvl2() {}

engine::ref<tower> tower::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<tower>(props, enemies);
}
