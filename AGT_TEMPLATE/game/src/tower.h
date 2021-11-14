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
	void init();
	void update(bool rotate_left, float dt);

	static engine::ref<tower> create(const engine::game_object_properties& props);

private:
	bool movingUp = true;
	float m_mSpeed = 0.25f;
};
