#include "enemy.h"
#include "player.h"

enemy::enemy(const engine::game_object_properties& props, float health, float strength, float speed, float spawn_time, e_type type) :
	engine::game_object(props), m_health(health), m_strength(strength), m_speed(speed), m_spawn_time(spawn_time), m_type(type)
{
	m_bounding_box = engine::bounding_box::bounding_box();
	m_bounding_box.set_box(props.bounding_shape.x * props.scale.x, props.bounding_shape.y * props.scale.y, props.bounding_shape.z * props.bounding_shape.z, position());
}

void enemy::update(player& player, std::vector<glm::vec3> checkpoints, float dt)
{
	m_life_time += dt;

	m_bounding_box.on_update(glm::vec3(position().x, position().y, position().z));

	if (m_health <= 0)
		enemy::~enemy();

	if (toRender())
	{
		if (!m_isStunned)
		{
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
				break;
			}
		}
		else
		{
			std::cout << m_stun_timer << " " << m_stun_duration << "\n";
			if (m_stun_timer >= m_stun_duration)
			{
				m_isStunned = false;
				m_stun_timer = 0.f;
			}
			else
				m_stun_timer += dt;
		}
	}
}

engine::ref<enemy> enemy::create(const engine::game_object_properties& props, float health, float strength, float speed, float spawn_time, e_type type)
{
	return std::make_shared<enemy>(props, health, strength, speed, spawn_time, type);
}
