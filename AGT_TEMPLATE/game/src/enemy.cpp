#include "enemy.h"
#include "player.h"

enemy::enemy(const engine::game_object_properties& props, float health, float strength, float speed, e_type type) :
	engine::game_object(props), m_health(health), m_strength(strength), m_speed(speed), m_type(type)
{

}

void enemy::update(player& player, std::vector<glm::vec3> checkpoints, float dt)
{
	if (m_health <= 0)
		enemy::~enemy();

	switch (m_location_state)
	{
	case START:
		set_rotation_amount(glm::pi<float>() / 2);
		if (m_type == SPIDER)
			set_rotation_amount(-rotation_amount());
		set_position(position() + glm::normalize(checkpoints[0] - position()) * dt * m_speed);
		if (position().x >= checkpoints[0].x && position().z == checkpoints[0].z)
			m_location_state = ONE;
		break;
	case ONE:
		set_rotation_amount(glm::pi<float>() / 2);
		if (m_type == SPIDER)
			set_rotation_amount(-rotation_amount());
		set_position(position() + glm::normalize(checkpoints[1] - position()) * dt * m_speed);
		if (position().x >= checkpoints[1].x && position().z == checkpoints[1].z)
			m_location_state = TWO;
		break;
	case TWO:
		set_rotation_amount(glm::pi<float>());
		if (m_type == SPIDER)
			set_rotation_amount(0.f);
		set_position(position() + glm::normalize(checkpoints[2] - position()) * dt * m_speed);
		if (position().z <= checkpoints[2].z)
			m_location_state = THREE;
		break;
	case THREE:
		set_rotation_amount(glm::pi<float>() / 2);
		if (m_type == SPIDER)
			set_rotation_amount(-rotation_amount());
		set_position(position() + glm::normalize(checkpoints[3] - position()) * dt * m_speed);
		if (position().x >= checkpoints[3].x)
			m_location_state = FOUR;
		break;
	case FOUR:
		set_rotation_amount(0.f);
		if (m_type == SPIDER)
			set_rotation_amount(glm::pi<float>());
		set_position(position() + glm::normalize(checkpoints[4] - position()) * dt * m_speed);
		if (position().z >= checkpoints[4].z)
			m_location_state = FIVE;
		break;
	case FIVE:
		set_rotation_amount(glm::pi<float>() / 2);
		if (m_type == SPIDER)
			set_rotation_amount(-rotation_amount());
		set_position(position() + glm::normalize(checkpoints[5] - position()) * dt * m_speed);
		if (position().x >= checkpoints[5].x)
			m_location_state = SIX;
		break;
	case SIX:
		set_rotation_amount(glm::pi<float>());
		if (m_type == SPIDER)
			set_rotation_amount(0.f);
		set_position(position() + glm::normalize(checkpoints[6] - position()) * dt * m_speed);
		if (position().z <= checkpoints[6].z)
			m_location_state = SEVEN;
		break;
	case SEVEN:
		set_rotation_amount(glm::pi<float>() / 2);
		if (m_type == SPIDER)
			set_rotation_amount(-rotation_amount());
		set_position(position() + glm::normalize(checkpoints[7] - position()) * dt * m_speed);
		if (position().x >= checkpoints[7].x)
			m_location_state = FINISH;
		break;
	case FINISH:
		player.damage(m_strength);
		m_isDead = true;
		this->~enemy();
	}
}

engine::ref<enemy> enemy::create(const engine::game_object_properties& props, float health, float strength, float speed, e_type type)
{
	return std::make_shared<enemy>(props, health, strength, speed, type);
}
