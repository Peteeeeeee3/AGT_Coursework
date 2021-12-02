#pragma once
#include <engine.h>
#include "tower.h"

class candle : public tower
{
public:
	candle(const engine::game_object_properties props);
	~candle();
	void init() override;
	void update() override;
	void attack() override;
	//engine::ref<engine::cylinder> body() { return m_body; }
	//engine::ref<engine::pentahedron> flame() { return m_flame; }

	static engine::ref<candle> create(const engine::game_object_properties& props);

private:
};
