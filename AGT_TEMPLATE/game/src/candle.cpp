#include "candle.h"

candle::candle(const engine::game_object_properties props) : tower(props)
{
	glm::vec3 position = props.position;
}

candle::~candle() {}

void candle::init()
{

}

void candle::update()
{

}

void candle::attack()
{

}

engine::ref<candle> candle::create(const engine::game_object_properties& props)
{
	return std::make_shared<candle>(props);
}
