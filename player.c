
/** @file   Player.c
    @author Noah King Gagjun
    @date   13 October 2018
    @brief  Provides support functions for central game.c; name might be off
*/


#include <stdio.h>
#define canvas_height 5// CHANGE THIS BEFORE SUBMIT
#define canvas_width 5






struct player_s {
    char dir;
    int total;
    int x;
    int y;
};
typedef struct player_s PLAYER;


struct food_s {
    int visible; // to flash
    int x;
    int y;
};
typedef struct food_s FOOD;





//* Acts on pointer, depends on constant canvas width (WE ARE NOT USING TINYGL CANVAS HEIGHT, NEED TO have free space)
void  player_move(PLAYER* player)
{
    // translate playe about the canvas; wrap on edge
    switch(player->dir) {
    case ('N'):
        player->y = ((player->y -1 % canvas_height + canvas_height) % canvas_height);
        break;
    case ('S'):
        player->y  = ((player->y + 1)%canvas_height);
        break;
    case ('W'):
        player->x = ((player->x + 1)%canvas_width);
        break;
    case ('E'):
        player->x = ((player->x -1 % canvas_width + canvas_width) % canvas_width);
        break;
    default:
        break;

    }


}



/* checks if overlapping */
int check_touch(PLAYER player, FOOD food)
{
    if ((player.x == food.x) && (player.y == food.y)) {
        return 1;
    }
    return 0;
}






/*
 * @return random direction.
 */
int get_random_dir (void)
{
    char items[4] = {'N','E','S','W'};
    int r = rand() % 4;
    return items[r];
}

/* @return random integer in 0 - @oaram
*/
int rand_int(int bound)
{
    int r = rand() % bound;
    return r;
}

/* Toggles visible 0->1, 1->0 of food */
void toggle_state_food(FOOD* food)
{
    food->visible = ((food->visible + 1) %2);
}

/*
Checks if someone is touching food
* Returns L for local, F for foreign, 0 for nothing
*/
char win(PLAYER  local, PLAYER foreign,FOOD  food)
{
    if (check_touch(local,food)) {
        return 'L';
    } else if (check_touch(foreign,food)) {
        return 'F';
    }
    return '0';
}


/* chooses a random location for the food */
void get_pos_for_food(FOOD* food)
{
    food->x = rand_int(4);
    food->y = rand_int(canvas_height);
}



/*
* Generates encoded char for food location given as

 10*x + y with the first bit set TO ONE.
*/
char get_position_f(FOOD player)
{
    // Uses simple encoding to keep the first bit free
    char encode = ((player.x*10)+player.y);
    return (encode &  ~(1 << 7));
}


/** Generates encoded char for location without setting first bit */
char get_position(PLAYER player)
{
    // Uses simple encoding to keep the first bit free
    char encode = ((player.x*10)+player.y);
    return encode;
}


/* @ param
*  Recives the encoded character;
*  Given as an 8-bit char, first char dictates whether we are updating food local or player head local
*/

void update_position_foreign_player(PLAYER* player, FOOD* food, char from_ir)
{
    // MAY NEED TO DO SOME ERROR HANDLING HERE///
    int received = from_ir;
    int decode = (received &  ~(1 << 7));
    if ((received >> 7) & 1) {
        player->x = (decode/10);
        player->y = (decode /((decode/10)*10)); // Rounded.
    } else {
        food->x = (decode/10);
        food->y = (decode /((decode/10)*10));
    }

}


//*returns 1 if the two overlap */
int match(PLAYER p1, PLAYER p2)
{
    return ((p1.x == p2.x) && (p2.y == p1.y));
}


