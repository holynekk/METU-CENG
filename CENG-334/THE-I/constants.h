#define PATH_MAX 4096
#define MAX_ARGS 10
#define MAX_ARG_LEN 10
#define MAX_BOMB 50
#define MAX_VISION 25

// bomber_state
#define KILLED 0
#define DIE 1
#define ALIVE 2

// map_objects
#define MEMPTY 0
#define MBOMB 1
#define MBOMB_AND_BOMBER 2
#define MBOMBER 3
#define MOBSTACLE 4

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
