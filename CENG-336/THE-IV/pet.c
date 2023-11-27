#include "pet.h"

#include "transmit.h"

extern void update_lcd(void);

static pet mypet;
static int do_play = 0;

static char command_play[] = "{P}";
static char command_feed[] = "{F}";
static char command_water[] = "{W}";

// Checks the pet's state and sends commands when necessary
static void work() {
    if(!do_play)
        return;
    if (mypet.happiness <= 20 && mypet.money >= 150) {
        mypet.happiness += 80;
        mypet.money -= 150;
        request_transmit(command_play);
    }
    if (mypet.hunger <= 40 && mypet.money >= 80) {
        mypet.hunger += 60;
        mypet.money -= 80;
        request_transmit(command_feed);
    }
    if (mypet.thirst <= 70 && mypet.money >= 30) {
        mypet.thirst += 30;
        mypet.money -= 30;
        request_transmit(command_water);
    }
    update_lcd();
}

void init_money(int money) {
    mypet.happiness = 100;
    mypet.hunger = 100;
    mypet.thirst = 100;
    mypet.money = money;
    do_play = 1;
    work();
}

void add_money(int money) {
    mypet.money += money;
    work();
}

void update_pet(char hunger, char thirst, char happiness) {
    mypet.hunger = hunger;
    mypet.thirst = thirst;
    mypet.happiness = happiness;
    work();
}

void stop_game(void) {
    do_play = 0;
}

pet* get_pet(void) {
    return &mypet;
}
