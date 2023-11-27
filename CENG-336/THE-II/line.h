#ifndef LINE_H
#define	LINE_H

#include <stdlib.h>
#include <stdint.h>

#define LINE_LENGTH 6

/* 
 * Struct that defines a note coming down a line.
 */
typedef struct note
{
    uint8_t position; /* range: [0, LINE_LENGTH - 1] */
} note;

/*
 * Struct that defines a single line that holds notes.
 * Uses a circular array.
 */
typedef struct line
{
    note notes[LINE_LENGTH+1];
    uint8_t begin;
    uint8_t end;
} line;

/*
 * Struct to help iterate over a line type.
 */
typedef struct line_iterator
{
    line *myline;
    uint8_t index;
} line_iterator;

/*
 * Checks if a given note is on the finish line.
 * Returns 1 if it is on finish, 0 otherwise.
 */
uint8_t on_finish(note *mynote);

/*
 * Adds a new note to the specified line.
 */
void add_note(line *myline);

/*
 * Pops the last note in the specified line.
 */
void pop_note(line *myline);

/*
 * Gets the note closest to the end point.
 * Returns NULL if there are no notes.
 */
note* peek(line *myline);

/*
 * Advances the notes in the line by one.
 * Returns 1 if a note crosses the finish line,
 * 0 otherwise.
 */
uint8_t advance_notes(line *myline);

/*
 * Gets the iterator pointing to a line's beginning.
 */
line_iterator begin(line *myline);

/*
 * Checks if an iterator is valid.
 * Returns 1 if valid, 0 otherwise.
 */
uint8_t is_valid(line_iterator *it);

/*
 * Gets the note type pointed by the iterator.
 * It can be NULL.
 */
note* get_note(line_iterator *it);

/*
 * Advances the iterator by one.
 */
void next(line_iterator *it);

#endif	/* LINE_H */
