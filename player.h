#include <player.c>



/** @file   player.h
    @author Noah King Gagjun
    @date   13 October 2018
    @brief  Provides support functions for central game.c; name might be off
*/






/** get_position
    @param player struct; just reference to values
    @return An encoded version of the location to transmit (Given
    Simply as x*10 + y to preserve first bit of 8)
*/
char get_position(PLAYER player);


/** toggle_state_food
    @param Takes a pointer to food, toggles visibility attribute
*/
void toggle_state_food(FOOD* food);

/** get_position_f
    @param player struct; just reference to values
    @return An encoded version of the location to transmit (Given
    Simply as x*10 + y to preserve first bit of 8) The first bit of this sequence
    is set to high to indicate that this si the food not player location
    (Values for position never exceed 7/8 bits.)
*/
char get_position_f(FOOD food);



/** get_pos_for_food
 * @param FOOD pointer
 * Generates and replaes the old loation with new random location
 */
void get_pos_for_food(FOOD* food);



/** Check_touch
    @param instances of food and player
    @return { 1 = the two locations are the same, 0 = elsewise}
    Simply as x*10 + y to preserve first bit of 8)
*/
int check_touch(PLAYER player, FOOD food);


/** update_position_foreign_player
    @param PLAYER pointer, FOOD pointer and the received char for either of these locations
    @return nothing
    If the first bit of the char received is one, the food's location attributes are updated by its
    decomposition, otherwise the player's is
*/
void update_position_foreign_player(PLAYER* player, FOOD* food, char from_ir);


/** win
    @param referenes of player, foreign player, food
    @return char : {L = player is touching food
                    F = player is touhing foreign
                    0 = Neither of these cases}
*/
char win(PLAYER  local, PLAYER foreign,FOOD  food);


/** player_move
    @param PLAYER pointer
    Updates the player's location about the canvas relative to their location, direction
*/
void player_move(PLAYER* player);


/** check_touch
    @param reference to food and player
    returns 1 if the two are overlapped
*/
int check_touch(PLAYER player, FOOD food);


/** match
 * @param two players
 * @return 1 if overlap, 0 if not
 */
int match(PLAYER p1, PLAYER p2);

