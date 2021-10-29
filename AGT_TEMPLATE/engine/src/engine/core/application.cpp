#include "pch.h"
#include "application.h"
#include "engine/renderer/renderer.h"
#include "GLFW/glfw3.h"
#include "engine/utils/timer.h"

//----------------------------------------------------------------------------- 

engine::application* engine::application::s_instance{ nullptr }; 
bool engine::application::s_running{ true }; 
bool engine::application::s_minimized{ false }; 

//----------------------------------------------------------------------------- 

engine::application::application()
{
    CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    m_window = window::create();
    m_window->event_callback(BIND_EVENT_FN(application::on_event));

    renderer::init();
}

engine::application::~application()
{
}

void engine::application::run()
{
	engine::timer gameLoopTimer;
	gameLoopTimer.start();
	while (s_running)
	{
		timestep time_step = (float)gameLoopTimer.elapsed();
		gameLoopTimer.reset();
		for (auto* layer : m_layers_stack)
		{
			layer->on_update(time_step);
			if (!application::s_minimized)
				layer->on_render();
		}
		m_window->on_update();
	}
}

void engine::application::on_event(event& event) 
{ 
    event_dispatcher dispatcher(event); 
    // dispatch event on window X pressed 
    dispatcher.dispatch<window_closed_event>(BIND_EVENT_FN(application::on_window_close)); 
    dispatcher.dispatch<window_resize_event>(BIND_EVENT_FN(application::on_window_resized)); 

    //LOG_CORE_TRACE("{0}", event); 

    // events are executed from top of the stack to bottom (aka end to start of the list) 
    for (auto it = m_layers_stack.end(); it != m_layers_stack.begin(); ) 
    { 
        (*--it)->on_event(event); 
        // stop event propagation to next layer if flagged as handled 
        if (event.handled) 
            break; 
    } 
} 

void engine::application::push_layer(layer* layer) 
{ 
    m_layers_stack.push_layer(layer); 
} 

void engine::application::push_overlay(layer* overlay) 
{ 
    m_layers_stack.push_overlay(overlay); 
} 

bool engine::application::on_window_close(window_closed_event&) 
{ 
    exit(); 
    const bool event_handled = false;
    return event_handled; 
} 

bool engine::application::on_window_resized(window_resize_event &e) 
{ 
    if(e.height() == 0 || e.width() == 0)
    {
        application::s_minimized = true;
        return false;
    }
    application::s_minimized = false;

    render_command::resize_viewport(0, 0, e.width(), e.height());

    const bool event_handled = false;
    return event_handled; 
} 

void engine::application::exit() 
{ 
    s_running = false; 
} 
