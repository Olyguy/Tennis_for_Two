#include "Game.h"
#include <iostream>
#include <iomanip>

// main rectangles
SDL_Rect ball_rect;
SDL_Rect ground;
SDL_Rect net;

void Game::init(const char* title, int w, int h, bool fullscreen) {
	int flags = 0;
	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystems Initialized" << std::endl;

		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
		if (window) {
			std::cout << "Window created..." << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			std::cout << "Renderer created..." << std::endl;
		}
		is_running = true;
	}

	ball = { SCREEN_WIDTH / 1.15f, SCREEN_HEIGHT / 3.0f, 0, -350, 10, 10 };// the starting pos is on the right side (PLAYER 1)
	// set the rectangles
	ball_rect = { (int)ball.x, (int)ball.y, ball.w, ball.h };	
	ground = { 20, (int)(SCREEN_HEIGHT / 2.0f + 100), SCREEN_WIDTH -40, 10 };	
	net = { (int)(SCREEN_WIDTH / 2), (int)(SCREEN_HEIGHT / 2.0f + 40), 10, 55 };
}

void Game::handle_events() {
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT)
			is_running = false;

		if (ev.type == SDL_KEYDOWN && ev.key.repeat == 0) {
			//checkers to know which side of the field the ball is
			on_p1_side = (ball.x > SCREEN_WIDTH / 2.0f);
			on_p2_side = (ball.x < SCREEN_WIDTH / 2.0f);
			if (ev.key.keysym.sym == SDLK_SPACE) {
				if (game_over)
					return;
				
				//use SPACE to restart
				if (turn_end) {
					//FOR DEBUGGING
					//std::cout << "Player 1: "<<Player1.score << std::endl;
					//std::cout << "Player 2: "<<Player2.score << std::endl;
					
					//switch the starter 
					//then set the initial conditions every round
					if (next_starter == PLAYER_1) {
						ball.x = SCREEN_WIDTH *0.85f;
						next_starter = PLAYER_2;
						current_turn = PLAYER_1;
					}
					else {
						ball.x = SCREEN_WIDTH *0.15f;
						next_starter = PLAYER_1;
						current_turn = PLAYER_2;
					}
					ball.y = SCREEN_HEIGHT / 3.0f;
					ball.dx = 0;
					ball.dy = -350;

					turn_end = false;
					ready_to_serve = true;
					power = 0.0f;
				}
				//use SPACE to serve (shot_mode 0)
				else if (ready_to_serve) {
					is_charging = true;
					power = 0.0f;
					shot_mode = 0;
				}
				//use SPACE to launch with forehand (shot_mode 0)
				else if (!is_charging) {
					//PLAYER 1 can hit if the ball is on the right
					if (current_turn == PLAYER_1 && on_p1_side) {
						is_charging = true;
						power = 0.0f;
						shot_mode = 0;
					}
					//PLAYER 2 can hit if the ball is on the left
					else if (current_turn == PLAYER_2 && on_p2_side) {
						is_charging = true;
						power = 0.0f;
						shot_mode = 0;
					}
				}
			}

			//use B to lob (shot_mode 1)
			if (ev.key.keysym.sym == SDLK_b) {
				if (game_over)
					return;
				if (!turn_end) {
					if (ready_to_serve) {
						is_charging = true;
						power = 0.0f;
						shot_mode = 1;
					}
					else if (!is_charging) {
						if (current_turn == PLAYER_1 && on_p1_side) {
							is_charging = true;
							power = 0.0f;
							shot_mode = 1;
						}
						else if (current_turn == PLAYER_2 && on_p2_side) {
							is_charging = true;
							power = 0.0f;
							shot_mode = 1;
						}
					}
				}
			}

			//use R to hard reset
			if (ev.key.keysym.sym == SDLK_r) {
				Player1.score = 0;
				Player2.score = 0;
				game_over = false;
				turn_end = true;
			}

			if (ev.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
			}
		}

		if (ev.type == SDL_KEYUP) {
			if (ev.key.keysym.sym == SDLK_SPACE && is_charging) {
				is_charging = false;
				ready_to_serve = false;
				touch_counter = 0;
				launch_ball(power, shot_mode);
			}
			else if (ev.key.keysym.sym == SDLK_b && is_charging) {
				is_charging = false;
				ready_to_serve = false;
				touch_counter = 0;
				launch_ball(power, shot_mode);
			}
		}
	}
}

