#ifndef PET_H
#define PET_H

typedef struct pet {
    int money;
    char thirst;
    char hunger;
    char happiness;
} pet;

void init_money(int money);

void add_money(int money);

void update_pet(char hunger, char thirst, char happiness);

void stop_game(void);

pet* get_pet(void);

#endif /* PET_H */
