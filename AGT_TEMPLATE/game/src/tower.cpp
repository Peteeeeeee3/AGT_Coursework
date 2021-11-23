#include "tower.h"

tower::tower(const engine::game_object_properties props) : engine::game_object(props) {}

tower::~tower() {}

void tower::init() {}

void tower::update()
{
	
}

void tower::attack()
{

}

engine::ref<tower> tower::create(const engine::game_object_properties& props)
{
	return std::make_shared<tower>(props);
}
