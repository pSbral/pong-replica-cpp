#define is_down(b) (input->buttons[b].is_down)
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

float player_p = 0.0f, player_dp = 0.0f;
const float player_hsx = 2.5f, player_hsy = 12.0f, arena_hsx = 85.0f, arena_hsy = 40.0f;
float ball_p_x = 0.0f, ball_p_y = 0.0f, ball_dp_x = 100.0f, ball_dp_y = 0.0f, ball_hs = 1.0f;

internal void simulate_game(Input* input, float dt) {
	clear_screen(0xff5500);
	draw_rect(0, 0, arena_hsx, arena_hsy, 0xffaa33);

	// Update player position and velocity
	float player_ddp = 0.0f;
	if (is_down(BUTTON_UP) || is_down(BUTTON_W)) {
		player_ddp += 2000.0f;
	}
	if (is_down(BUTTON_DOWN) || is_down(BUTTON_S)) {
		player_ddp -= 2000.0f;
	}
	player_ddp -= player_dp * 10.0f;

	float dt_half = dt * 0.5f;
	player_p += player_dp * dt + player_ddp * dt * dt_half;
	player_dp += player_ddp * dt;

	// Clamp player position to arena bounds
	if (player_p + player_hsy > arena_hsy) {
		player_p = arena_hsy - player_hsy;
		player_dp = 0.0f;
	}
	else if (player_p - player_hsy < -arena_hsy) {
		player_p = -arena_hsy + player_hsy;
		player_dp = 0.0f;
	}

	// Update ball position
	ball_p_x += ball_dp_x * dt;
	ball_p_y += ball_dp_y * dt;

	// Ball collision with player on the right
	if (ball_p_x + ball_hs > 80 - player_hsx &&
		ball_p_x - ball_hs < 80 + player_hsx &&
		ball_p_y + ball_hs > player_p - player_hsy &&
		ball_p_y - ball_hs < player_p + player_hsy) {

		ball_p_x = 80 - player_hsx - ball_hs;
		ball_dp_x *= -1.0f;
		ball_dp_y = player_dp;
	}
	// Ball collision with player on the left
	else if (ball_p_x + ball_hs > -80 - player_hsx &&
		ball_p_x - ball_hs < -80 + player_hsx &&
		ball_p_y + ball_hs > -player_hsy &&
		ball_p_y - ball_hs < player_hsy) {

		ball_p_x = -80 + player_hsx + ball_hs;
		ball_dp_x *= -1.0f;
	}

	if (ball_p_y + ball_hs > arena_hsy) {
		ball_p_y = arena_hsy - ball_hs;
		ball_dp_y *= -1;
	}
	else if (ball_p_y + ball_hs < -arena_hsy) {
		ball_p_y = -arena_hsy - ball_hs;
		ball_dp_y *= -1;
	}

	if (ball_p_x + ball_hs > arena_hsx || ball_p_x + ball_hs < -arena_hsx) {
		ball_p_x = 0;
		ball_p_y = 0;
		ball_dp_x = 100;
	}

	

	// Draw ball and players
	draw_rect(ball_p_x, ball_p_y, ball_hs, ball_hs, 0xffffff);
	draw_rect(80, player_p, player_hsx, player_hsy, 0xff0000);
	draw_rect(-80, 0, player_hsx, player_hsy, 0xff0000);
}