void Game::launch_ball(float power, int mode) {
	if (mode == 0) { // forehand
		if (current_turn == PLAYER_1) {
			ball.dx = -power * 4.3f;
			current_turn = PLAYER_2;
		}
		else {
			ball.dx = power * 4.3f;
			current_turn = PLAYER_1;
		}
		ball.dy = -power * 0.9f;
	}
	else {	// lob
		if (current_turn == PLAYER_1) {
			ball.dx = -power * 2.79f;
			current_turn = PLAYER_2;
		}
		else {
			ball.dx = power * 2.79f;
			current_turn = PLAYER_1;
		}
		ball.dy = -power * 3.9f;
	}
	half_field = false;
}

// function that checks if the ball crossed half the field
bool Game::crossed_half_field() {
	// return false if it is a serve
	if (ready_to_serve) {
		half_field = false;
		return half_field;
	}

	if (current_turn == PLAYER_1 && ball.x > SCREEN_WIDTH / 2.0f) {
		half_field = true;
	}
	else if (current_turn == PLAYER_2 && ball.x < SCREEN_WIDTH / 2.0f) {
		half_field = true;
	}

	return half_field;
}

// see if the ball intersects with the net
bool Game::touched_net() {
	return SDL_HasIntersection(&ball_rect, &net);
}

// helper function to set the end of the round
void Game::end_the_round() {
	ball.dx = 0;
	ball.dy = 0;
	turn_end = true;
	is_charging = false;
	half_field = false;
	touch_counter = 0;
}

// helper function that checks if the game ended
void Game::game_over_check() {
	if (Player1.score == 5) {
		winner = PLAYER_1;
		game_over = true;
	}
	else if (Player2.score == 5) {
		winner = PLAYER_2;
		game_over = true;
	}
}

void Game::update(float delta_time) {
	//FOR DEBUGGING
	/*if (current_turn == PLAYER_1) {
		std::cout << "player 1" << std::endl;
	}
	else {
		std::cout << "player 2" << std::endl;
	}
	std::cout << touch_counter << std::endl;
	std::cout << std::boolalpha <<"Crossed: " << half_field << std::endl;
	std::cout << std::boolalpha<<"is charging: "<<is_charging << std::endl;
	std::cout << std::boolalpha<<"serving: "<< ready_to_serve << std::endl;*/


	if (!turn_end) {
		// lambda to award the point to the winner
		auto award_point = [&](int player) {
			if (player == PLAYER_1) {
				round_winner = Player1.player;
				Player1.score++;
			}
			else {
				round_winner = Player2.player;
				Player2.score++;
			}
			end_the_round();
			game_over_check();
		};
		
		//stop if touches net
		if (touched_net()) {
			if (current_turn == PLAYER_1) {
				ball.x = net.x - ball.w;
				award_point(PLAYER_1);
			}
			else {
				ball.x = net.x + net.w;
				award_point(PLAYER_2);
			}
		}

		//stop if it touches the ceiling or either sides
		if (ball.x + ball.w > SCREEN_WIDTH) {
			ball.x = SCREEN_WIDTH - ball.w;
			award_point(PLAYER_2);
		}
		else if (ball.x < 0) {
			ball.x = 0;
			award_point(PLAYER_1);
		}else if(ball.y < 0){
			if (current_turn == PLAYER_1) 
				award_point(PLAYER_1);
			else
				award_point(PLAYER_2);
		}

		//stop if it touches the ground
		if (ball.y + ball.h >= ground.y) {
			ball.y = (float)ground.y - ball.h;
			// first touch allowed in the opponent's field
			if (ball.dy > 0) {
				touch_counter++;
				ball.dy *= -0.6f;
				ball.dx *= 0.8f;  
			}
			// check if the ball's first touch is in the field of the shooter
			if (touch_counter == 1 && !crossed_half_field() ) {
				if (ready_to_serve && current_turn == PLAYER_1)
					award_point(PLAYER_2);
				else if (ready_to_serve && current_turn == PLAYER_2)
					award_point(PLAYER_1);
				else if (current_turn == PLAYER_1)
					award_point(PLAYER_1);
				else if (current_turn == PLAYER_2)
					award_point(PLAYER_2);
			}
			else if (touch_counter > 1) {
				if (crossed_half_field()) {
					if (current_turn == PLAYER_1)
						award_point(PLAYER_2);
					else
						award_point(PLAYER_1);
				}
				else {
					if (current_turn == PLAYER_1)
						award_point(PLAYER_1);
					else
						award_point(PLAYER_2);
				}
			}
		}
	}
	//charge the power as long as you hold
	if (is_charging) {
		power += 350.0f * delta_time;
		if (power > MAX_POWER) {
			power = MAX_POWER;
		}
	}

	//update the location of the ball unless the round ends
	if (!turn_end) {
		ball.dy += GRAVITY * (float)delta_time;
		ball.x += ball.dx * (float)delta_time;
		ball.y += ball.dy * (float)delta_time;
	}
	//update the rectangle
	ball_rect.x = (int)ball.x;
	ball_rect.y = (int)ball.y;
}

