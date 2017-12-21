#include "./hy352_gui.h"
#ifdef APPLE
#include <allegro5/allegro.h> //**
#endif

int main(int argc, char **argv)
{
	init_GUI();

	set_screen_color(255, 255, 255);
	set_pen_color(0, 0, 0);
	set_pen_thickness(0.1f);

	for (int i = 0; i < 360; i++)
	{
		turtle_mv_backward(2);
		turtle_rotate(1);
	}

	pen_up();
	turtle_go_to_position(200, 200);
	pen_down();
	for (int i = 0; i < 4; i++)
	{
		turtle_mv_forward(100);
		turtle_rotate(90);
	}

	destroy_GUI();
	return 0;
}