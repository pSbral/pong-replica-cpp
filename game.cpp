#define is_down(b) (input->buttons[b].is_down)
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

float player_p = 0.0f, player_dp = 0.0f, ai_p = 0.0f, ai_dp = 0.0f;
const float player_hsx = 2.5f, player_hsy = 12.0f, arena_hsx = 85.0f, arena_hsy = 37.0f;
float ball_p_x = 0.0f, ball_p_y = 0.0f, ball_dp_x = 100.0f, ball_dp_y = 0.0f, ball_hs = 1.0f;

int player_1_score, player_2_score;


// Function to handle player movement and clamping
internal void update_player(float& player_p, float& player_dp, float player_ddp, float& ai_p, float& ai_dp, float ai_ddp, float dt, float player_hsy, float arena_hsy) {
    float dt_half = dt * 0.5f;

    player_p += player_dp * dt + player_ddp * dt * dt_half;
    player_dp += player_ddp * dt;

    ai_p += ai_dp * dt + ai_ddp * dt * dt_half;
    ai_dp += ai_ddp * dt;

    // Clamp player position to arena bounds
    if (player_p + player_hsy > arena_hsy) {
        player_p = arena_hsy - player_hsy;
        player_dp = 0.0f;
    }
    else if (player_p - player_hsy < -arena_hsy) {
        player_p = -arena_hsy + player_hsy;
        player_dp = 0.0f;
    }

    // Clamp ai position to arena bounds
    if (ai_p + player_hsy > arena_hsy) {
        ai_p = arena_hsy - player_hsy;
        ai_dp = 0.0f;
    }
    else if (ai_p - player_hsy < -arena_hsy) {
        ai_p = -arena_hsy + player_hsy;
        ai_dp = 0.0f;
    }
}

// Function to handle ball collisions
internal bool ball_collides(float ball_p_x, float ball_p_y, float ball_hs, float player_p, float player_hsx, float player_hsy, float boundary_x) {
    return (ball_p_x + ball_hs > boundary_x - player_hsx &&
        ball_p_x - ball_hs < boundary_x + player_hsx &&
        ball_p_y + ball_hs > player_p - player_hsy &&
        ball_p_y - ball_hs < player_p + player_hsy);
}

// Function to reset the ball's position and velocity
internal void reset_ball(float& ball_p_x, float& ball_p_y, float& ball_dp_x, float& ball_dp_y) {
    ball_p_x = 0.0f;
    ball_p_y = 0.0f;
    ball_dp_x = 100.0f;
    ball_dp_y = 0.0f;
}

enum Gamemode {
    GM_MENU,
    GM_GAMEPLAY,
};

Gamemode current_gamemode;
int hot_button;
bool enemy_is_ai;

internal void simulate_game(Input* input, float dt) {
    draw_rect(0, 0, arena_hsx, arena_hsy, 0x8d99ae);
    draw_arena_borders(arena_hsx, arena_hsy, 0x2b2d42);

    if (current_gamemode == GM_GAMEPLAY) {
        // Update player position and velocity
        float player_ddp = 0.0f;
        if (is_down(BUTTON_W)) player_ddp += 2000.0f;
        if (is_down(BUTTON_S)) player_ddp -= 2000.0f;
        player_ddp -= player_dp * 10.0f;

        // Update ai position and velocity
        float ai_ddp = 0.0f;
        if (!enemy_is_ai) {
            if (ball_p_y > ai_p + 2.f) ai_ddp += 1200;
            if (ball_p_y < ai_p - 2.f) ai_ddp -= 1200;
            ai_ddp -= ai_dp * 10.0f;
        }
        else {
            if (is_down(BUTTON_UP)) ai_ddp += 2000.0f;
            if (is_down(BUTTON_DOWN)) ai_ddp -= 2000.0f;
            ai_ddp -= ai_dp * 10.0f;
        }
        

        update_player(player_p, player_dp, player_ddp, ai_p, ai_dp, ai_ddp, dt, player_hsy, arena_hsy);

        // Update ball position
        ball_p_x += ball_dp_x * dt;
        ball_p_y += ball_dp_y * dt;

        // Ball collision with player on the right
        if (ball_collides(ball_p_x, ball_p_y, ball_hs, player_p, player_hsx, player_hsy, 80.0f)) {
            ball_p_x = 80.0f - player_hsx - ball_hs;
            ball_dp_x *= -1.0f;
            ball_dp_y = player_dp;
        }
        // Ball collision with ai on the left
        else if (ball_collides(ball_p_x, ball_p_y, ball_hs, ai_p, player_hsx, player_hsy, -80.0f)) {
            ball_p_x = -80.0f + player_hsx + ball_hs;
            ball_dp_x *= -1.0f;
        }

        // Ball collision with top/bottom walls
        if (ball_p_y + ball_hs > arena_hsy) {
            ball_p_y = arena_hsy - ball_hs;
            ball_dp_y *= -1.0f;
        }
        else if (ball_p_y + ball_hs < -arena_hsy) {
            ball_p_y = -arena_hsy + ball_hs;
            ball_dp_y *= -1.0f;
        }

        // Ball out of horizontal bounds and Score
        if (ball_p_x + ball_hs > arena_hsx || ball_p_x - ball_hs < -arena_hsx) {
            if (ball_p_x + ball_hs > arena_hsx) { player_1_score++; }
            if (ball_p_x - ball_hs < -arena_hsx) { player_2_score++; }

            reset_ball(ball_p_x, ball_p_y, ball_dp_x, ball_dp_y);
            }

        draw_number(player_1_score, -10, 30, 1.f, 0xedf2f4);
        draw_number(player_2_score, 10, 30, 1.f, 0xedf2f4);

        // Draw ball and players
        draw_rect(ball_p_x, ball_p_y, ball_hs, ball_hs, 0xef233c);
        draw_rect(80, player_p, player_hsx, player_hsy, 0xd90429);
        draw_rect(-80, ai_p, player_hsx, player_hsy, 0xd90429);
    }
    else {

        if (pressed(BUTTON_A) || pressed(BUTTON_D)) {
            hot_button = !hot_button;
        }

        if (pressed(BUTTON_SPACE)) {
            current_gamemode = GM_GAMEPLAY;
            enemy_is_ai = hot_button ? 0 : 1;
        }

        if (hot_button == 0) {
            draw_text("SP", -20, 0, 1, 0xffffff);
            draw_text("MP", 20, 0, 1, 0xff0000);
        } else {
            draw_text("SP", -20, 0, 1, 0xff0000);
            draw_text("MP", 20, 0, 1, 0xffffff);
        }
    }
}
