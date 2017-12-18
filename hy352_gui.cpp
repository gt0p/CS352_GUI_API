/*
Course: CS-352
last update: 16122017
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "allegro5/allegro_image.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <string>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <cmath> // std::abs
#include <math.h>

#include <vector>
#include <string>
#include "./hy352_gui.h"

#ifdef _WIN32
#include <io.h>
#include <Windows.h>
#define Sleep(x) Sleep(x * 1000)
#elif defined __unix__
#include <unistd.h>
#define Sleep sleep
#elif defined __APPLE__ //**
#include <unistd.h>
#include <thread>
#include <chrono>
#define Sleep(X) this_thread::sleep_for(chrono::milliseconds(X * 1000));
#endif

#define DELAY_PER_REDRAW 10000

using namespace ::std;

Color_pallete SCREEN_COLOR;
Color_pallete PEN_COLOR;

const int SCREEN_W = 1024;
const int SCREEN_H = 720;
const float PIXELS_PER_STEP = 0.005;
const int DELAY_BEFORE_TERMINATION = 6;
const int IMAGE_W = 30;
const int IMAGE_H = 30;

float PEN_THICKNESS = 4.0;
bool IS_PEN_DOWN = true;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_BITMAP *image = NULL;
ALLEGRO_FONT *TEXT_FONT = NULL;

float turtle_x = 0, turtle_y = 0;
float old_turtle_x = 0, old_turtle_y = 0;
unsigned angle = 0;
unsigned program_counter = 0;
vector<Shape *> path;
char *last_debug_msg = NULL;

int dcounter = 0;

void redraw()
{
	++dcounter;
	if (dcounter == DELAY_PER_REDRAW)
	{
		if (dcounter == DELAY_PER_REDRAW)
			dcounter = 0;
		unsigned int i = 1;
		int x1, x2, y1, y2;
		int cx, cy, radius;

		al_clear_to_color(al_map_rgb(SCREEN_COLOR.red, SCREEN_COLOR.green, SCREEN_COLOR.blue));

		for (i = 0; i < path.size(); i++)
		{
			if (path[i]->getName() == "Line")
			{
				Line *l = (Line *)path[i];
				x1 = l->start.x;
				x2 = l->end.x;
				y1 = l->start.y;
				y2 = l->end.y;
				if (l->draw == true)
					al_draw_line(x1, y1, x2, y2, al_map_rgb(l->color.red, l->color.green, l->color.blue), l->thickness);
			}
			else if (path[i]->getName() == "Circle")
			{
				Circle *c = (Circle *)path[i];
				cx = c->center.x;
				cy = c->center.y;
				radius = c->radius;
				al_draw_circle(cx, cy, radius, al_map_rgb(c->color.red, c->color.green, c->color.blue), c->thickness);
			}
			else if (path[i]->getName() == "Label")
			{
				Label *t = (Label *)path[i];
				al_draw_text(TEXT_FONT, al_map_rgb(t->color.red, t->color.green, t->color.blue), t->point.x, t->point.y, ALLEGRO_ALIGN_CENTRE, t->text);
			}
		}

		al_draw_rotated_bitmap(image, IMAGE_W / 2, IMAGE_H / 2, turtle_x, turtle_y, degreesToRadians(angle), 0);

		if (last_debug_msg)
			al_draw_textf(TEXT_FONT, al_map_rgb(255, 255, 255), 10, SCREEN_H - 20, ALLEGRO_ALIGN_LEFT, "Debug: %s", last_debug_msg);

		al_flip_display();
	}
	old_turtle_x = turtle_x;
	old_turtle_y = turtle_y;
	++program_counter;
}

void redraw_now()
{
	dcounter = DELAY_PER_REDRAW - 1;
	redraw();
}

void calc_new_pos(bool forward)
{
	if (angle == 0)
	{
		if (forward)
			turtle_y -= PIXELS_PER_STEP;
		else
			turtle_y += PIXELS_PER_STEP;
	}
	else if (angle > 0 && angle < 90)
	{
		if (forward)
		{
			turtle_y -= cos(degreesToRadians(angle)) * PIXELS_PER_STEP;
			turtle_x += sin(degreesToRadians(angle)) * PIXELS_PER_STEP;
		}
		else
		{
			turtle_y += cos(degreesToRadians(angle)) * PIXELS_PER_STEP;
			turtle_x -= sin(degreesToRadians(angle)) * PIXELS_PER_STEP;
		}
	}
	else if (angle == 90)
	{
		if (forward)
			turtle_x += PIXELS_PER_STEP;
		else
			turtle_x -= PIXELS_PER_STEP;
	}
	else if (angle > 90 && angle < 180)
	{
		if (forward)
		{
			turtle_y += -cos(degreesToRadians(angle)) * PIXELS_PER_STEP;
			turtle_x += sin(degreesToRadians(angle)) * PIXELS_PER_STEP;
		}
		else
		{
			turtle_y -= -cos(degreesToRadians(angle)) * PIXELS_PER_STEP;
			turtle_x -= sin(degreesToRadians(angle)) * PIXELS_PER_STEP;
		}
	}
	else if (angle == 180)
	{
		if (forward)
			turtle_y += PIXELS_PER_STEP;
		else
			turtle_y -= PIXELS_PER_STEP;
	}
	else if (angle > 180 && angle < 270)
	{
		if (forward)
		{
			turtle_y += -cos(degreesToRadians(angle)) * PIXELS_PER_STEP;
			turtle_x += sin(degreesToRadians(angle)) * PIXELS_PER_STEP;
		}
		else
		{
			turtle_y -= -cos(degreesToRadians(angle)) * PIXELS_PER_STEP;
			turtle_x -= sin(degreesToRadians(angle)) * PIXELS_PER_STEP;
		}
	}
	else if (angle == 270)
	{
		if (forward)
			turtle_x -= PIXELS_PER_STEP;
		else
			turtle_x += PIXELS_PER_STEP;
	}
	else if (angle > 270 && angle < 360)
	{
		if (forward)
		{
			turtle_y -= cos(degreesToRadians(angle)) * PIXELS_PER_STEP;
			turtle_x += sin(degreesToRadians(angle)) * PIXELS_PER_STEP;
		}
		else
		{
			turtle_y += cos(degreesToRadians(angle)) * PIXELS_PER_STEP;
			turtle_x -= sin(degreesToRadians(angle)) * PIXELS_PER_STEP;
		}
	}
}

int turtle_rotate(int degrees)
{

	if (degrees < 0)
		degrees = 360 - (std::abs(degrees) % 360);
	else
		degrees = degrees % 360;

	angle = (angle + degrees) % 360;

	redraw();

	return 1;
}

int turtle_draw_circle(unsigned radius)
{

	Point p;

	p.x = turtle_x;
	p.y = turtle_y;

	Circle *c = new Circle();
	c->center = p;
	c->radius = radius;
	c->color = PEN_COLOR;
	c->thickness = PEN_THICKNESS;

	path.push_back(c);
	redraw_now();

	return 1;
}

int turtle_mv_forward(float steps)
{
	int i = 0;
	for (i = 0; i < (int)(steps * 100); ++i)
	{

		calc_new_pos(true);

		Point s, e;

		s.x = old_turtle_x;
		s.y = old_turtle_y;

		e.x = turtle_x;
		e.y = turtle_y;

		Line *l = new Line();
		l->draw = IS_PEN_DOWN;
		l->start = s;
		l->end = e;
		l->color = PEN_COLOR;
		l->thickness = PEN_THICKNESS;

		path.push_back(l);
		if (i % 100 == 0)
			redraw();
	}

	return 1;
}

int turtle_mv_backward(float steps)
{
	int i = 0;
	for (i = 0; i < (int)(steps * 100); ++i)
	{

		calc_new_pos(false);

		Point s, e;

		s.x = old_turtle_x;
		s.y = old_turtle_y;

		e.x = turtle_x;
		e.y = turtle_y;

		Line *l = new Line();
		l->draw = true;
		l->start = s;
		l->end = e;
		l->color = PEN_COLOR;
		l->thickness = PEN_THICKNESS;

		path.push_back(l);
		if (i % 100 == 0)
			redraw();
	}
	return 1;
}

int set_pen_color(unsigned r, unsigned g, unsigned b)
{
	if ((r < 0 || r > 255) || (g < 0 || g > 255) || (b < 0 || b > 255))
	{
		fprintf(stderr, "(r,g,b) values must be between 0 and 255");
		return -1;
	}
	PEN_COLOR.red = r;
	PEN_COLOR.green = g;
	PEN_COLOR.blue = b;

	return 1;
}

int set_screen_color(unsigned r, unsigned g, unsigned b)
{

	if ((r < 0 || r > 255) || (g < 0 || g > 255) || (b < 0 || b > 255))
	{
		fprintf(stderr, "(r,g,b) values must be between 0 and 255");
		return -1;
	}

	SCREEN_COLOR.red = r;
	SCREEN_COLOR.green = g;
	SCREEN_COLOR.blue = b;

	redraw_now();

	return 1;
}

int set_pen_thickness(float thickness)
{
	PEN_THICKNESS = thickness;
	return 1;
}

void pen_up()
{
	IS_PEN_DOWN = false;
}

void pen_down()
{
	IS_PEN_DOWN = true;
}

int turtle_go_to_position(unsigned x, unsigned y)
{

	if (x < 0 || y > SCREEN_W || y < 0 || y > SCREEN_H)
	{
		fprintf(stderr, "set_xy: Coordinates x,y should be greater to 0 and less than window's dimentions!\n");
		return -1;
	}

	turtle_x = x;
	turtle_y = y;

	Point s, e;

	s.x = old_turtle_x;
	s.y = old_turtle_y;

	e.x = turtle_x;
	e.y = turtle_y;

	Line *l = new Line();
	l->draw = IS_PEN_DOWN;
	l->start = s;
	l->end = e;
	l->color = PEN_COLOR;
	l->thickness = PEN_THICKNESS;

	path.push_back(l);

	redraw_now();
	return 1;
}

int turtle_go_to_center()
{
	int status = turtle_go_to_position(SCREEN_W / 2.0 - IMAGE_W / 2, SCREEN_H / 2.0 - IMAGE_H / 2);
	if (status == -1)
	{
		fprintf(stderr, "turtle_go_to_center: Cannot move turtle to the center!\n");
		return -1;
	}
	else
		return 1;
}

int turtle_draw_label(char const *text)
{

	if (!text)
	{
		fprintf(stderr, "turtle_draw_label: Cannot draw text %s!\n", text);
		return -1;
	}

	Label *t = new Label();

	t->point.x = turtle_x;
	t->point.y = turtle_y;

	t->draw = IS_PEN_DOWN;
	t->color = PEN_COLOR;
	t->text = text;
	path.push_back(t);

	redraw_now();
	return 1;
}

int init_GUI()
{
	if (!al_init())
	{
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	if (!al_init_image_addon())
	{
		fprintf(stderr, "failed to initialize allegro image addon!\n");
		return -1;
	}

	if (!al_init_primitives_addon())
	{
		fprintf(stderr, "failed to initialize allegro primitives addon!\n");
		return -1;
	}

	// initialize the font addon
	if (!al_init_font_addon())
	{
		fprintf(stderr, "failed to initialize allegro font addon!\n");
		return -1;
	};

	// initialize the ttf (True Type Font) addon
	if (!al_init_ttf_addon())
	{
		fprintf(stderr, "failed to initialize allegro ttf addon!\n");
		return -1;
	};

	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display)
	{
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

	image = al_load_bitmap("turtle.png");
	if (!image)
	{
		fprintf(stderr, "failed to load image turtle.png!\n");
		al_destroy_display(display);
		return 0;
	}

	ALLEGRO_FONT *font = al_load_ttf_font("pirulen rg.ttf", 16, 0);
	if (!font)
	{
		fprintf(stderr, "Could not load 'pirulen.ttf'.\n");
		return -1;
	}

	event_queue = al_create_event_queue();
	if (!event_queue)
	{
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));

	// Init pen color to white
	PEN_COLOR.red = 255;
	PEN_COLOR.green = 255;
	PEN_COLOR.blue = 255;

	// Init screen color to white
	SCREEN_COLOR.red = 0;
	SCREEN_COLOR.green = 0;
	SCREEN_COLOR.blue = 0;

	TEXT_FONT = font;
	// Init pen thickness to white
	PEN_THICKNESS = 2;

	// Init turtle position to center
	turtle_x = SCREEN_W / 2.0 - IMAGE_W / 2;
	turtle_y = SCREEN_H / 2.0 - IMAGE_H / 2;

	old_turtle_x = turtle_x;
	old_turtle_y = turtle_y;

	al_clear_to_color(al_map_rgb(SCREEN_COLOR.red, SCREEN_COLOR.green, SCREEN_COLOR.blue));
	al_draw_rotated_bitmap(image, IMAGE_W / 2, IMAGE_H / 2, turtle_x, turtle_y, angle, 0);
	al_flip_display();

	return 1;
}

void destroy_GUI()
{
	redraw_now();
	al_rest(10);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
}

// EXTRA METHODS

int wait(unsigned s)
{
	if (s < 0)
	{
		fprintf(stderr, "Delay per command must be greater than zero\n");
		return -1;
	}
	Sleep(s);
}

int show_debug_message(char const *msg)
{
	if (!msg)
	{
		fprintf(stderr, "failed to create event_queue!\n");
		return -1;
	}

	last_debug_msg = (char *)msg;
	redraw();
	return 1;
}