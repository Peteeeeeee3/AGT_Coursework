#include "toygun.h"

toygun::toygun(const engine::game_object_properties props) : tower(props)
{
	damage = 10.f;
	attack_speed = 10.f;
	range = 10.f;
}

toygun::~toygun() {}

void toygun::init()
{
	
}

void toygun::update()
{
	
}

void toygun::rotate(bool rotate_left, float dt)
{
	//check for correct movement
	if (position().y >= 3.5f)
	{
		movingUp = false;
	}
	else if (position().y <= 2.5f)
	{
		movingUp = true;
	}

	//adjust position
	if (movingUp)
	{
		set_position(position() + glm::vec3(0.f, m_mSpeed * dt, 0.f));
	}
	else
	{
		set_position(position() - glm::vec3(0.f, m_mSpeed * dt, 0.f));
	}

	//rotate
	if (rotate_left)
	{
		set_rotation_amount(rotation_amount() + dt * 1.5f);
	}
	else
	{
		set_rotation_amount(rotation_amount() - dt * 1.5f);
	}
}

void toygun::attack()
{
	
}

engine::ref<toygun> toygun::create(const engine::game_object_properties& props)
{
	return std::make_shared<toygun>(props);
}
