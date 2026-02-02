#ifndef _GAME_H_
#define _GAME_H_

#include <SDL.h>

enum Players { PLAYER_1, PLAYER_2 };

struct Player {
	Players player;
	int score;
};

struct Object {
	float x, y;
	float dx, dy;
	int w, h;
};

class Game {
public:
	void init(const char* title, int w, int h, bool fullscreen);

	void handle_events();
	void update(float dt);
	void render();
	void clean();

	void render_scoreboard();
	void draw_digit(int num, int x, int y);
	void drawer_loop(const int num[5][3], int x, int y);
	void launch_ball(float power, int mode);
	bool crossed_half_field();
	bool touched_net();
	void game_over_check();
	void end_the_round();


	bool running() { return is_running; };

private:
	Player Player1 = {PLAYER_1, 0}; // initate both with their corresponding enums
	Player Player2 = { PLAYER_2, 0 };
	Players round_winner;
	Players winner;
	Players next_starter = PLAYER_2;
	Players current_turn = PLAYER_1;
	bool is_running = false;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	Object ball;
	bool is_charging = false;
	float power = 0.0f;
	bool half_field = false;
	bool turn_end = false;
	bool game_over = false;
	bool ready_to_serve = true;
	int touch_counter = NULL;
	int shot_mode = NULL;
	bool on_p1_side;
	bool on_p2_side;

	const float MAX_POWER = 140.0f;
	const float GRAVITY = 600.0f;
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
};

#endif // !_GAME_H_