#define MAX_VISION 25

// bomber_state
#define KILLED 0
#define DIE 1
#define ALIVE 2

// map_objects
#define EMPTY_CELL 0
#define CELL_WITH_OBSTACLE 1
#define CELL_WTH_BOMB 2
#define CELL_WITH_BOMBER 3
#define CELL_WITH_BOMB_AND_BOMBER 4

typedef struct object
{
    int type;
    int remaining_durability;
} obj;

typedef struct bomber
{
    int arg_count;
    int state;
    pid_t bomber_pid;
    coordinate position;
} bomber;

typedef struct bomb
{
    pid_t pid;
    unsigned int radius;
    coordinate position;
} bomb;
