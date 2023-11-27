#include "player.h"
#include "line.h"

void button_clicked(player *myplayer, line* myline)
{
    if(!myplayer->health || !myplayer->remaining_notes)
        return;
    note* mynote = peek(myline);
    if(mynote && on_finish(mynote))
    {
        pop_note(myline);
        // Finished all notes?
        if(!--myplayer->remaining_notes)
            next_level();
    }
    else
    {
        // Mistake leads to game over?
        if(!--myplayer->health)
            game_over(0);
    }
}
