#include "enemy.h"
#include "player.h"

enemy::enemy(const engine::game_object_properties& props, float health, float strength, float speed) :
	engine::game_object(props), m_health(health), m_strength(strength), m_speed(speed)
{

}

void enemy::update(player& player, std::vector<glm::vec3> checkpoints, float dt)
{
	if (m_health <= 0)
		enemy::~enemy();

	std::cout << position() << "\n" << m_location_state << "\n";

	if (m_location_state == START)
	{
		set_rotation_amount(glm::pi<float>() / 2);
		set_position(position() + glm::normalize(checkpoints[0] - position()) * dt * m_speed);
		if (position().x >= checkpoints[0].x && position().z == checkpoints[0].z)
			m_location_state = ONE;
	}
	else if (m_location_state == ONE)
	{
		set_rotation_amount(glm::pi<float>() / 2);
		set_position(position() + glm::normalize(checkpoints[1] - position()) * dt * m_speed);
		if (position().x >= checkpoints[1].x && position().z == checkpoints[1].z)
			m_location_state = TWO;
	}
	else if (m_location_state == TWO)
	{
		set_rotation_amount(glm::pi<float>());
		set_position(position() - glm::normalize(checkpoints[2] - position()) * dt * m_speed);
		if (position().x >= checkpoints[2].x && position().z <= checkpoints[2].z)
			m_location_state = THREE;
	}
	else if (m_location_state == THREE)
	{
		set_rotation_amount(glm::pi<float>() / 2);
		set_position(position() + glm::normalize(checkpoints[3] - position()) * dt * m_speed);
		if (position().x <= checkpoints[3].x && position().z >= checkpoints[3].z)
			m_location_state = FOUR;
	}
	else if (m_location_state == FOUR)
	{
		set_rotation_amount(0.f);
		set_position(position() + glm::normalize(checkpoints[4] - position()) * dt * m_speed);
		if (position().x >= checkpoints[4].x && position().z == checkpoints[4].z)
			m_location_state = FIVE;
	}
	else if (m_location_state == FIVE)
	{
		set_rotation_amount(glm::pi<float>() / 2);
		set_position(position() + glm::normalize(checkpoints[5] - position()) * dt * m_speed);
		if (position().x >= checkpoints[5].x && position().z == checkpoints[5].z)
			m_location_state = SIX;
	}
	else if (m_location_state == SIX)
	{
			set_rotation_amount(glm::pi<float>());
			set_position(position() + glm::normalize(checkpoints[6] - position()) * dt * m_speed);
			if (position().x >= checkpoints[6].x && position().z == checkpoints[6].z)
				m_location_state = SEVEN;
	}
	else if (m_location_state == SEVEN)
	{
			set_rotation_amount(glm::pi<float>() / 2);
			set_position(position() + glm::normalize(checkpoints[7] - position()) * dt * m_speed);
			if (position().x >= checkpoints[7].x && position().z == checkpoints[7].z)
				m_location_state = FINISH;
	}
	else if (m_location_state == FINISH)
	{
		player.damage(m_strength);
		enemy::~enemy();
	}
}

engine::ref<enemy> enemy::create(const engine::game_object_properties& props, float health, float strength, float speed)
{
	return std::make_shared<enemy>(props, health, strength, speed);
}
