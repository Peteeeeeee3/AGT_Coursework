#include "tower.h"

tower::tower(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies) : engine::game_object(props), m_active_enemies(enemies) {}

tower::~tower() {}

void tower::init() {}

void tower::update()
{
	
}

void tower::attack()
{

}

void tower::upgradeLeft_lvl1() {}
void tower::upgradeLeft_lvl2() {}
void tower::upgradeRight_lvl1() {}
void tower::upgradeRight_lvl2() {}

engine::ref<tower> tower::create(const engine::game_object_properties& props, std::vector<engine::ref<enemy>>& enemies)
{
	return std::make_shared<tower>(props, enemies);
}
