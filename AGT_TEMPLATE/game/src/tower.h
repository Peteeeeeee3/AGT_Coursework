#pragma once
#include <engine.h>

// the current state of this class is only temporary
// this class will be redeveloped to support towers in general
// in this current build it is mostly a place holder as it only handles the rotation of the menu guns

class tower : public engine::game_object
{
public:
	tower(const engine::game_object_properties props);
	~tower();
	virtual void init();
	virtual void update();
	virtual void attack();

	static engine::ref<tower> create(const engine::game_object_properties& props);

protected:
	std::vector<engine::game_object> enemies_in_range;
	float damage;
	float attack_speed;
	float range;
};
