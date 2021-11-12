#pragma once
#include <engine.h>

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
