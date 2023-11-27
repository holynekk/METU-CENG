#include "line.h"

uint8_t on_finish(note *mynote)
{
    if(mynote->position == LINE_LENGTH - 1)
        return 1;
    return 0;
}

void add_note(line *myline)
{
    uint8_t newpos = (myline->end + 1) % (LINE_LENGTH+1);
    // Do nothing if array is full.
    if (newpos == myline->begin)
        return;
    myline->notes[myline->end].position = 0;
    myline->end = newpos;
}

void pop_note(line *myline)
{
    // Do nothing if array is empty.
    if (myline->begin == myline->end)
        return;
    myline->begin = (myline->begin + 1) % (LINE_LENGTH+1);
}

note* peek(line *myline)
{
    // Return NULL if array is empty.
    if(myline->begin == myline->end)
        return NULL;
    return &myline->notes[myline->begin];
}

uint8_t advance_notes(line *myline)
{
    uint8_t crossed = 0;
    for(uint8_t i = myline->begin; i != myline->end; i = (i + 1) % (LINE_LENGTH+1))
    {
        note *mynote = &myline->notes[i];
        // Increment and check if note crosses the finish line.
        if(++mynote->position == LINE_LENGTH)
        {
            pop_note(myline);
            crossed = 1;
        }
    }
    return crossed;
}

line_iterator begin(line *myline)
{
    line_iterator it = {
        .myline = myline,
        .index = myline->begin
    };
    if (myline->begin == myline->end) {
        it.myline = NULL;
    }
    return it;
}

uint8_t is_valid(line_iterator *it)
{
    if(it->myline)
        return 1;
    return 0;
}

note* get_note(line_iterator *it)
{
    return &it->myline->notes[it->index];
}

void next(line_iterator *it)
{
    // If all items are iterated over, invalidate iterator.
    it->index = (it->index + 1) % (LINE_LENGTH+1);
    if(it->index == it->myline->end)
    {
        it->myline = NULL;
    }
}
