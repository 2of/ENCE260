
/** @file   Noah King
    @author
    @date   13 October 2018
    @brief  Contains the main method for the game, see the readme for information
    about playing
*/

// ---------------------
#include <stdlib.h>
#include "system.h"
#include "button.h"
#include "pacer.h"
#include "task.h"
#include "tinygl.h"
#include "navswitch.h"
#include "ledmat.h"
#include "ir_uart.h"
#include "player.h"
#include "led.h"
#include "../fonts/font5x7_1.h"
//---------------------


#define matrix_loop_rate 1000
#define matrix_text_translate_speed 10
#define receive_rate 20
#define max_score 5
#define matrix_update_rate 500
#define text_scroll_speed 10
#define HOST_CODE 'M'
#define start_message "Press Button To start"
#define win_message "Win!"
#define lose_message "Lose!"
#define BUTTON_TASK_RATE 100
#define DISPLAY_TASK_RATE 250
#define LED_TASK_RATE 100
#define IR_TASK_RATE 100
#define GAME_TASK_RATE 6
#define BLINK_FOOD_RATE 5
#define PLAYER_TASK_RATE 6
#define led1 LED1
#define local_start {'N',0, 3,6}
#define foreign_start {'S',0, 3,3}
#define food_start {1,2,2}





// - Header files -//
void display_to_start(void);
void draw_canvas(PLAYER p1, PLAYER p2, FOOD food);
void send_char(char in);
void win_end(void);
void display_task_init(void);
void lose_end(void);



//Static vars -//
static char character = '0';
static int led_state = 0;
static int g_state = 0;
static char input_direction = '0';
static int host = 0;
static PLAYER local = local_start;
static PLAYER * ptr_local = &local;
static PLAYER foreign = foreign_start;
static PLAYER* ptr_foreign = &foreign;
static FOOD food = food_start;
static FOOD* ptr_food = &food;
static int led_flash_num = 0;









/* Initialises the TINYGL display
*
*/
void display_task_init()
{
    tinygl_init (matrix_update_rate);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (text_scroll_speed);
}


/* This method polls the buttons:
* Directions : Directions
* B		  	 : Button is pressed
* D			 : Dpad is depressed
* Z			 : No button is pressed
* This value is given to the @param input_direction
* This function also starts the game from g_state 1 if the button is pressed
		-declares us as host, sends this over ir_uart
*/
static void button_task(__unused__ void* data)
{
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        input_direction = 'N';
    } else if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        input_direction = 'S';
    } else if (navswitch_push_event_p (NAVSWITCH_EAST)) {
        input_direction = 'W';
    } else if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        input_direction = 'E';
    } else if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
        input_direction = 'D';
        if (g_state == 1) {
            host = 0;
            g_state = 2;
            //tinygl_clear();
        }
    } else if (button_push_event_p(0)) {
        input_direction = 'B';
        if (g_state == 1) {
            host = 1;
            led_set(led1,1);
            send_char(HOST_CODE);
            g_state = 2;
        }
    } else {
        input_direction = 'Z';
    }
    if (input_direction != 'Z') {
        local.dir = input_direction;
    }
    navswitch_update();
    button_update();
}



/* Toggles visibility of food, left open for addition
*
*/
static void blink_food_task(__unused__ void *data)
{
    toggle_state_food(ptr_food);
}


/* Translates the local player, decrements score if moves into other player
*
*/
static void player_task(__unused__ void *data)
{
    player_move(ptr_local);
    if (match(local,foreign)) {
		if ( local.total >0 ) {
        local.total--;
        }
    }
}



/* Flashes LED and decrements LED flash counter
*	 - call from elsewhere with led_flash_num = n;
*
*/
static void led_task(__unused__ void *data)
{	

    if ((led_flash_num > 0)) {
        led_state = (led_state + 1)%2;
        led_flash_num--;
        led_set(led1,led_state);


    }
}


/* Method checks whether or not the maximum score is reached
* Also checks if we should increment score or not, from whether we
* are overlapping with the other player or the food
*
*/
static void update_game_task (__unused__ void *data)
{
    if (local.total >= max_score) {
        win_end();
        g_state = 3;
    } else if (foreign.total >= max_score) {
        lose_end();
        g_state = 3;

    } else {
        char test = (win(local,foreign,food));
        switch (test) {
        case ('L'):
            get_pos_for_food(ptr_food);
            led_flash_num = 8;
            local.total++;
            break;
        case ('F'):
            led_flash_num = 2;
            break;
        case ('0'):
            break;
        default:
            break;
        }
    }
}







/*Sends the encoded player position over the IRUART
* Sends the food location if we are the host
* - see the method for encoding location (get_location and get_location_food) in snake.c
* an explanaation of this

* Also receives if we are the host or not via hostcode; if received we are NOT host.
*/
static void ir_task(__unused__ void *data)
{


    if (ir_uart_read_ready_p()) {
        if (((g_state == 0) || (g_state == 1)) && (character == HOST_CODE)) {
            g_state = 2;
            host = 0;
            tinygl_clear();
        }  else {
            char character = ir_uart_getc();
            update_position_foreign_player(ptr_foreign, ptr_food, character);
        }
    }

    send_char(get_position(local));
    if (host) {
        (send_char(get_position_f(food)));
    }
}



/* Updates the display depending on g_state
* Case 3 is a paused state;
*/
static void display_task(__unused__ void *data)
{
    switch (g_state) {
    case 0:
        display_to_start();
        g_state = 1;
        break;
    case 2:
        draw_canvas(local,foreign,food);

        break;
    case 3:

        break;


    default:
        break;
    }

    tinygl_update();
}




int main (void)
{

    // -setup- //
    system_init ();
    display_task_init();
    navswitch_init ();
    ir_uart_init();
    led_init();
    led_set(led1,0);
    button_init();


	//place tasks into task
    task_t tasks[] = {
        {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE},
        {.func = button_task, .period = TASK_RATE / BUTTON_TASK_RATE},
        {.func = led_task, .period = TASK_RATE / LED_TASK_RATE},
        {.func = player_task, .period = TASK_RATE / PLAYER_TASK_RATE},
        {.func = ir_task, .period = TASK_RATE / IR_TASK_RATE},
        {.func = update_game_task, .period = TASK_RATE / GAME_TASK_RATE},
        {.func = blink_food_task, .period = TASK_RATE / BLINK_FOOD_RATE},


    };

    display_task_init();

    task_schedule (tasks, ARRAY_SIZE (tasks));
}

/* Draws Canvas;
 * Points are drawn individually for each player,food
 * Scorebars are drawn on the top two rows.
 */
void draw_canvas(PLAYER p1, PLAYER p2, FOOD food)
{
    tinygl_clear();
    tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
    tinygl_pixel_set(tinygl_point(local.x,p1.y),1);
    tinygl_pixel_set(tinygl_point(p2.x,p2.y),1);
    tinygl_pixel_set(tinygl_point(food.x,food.y),food.visible);
    for (int i =  0; i < p1.total ; i ++ ) {
        tinygl_pixel_set(tinygl_point(i,6),1);
    }
    for (int i = 0; i <p2.total ; i ++ ) {
        tinygl_pixel_set(tinygl_point(i,5),1);
    }

}

/* Broadcast character over IR_UART */
void send_char(char to_send)
{
    ir_uart_putc(to_send);
}

/*Shows welcome message*/
void display_to_start(void)
{
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text(start_message);
    tinygl_text_speed_set (matrix_text_translate_speed);
}




/*display winning message*/
void win_end()
{
    tinygl_text(win_message);
}
/* display lose message*/
void lose_end()
{
    tinygl_text(lose_message);
}