// loop that renders numbers on the 2d arrays
void Game::drawer_loop(const int num[5][3], int x, int y) {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 3; j++) {
			if (num[i][j] == 1) {
				SDL_Rect tiler = {x+(j*17),y+(i*17), 17, 17};
				SDL_RenderFillRect(renderer, &tiler);
			}
		}
	}
}

void Game::draw_digit(int num, int x, int y) {
	// use the 2d arrays to render the scores
	static const int ZERO[5][3] = {
		{1,1,1},
		{1,0,1},
		{1,0,1},
		{1,0,1},
		{1,1,1}
	};
	static const int ONE[5][3] = {
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0}
	};
	static const int TWO[5][3] = {
		{1,1,1},
		{0,0,1},
		{1,1,1},
		{1,0,0},
		{1,1,1}
	};
	static const int THREE[5][3] = {
		{1,1,1},
		{0,0,1},
		{1,1,1},
		{0,0,1},
		{1,1,1}
	};
	static const int FOUR[5][3] = {
		{1,0,1},
		{1,0,1},
		{1,1,1},
		{0,0,1},
		{0,0,1}
	};
	static const int FIVE[5][3] = {
		{1,1,1},
		{1,0,0},
		{1,1,1},
		{0,0,1},
		{1,1,1}
	};

	switch (num) {
		case 0:
			drawer_loop(ZERO, x, y);
			break;
		case 1:
			drawer_loop(ONE, x, y);
			break;
		case 2:
			drawer_loop(TWO, x, y);
			break;
		case 3:
			drawer_loop(THREE, x, y);
			break;
		case 4:
			drawer_loop(FOUR, x, y);
			break;
		case 5:
			drawer_loop(FIVE, x, y);
			break;
	}
}
//score board renderer
void Game::render_scoreboard() {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	draw_digit(Player2.score, SCREEN_WIDTH * 0.35f, (SCREEN_HEIGHT / 2 + 150));
	SDL_Rect dash = { SCREEN_WIDTH * 0.46f, SCREEN_HEIGHT / 2.0f + 185, 70, 15 };
	SDL_RenderFillRect(renderer, &dash);
	draw_digit(Player1.score, (SCREEN_WIDTH-51) * 0.64f, (SCREEN_HEIGHT / 2 + 150));
}

void Game::render() {
	// dark background
	SDL_SetRenderDrawColor(renderer, 0, 20, 20, 255); 
	SDL_RenderClear(renderer);

	// the glow effect
	for (int i = 0; i < 30; ++i) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 20, 40, 60 - (i * 2)); // fading alpha

		SDL_Rect glow_rect;
		glow_rect.w = SCREEN_WIDTH - (i * 60);
		glow_rect.h = SCREEN_HEIGHT - (i * 48);
		glow_rect.x = (SCREEN_WIDTH - glow_rect.w) / 2;
		glow_rect.y = (SCREEN_HEIGHT - glow_rect.h) / 2;

		SDL_RenderFillRect(renderer, &glow_rect);
	}
	//light colors 
	SDL_SetRenderDrawColor(renderer, 204, 255, 255, 255);
	SDL_RenderFillRect(renderer, &ball_rect);
	SDL_RenderFillRect(renderer, &ground);
	SDL_RenderFillRect(renderer, &net);
	render_scoreboard();
	
	// power bar that appear on top of the ball
	if (is_charging && !turn_end) {
		int filledWidth = (int)((power / MAX_POWER) * 100);
		SDL_Rect barFill = { (int)ball.x - 20, (int)ball.y - 20, filledWidth, 10 };

		SDL_SetRenderDrawColor(renderer, 0, 255, 150, 255);
		SDL_RenderFillRect(renderer, &barFill);
	}

	// the golden indicator to show the winner
	if (game_over) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		if (winner == PLAYER_1) {
			SDL_Rect win_indicator = { (int)((SCREEN_WIDTH - 51) * 0.64f) - 30, (SCREEN_HEIGHT / 2 + 150), 10, 10 };
			SDL_RenderFillRect(renderer, &win_indicator);
		}
		else {
			SDL_Rect win_indicator = { (int)(SCREEN_WIDTH * 0.35f) + 60, (SCREEN_HEIGHT / 2 + 150), 10, 10 };
			SDL_RenderFillRect(renderer, &win_indicator);
		}
	}

	SDL_RenderPresent(renderer);

}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}
