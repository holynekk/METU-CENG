#ifndef PLAYER_H
#define	PLAYER_H

#include <stdint.h>

extern void next_level();
extern void game_over(uint8_t is_success);

typedef struct line line;

/*
 * Struct to define a player and keep the main game state.
 */
typedef struct player
{
    uint8_t health;
    uint8_t level;
    uint8_t remaining_notes;
} player;

/*
 * Called when a button is clicked which is associated with the given line.
 */
void button_clicked(player *myplayer, line* myline);

#endif	/* PLAYER_H */

