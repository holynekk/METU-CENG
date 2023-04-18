#define MAX_VISION 64

#define ALIVE 0
#define KILLED 1
#define DIE 2

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
    pid_t bomber_pid;
    int arg_count;
    int status;
    coordinate position;
} bomber;

typedef struct bomb
{
    pid_t pid;
    unsigned int radius;
    coordinate position;
} bomb;
