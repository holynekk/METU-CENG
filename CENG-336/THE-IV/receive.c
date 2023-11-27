#include "receive.h"

#include "pet.h"
#include "tsk_hash.h"

enum COMMAND_TYPE {
    UNKNOWN,
    GO,
    END,
    SENSOR,
    ALERT,
    PAYMENT
};

typedef struct command_go {
    int money;
} go;

typedef struct command_end {
    char skip;
} end;

typedef struct command_alert {
    char str[9];
} alert;

typedef struct command_sensor {
    char hunger;
    char happy;
    char thirst;
} sensor;

typedef struct command_payment {
    int money;
} payment;

typedef union command {
    go go;
    end end;
    sensor sensor;
    alert alert;
    payment payment;
} command;

typedef struct message {
    enum COMMAND_TYPE type;
    command command;
} message;

static char buffer[32];
static char *head;

static void handle_message(message *m) {
    switch(m->type) {
        case UNKNOWN:
            break;
        case GO:
            init_money(m->command.go.money);
            break;
        case END:
            stop_game();
            break;
        case SENSOR:
            update_pet(m->command.sensor.hunger,
                    m->command.sensor.thirst,
                    m->command.sensor.happy);
            break;
        case ALERT:
            alert(m->command.alert.str);
            break;
        case PAYMENT:
            add_money(m->command.payment.money);
            break;
        default:
            break;
    }
}

// Parses the received input
static void parse(void) {
    message m;
    int i;
    char *start = buffer;
    m.type = UNKNOWN;
    // Catching the first char of command
    while(*start != '{')
        ++start;
    if(start[1] == 'G' && start[2] == 'O') {
        // GO command
        m.type = GO;
        m.command.go.money = 0;
        m.command.go.money = (int)start[3] << 8;
        m.command.go.money += (int)start[4];
    } else if(start[1] == 'E' && start[2] == 'N' && start[3] == 'D') {
        // END command
        m.type = END;
    } else if(start[1] == 'S') {
        // SXYZ command
        m.type = SENSOR;
        m.command.sensor.hunger = start[2];
        m.command.sensor.happy = start[3];
        m.command.sensor.thirst = start[4];
    }else if(start[1] == 'A') {
        // ASTR
        m.type = ALERT;
        for(i = 0; i < 8; ++i)
            m.command.alert.str[i] = start[i+2];
        m.command.alert.str[8] = '\0';
    } else if(start[1] == 'M') {
        // MXX command
        m.type = PAYMENT;
        m.command.payment.money = 0;
        m.command.payment.money = (int)start[2] << 8;
        m.command.payment.money += (int)start[3];
    }
    handle_message(&m);
    receive_init();
}

void receive_init(void) {
    head = buffer;
}

void receive(char c) {
    *head++ = c;
    if(c == '}')
        parse();
}
