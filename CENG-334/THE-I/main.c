#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "logging.h"
#include "message.h"
#include "constants.h"

void main()
{
    int map_width, map_height, obstacle_count, bomber_count;
    scanf("%d %d %d %d", &map_width, &map_height, &obstacle_count, &bomber_count);

    obj map[map_height][map_width];
    for (int i = 0; i < map_height; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            map[i][j].type = EMPTY_CELL;
        }
    }

    int obstacle_x, obstacle_y, obstacle_durability;
    for (int i = 0; i < obstacle_count; i++)
    {
        scanf("%d %d %d", &obstacle_x, &obstacle_y, &obstacle_durability);
        map[obstacle_y][obstacle_x].type = CELL_WITH_OBSTACLE;
        map[obstacle_y][obstacle_x].remaining_durability = obstacle_durability;
    }

    char *exec_of_bomber = (char *)malloc(sizeof(char) * 64);
    char bomber_execs[bomber_count][sizeof(char) * 64];

    char **arg_of_bomber = (char **)malloc(sizeof(char *) * 10);
    char *bomber_args[bomber_count][10];

    bomber bombers[bomber_count];
    bomb bombs[64];

    pid_t child_pid_bomber;
    int bomber_pipes[bomber_count][2];
    int bomb_pipes[64][2];
    int child_status_bomb[64];
    int child_status_bomber[bomber_count];

    for (int i = 0; i < bomber_count; i++)
    {
        PIPE(bomber_pipes[i]);

        scanf("%d %d %d", &bombers[i].position.x, &bombers[i].position.y, &bombers[i].arg_count);
        map[bombers[i].position.y][bombers[i].position.x].type = CELL_WITH_BOMBER;
        scanf("%s", exec_of_bomber);
        strcpy(bomber_execs[i], exec_of_bomber);
        *(arg_of_bomber) = malloc(10 * sizeof(char));
        bomber_args[i][0] = exec_of_bomber;
        for (int a = 1; a < bombers[i].arg_count; a++)
        {
            *(arg_of_bomber + a) = malloc(10 * sizeof(char));
            scanf("%s", *(arg_of_bomber + a));
            bomber_args[i][a] = (char *)malloc(strlen(*(arg_of_bomber + a)) + 1);
            strcpy(bomber_args[i][a], *(arg_of_bomber + a));
        }
        bomber_args[i][bombers[i].arg_count] = NULL;
        bombers[i].state = ALIVE;

        if (fork() != 0)
        {
            close(bomber_pipes[i][1]);
            bombers[i].bomber_pid = child_pid_bomber;
        }
        else
        {
            bombers[i].bomber_pid = child_pid_bomber;
            dup2(bomber_pipes[i][1], 1);
            dup2(bomber_pipes[i][1], 0);
            close(bomber_pipes[i][0]);
            close(bomber_pipes[i][1]);
            execv(exec_of_bomber, bomber_args[i]);
        }
    }

    struct pollfd bomber_pollfds[bomber_count], bomb_pollfds[64];
    int num_bombers_alive = bomber_count, bombomber_count = 0, bombs_exploded = 0;

    while (num_bombers_alive != 0)
    {
        for (int i = 0; i < bombomber_count; i++)
        {
            bomb_pollfds[i].fd = bomb_pipes[i][0];
            bomb_pollfds[i].events = POLLIN;
            bomb_pollfds[i].revents = 0;
        }

        if (bombomber_count != 0)
        {
            poll(bomb_pollfds, bombomber_count, 1);
        }

        for (int i = 0; i < bombomber_count; i++)
        {
            if (bomb_pollfds[i].revents & POLLIN)
            {
                im incoming_message;
                if (read_data(bomb_pipes[i][0], &incoming_message) == -1)
                {
                    perror("read bomb data error");
                }

                imp imessage_print;
                imessage_print.m = &incoming_message;
                imessage_print.pid = bombs[i].pid;
                print_output(&imessage_print, NULL, NULL, NULL);

                if (incoming_message.type == BOMB_EXPLODE)
                {
                    unsigned int r = bombs[i].radius;
                    coordinate location = bombs[i].position;
                    for (int d = 0; d < (r + 1); d++)
                    {
                        if ((location.y + d) == map_height)
                        {
                            break;
                        }
                        if (map[location.y + d][location.x].type == CELL_WITH_OBSTACLE)
                        {
                            map[location.y + d][location.x].remaining_durability -= 1;
                            obsd obstacle;
                            obstacle.position.x = location.x;
                            obstacle.position.y = location.y + d;
                            obstacle.remaining_durability = map[location.y + d][location.x].remaining_durability;
                            print_output(NULL, NULL, &obstacle, NULL);
                            if (map[location.y + d][location.x].remaining_durability == 0)
                            {
                                map[location.y + d][location.x].type = EMPTY_CELL;
                            }
                            break;
                        }
                        else if (map[location.y + d][location.x].type == CELL_WITH_BOMBER || map[location.y + d][location.x].type == CELL_WITH_BOMB_AND_BOMBER)
                        {
                            for (int y = 0; y < bomber_count; y++)
                            {
                                if (bombers[y].position.x == location.x && bombers[y].position.y == location.y + d)
                                {
                                    bombers[y].state = DIE;
                                    map[location.y + d][location.x].type = EMPTY_CELL ? map[location.y + d][location.x].type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int d = 1; d < (r + 1); d++)
                    {
                        if ((location.y - d) == -1)
                        {
                            break;
                        }
                        if (map[location.y - d][location.x].type == CELL_WITH_OBSTACLE)
                        {
                            map[location.y - d][location.x].remaining_durability -= 1;
                            if (map[location.y - d][location.x].remaining_durability == 0)
                            {
                                map[location.y - d][location.x].type = EMPTY_CELL;
                            }
                            break;
                        }
                        else if (map[location.y - d][location.x].type == CELL_WITH_BOMBER || map[location.y - d][location.x].type == CELL_WITH_BOMB_AND_BOMBER)
                        {
                            for (int y = 0; y < bomber_count; y++)
                            {
                                if (bombers[y].position.x == location.x && bombers[y].position.y == location.y - d)
                                {
                                    bombers[y].state = DIE;
                                    map[location.y - d][location.x].type = EMPTY_CELL ? map[location.y - d][location.x].type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int d = 0; d < (r + 1); d++)
                    {
                        if ((location.x + d) == map_width)
                        {
                            break;
                        }
                        if (map[location.y][location.x + d].type == CELL_WITH_OBSTACLE)
                        {
                            map[location.y][location.x + d].remaining_durability -= 1;
                            if (map[location.y][location.x + d].remaining_durability == 0)
                            {
                                map[location.y][location.x + d].type = EMPTY_CELL;
                            }
                            break;
                        }
                        else if (map[location.y][location.x + d].type == CELL_WITH_BOMBER || map[location.y][location.x + d].type == CELL_WITH_BOMB_AND_BOMBER)
                        {
                            for (int y = 0; y < bomber_count; y++)
                            {
                                if (bombers[y].position.x == location.x + d && bombers[y].position.y == location.y)
                                {
                                    bombers[y].state = DIE;
                                    map[location.y][location.x + d].type = EMPTY_CELL ? map[location.y][location.x + d].type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int d = 0; d < (r + 1); d++)
                    {
                        if ((location.x - d) == -1)
                        {
                            break;
                        }
                        if (map[location.y][location.x - d].type == CELL_WITH_OBSTACLE)
                        {
                            map[location.y][location.x - d].remaining_durability -= 1;
                            if (map[location.y][location.x - d].remaining_durability == 0)
                            {
                                map[location.y][location.x - d].type = EMPTY_CELL;
                            }
                            break;
                        }
                        else if (map[location.y][location.x - d].type == CELL_WITH_BOMBER || map[location.y][location.x - d].type == CELL_WITH_BOMB_AND_BOMBER)
                        {
                            for (int y = 0; y < bomber_count; y++)
                            {
                                if (bombers[y].position.x == location.x - d && bombers[y].position.y == location.y)
                                {
                                    bombers[y].state = DIE;
                                    map[location.y][location.x - d].type = EMPTY_CELL ? map[location.y][location.x - d].type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    bombs_exploded++;
                    close(bomb_pipes[i][0]);
                    waitpid(bombs[i].pid, &child_status_bomb[i], 0);
                }
            }
        }

        for (int i = 0; i < bomber_count; i++)
        {
            bomber_pollfds[i].fd = bomber_pipes[i][0];
            bomber_pollfds[i].events = POLLIN;
            bomber_pollfds[i].revents = 0;
        }

        poll(bomber_pollfds, bomber_count, -1);

        for (int i = 0; i < bomber_count; i++)
        {
            if (num_bombers_alive == 1)
            {
                om outgoing_message_win;
                outgoing_message_win.type = BOMBER_WIN;
                num_bombers_alive = 0;
                if (send_message(bomber_pipes[i][0], &outgoing_message_win) == -1)
                {
                    perror("BOMBER_WIN send message error");
                    exit(EXIT_FAILURE);
                }

                omp omessage_print_WIN;
                omessage_print_WIN.m = &outgoing_message_win;
                omessage_print_WIN.pid = bombers[i].bomber_pid;
                print_output(NULL, &omessage_print_WIN, NULL, NULL);

                close(bomber_pipes[i][0]);
                waitpid(bombers[i].bomber_pid, &child_status_bomber[i], 0);
                break;
            }

            if ((bomber_pollfds[i].revents & POLLIN) && bombers[i].state != KILLED)
            {
                if (bombers[i].state == DIE)
                {
                    om outgoing_message_die;
                    outgoing_message_die.type = BOMBER_DIE;
                    bombers[i].state = KILLED;
                    num_bombers_alive--;
                    if (send_message(bomber_pipes[i][0], &outgoing_message_die) == -1)
                    {
                        perror("BOMBER_DIE send message error");
                        exit(EXIT_FAILURE);
                    }

                    omp omessage_print_DIE;
                    omessage_print_DIE.m = &outgoing_message_die;
                    omessage_print_DIE.pid = bombers[i].bomber_pid;
                    print_output(NULL, &omessage_print_DIE, NULL, NULL);

                    close(bomber_pipes[i][0]);
                    waitpid(bombers[i].bomber_pid, &child_status_bomber[i], 0);
                }
                im incoming_message;
                if (bombers[i].state == ALIVE)
                {
                    if (read_data(bomber_pipes[i][0], &incoming_message) == -1)
                    {
                        perror("read bomber data error");
                        exit(EXIT_FAILURE);
                    }

                    imp imessage_print;
                    imessage_print.m = &incoming_message;
                    imessage_print.pid = bombers[i].bomber_pid;
                    print_output(&imessage_print, NULL, NULL, NULL);

                    int xpos = bombers[i].position.x;
                    int ypos = bombers[i].position.y;
                    om outgoing_message;
                    switch (incoming_message.type)
                    {
                    case BOMBER_START:
                        outgoing_message.type = BOMBER_LOCATION;
                        outgoing_message.data.new_position.x = bombers[i].position.x;
                        outgoing_message.data.new_position.y = bombers[i].position.y;
                        send_message(bomber_pipes[i][0], &outgoing_message);
                        omp omessage_print_START;
                        omessage_print_START.m = &outgoing_message;
                        omessage_print_START.pid = bombers[i].bomber_pid;
                        print_output(NULL, &omessage_print_START, NULL, NULL);
                        break;

                    case BOMBER_SEE:
                        outgoing_message.type = BOMBER_VISION;
                        int obj_count = 0;
                        od objects[MAX_VISION];
                        for (int d = ypos; d < ypos + 4; d++)
                        {
                            if (d == map_height)
                            {
                                break;
                            }
                            if (map[d][xpos].type == CELL_WTH_BOMB)
                            {
                                objects[obj_count].type = BOMB;
                                objects[obj_count].position.x = xpos;
                                objects[obj_count].position.y = d;
                                obj_count++;
                            }
                            else if (map[d][xpos].type == CELL_WITH_BOMB_AND_BOMBER)
                            {
                                objects[obj_count].type = BOMB;
                                objects[obj_count].position.x = xpos;
                                objects[obj_count].position.y = d;
                                obj_count++;
                                if (d != ypos)
                                {
                                    objects[obj_count].type = BOMBER;
                                    objects[obj_count].position.x = xpos;
                                    objects[obj_count].position.y = d;
                                    obj_count++;
                                }
                            }
                            else if (map[d][xpos].type == CELL_WITH_BOMBER)
                            {
                                if (d != ypos)
                                {
                                    objects[obj_count].type = BOMBER;
                                    objects[obj_count].position.x = xpos;
                                    objects[obj_count].position.y = d;
                                    obj_count++;
                                }
                            }
                            else if (map[d][xpos].type == CELL_WITH_OBSTACLE)
                            {
                                objects[obj_count].type = OBSTACLE;
                                objects[obj_count].position.x = xpos;
                                objects[obj_count].position.y = d;
                                obj_count++;
                                break;
                            }
                        }
                        for (int d = ypos - 1; d > ypos - 4; d--)
                        {
                            if (d == -1)
                            {
                                break;
                            }
                            if (map[d][xpos].type == CELL_WTH_BOMB || map[d][xpos].type == CELL_WITH_BOMBER)
                            {
                                objects[obj_count].type = BOMB ? map[d][xpos].type == CELL_WTH_BOMB : BOMBER;
                                objects[obj_count].position.x = xpos;
                                objects[obj_count].position.y = d;
                                obj_count++;
                            }
                            else if (map[d][xpos].type == CELL_WITH_BOMB_AND_BOMBER)
                            {
                                objects[obj_count].type = BOMB;
                                objects[obj_count].position.x = xpos;
                                objects[obj_count].position.y = d;
                                obj_count++;
                                objects[obj_count].type = BOMBER;
                                objects[obj_count].position.x = xpos;
                                objects[obj_count].position.y = d;
                                obj_count++;
                            }
                            else if (map[d][xpos].type == CELL_WITH_OBSTACLE)
                            {
                                objects[obj_count].type = OBSTACLE;
                                objects[obj_count].position.x = xpos;
                                objects[obj_count].position.y = d;
                                obj_count++;
                                break;
                            }
                        }
                        for (int d = xpos + 1; d < xpos + 4; d++)
                        {
                            if (d == map_width)
                            {
                                break;
                            }
                            if (map[ypos][d].type == CELL_WTH_BOMB || map[ypos][d].type == CELL_WITH_BOMBER)
                            {
                                objects[obj_count].type = BOMB ? map[ypos][d].type == CELL_WTH_BOMB : BOMBER;
                                objects[obj_count].position.x = d;
                                objects[obj_count].position.y = ypos;
                                obj_count++;
                            }
                            else if (map[ypos][d].type == CELL_WITH_BOMB_AND_BOMBER)
                            {
                                objects[obj_count].type = BOMB;
                                objects[obj_count].position.x = d;
                                objects[obj_count].position.y = ypos;
                                obj_count++;
                                objects[obj_count].type = BOMBER;
                                objects[obj_count].position.x = d;
                                objects[obj_count].position.y = ypos;
                                obj_count++;
                            }
                            else if (map[ypos][d].type == CELL_WITH_OBSTACLE)
                            {
                                objects[obj_count].type = OBSTACLE;
                                objects[obj_count].position.x = d;
                                objects[obj_count].position.y = ypos;
                                obj_count++;
                                break;
                            }
                        }
                        for (int d = xpos - 1; d > xpos - 4; d--)
                        {
                            if (d == -1)
                            {
                                break;
                            }
                            if (map[ypos][d].type == CELL_WTH_BOMB || map[ypos][d].type == CELL_WITH_BOMBER)
                            {
                                objects[obj_count].type = BOMB ? map[ypos][d].type == CELL_WTH_BOMB : BOMBER;
                                objects[obj_count].position.x = d;
                                objects[obj_count].position.y = ypos;
                                obj_count++;
                            }
                            else if (map[ypos][d].type == CELL_WITH_BOMB_AND_BOMBER)
                            {
                                objects[obj_count].type = BOMB;
                                objects[obj_count].position.x = d;
                                objects[obj_count].position.y = ypos;
                                obj_count++;
                                objects[obj_count].type = BOMBER;
                                objects[obj_count].position.x = d;
                                objects[obj_count].position.y = ypos;
                                obj_count++;
                            }
                            else if (map[ypos][d].type == CELL_WITH_OBSTACLE)
                            {
                                objects[obj_count].type = OBSTACLE;
                                objects[obj_count].position.x = d;
                                objects[obj_count].position.y = ypos;
                                obj_count++;
                                break;
                            }
                        }
                        outgoing_message.data.object_count = obj_count;

                        if (send_message(bomber_pipes[i][0], &outgoing_message) == -1)
                        {
                            perror("BOMBER_SEE send message error");
                            exit(EXIT_FAILURE);
                        }
                        if (send_object_data(bomber_pipes[i][0], obj_count, objects) == -1)
                        {
                            perror("BOMBER_SEE send object data error");
                            exit(EXIT_FAILURE);
                        }

                        omp omessage_print_SEE;
                        omessage_print_SEE.pid = bombers[i].bomber_pid;
                        omessage_print_SEE.m = &outgoing_message;
                        print_output(NULL, &omessage_print_SEE, NULL, objects);

                        break;

                    case BOMBER_MOVE:
                        outgoing_message.type = BOMBER_LOCATION;
                        coordinate target = incoming_message.data.target_position;
                        if (target.x >= 0 && target.x < map_width && target.y >= 0 && target.y < map_height)
                        {
                            if ((target.x == xpos && (target.y == ypos - 1 || target.y == ypos + 1)) ||
                                (target.y == ypos && (target.x == xpos - 1 || target.x == xpos + 1)))
                            {
                                if (map[target.y][target.x].type != CELL_WITH_OBSTACLE || map[target.y][target.x].type != CELL_WITH_BOMBER)
                                {
                                    if (map[target.y][target.x].type == CELL_WTH_BOMB)
                                    {
                                        map[target.y][target.x].type = CELL_WITH_BOMB_AND_BOMBER;
                                        if (map[bombers[i].position.y][bombers[i].position.x].type == CELL_WITH_BOMB_AND_BOMBER)
                                        {
                                            map[bombers[i].position.y][bombers[i].position.x].type = CELL_WTH_BOMB;
                                        }
                                        else
                                        {
                                            map[bombers[i].position.y][bombers[i].position.x].type = EMPTY_CELL;
                                        }
                                    }
                                    else
                                    {
                                        map[target.y][target.x].type = CELL_WITH_BOMBER;
                                        if (map[bombers[i].position.y][bombers[i].position.x].type == CELL_WITH_BOMB_AND_BOMBER)
                                        {
                                            map[bombers[i].position.y][bombers[i].position.x].type = CELL_WTH_BOMB;
                                        }
                                        else
                                        {
                                            map[bombers[i].position.y][bombers[i].position.x].type = EMPTY_CELL;
                                        }
                                    }
                                    bombers[i].position.x = target.x;
                                    bombers[i].position.y = target.y;
                                    outgoing_message.data.new_position = target;
                                }
                            }
                        }
                        else
                        {
                            outgoing_message.data.new_position.x = xpos;
                            outgoing_message.data.new_position.y = ypos;
                        }

                        if (send_message(bomber_pipes[i][0], &outgoing_message) == -1)
                        {
                            perror("BOMBER_MOVE send message error");
                            exit(EXIT_FAILURE);
                        }

                        omp omessage_print_MOVE;
                        omessage_print_MOVE.m = &outgoing_message;
                        omessage_print_MOVE.pid = bombers[i].bomber_pid;
                        print_output(NULL, &omessage_print_MOVE, NULL, NULL);

                        break;

                    case BOMBER_PLANT:
                        if (map[bombers[i].position.y][bombers[i].position.x].type != CELL_WITH_BOMB_AND_BOMBER)
                        {
                            map[bombers[i].position.y][bombers[i].position.x].type = CELL_WITH_BOMB_AND_BOMBER;
                            bombs[bombomber_count].radius = incoming_message.data.bomb_info.radius;
                            bombs[bombomber_count].position = bombers[i].position;

                            pid_t pid_bomb;
                            PIPE(bomb_pipes[bombomber_count]);

                            if (fork() != 0)
                            {
                                close(bomb_pipes[bombomber_count][1]);
                                bombs[bombomber_count].pid = pid_bomb;
                            }
                            else
                            {
                                char interval[10];
                                bombs[bombomber_count].pid = pid_bomb;
                                dup2(bomb_pipes[bombomber_count][1], 1);
                                dup2(bomb_pipes[bombomber_count][1], 0);
                                close(bomb_pipes[bombomber_count][0]);
                                close(bomb_pipes[bombomber_count][1]);
                                sprintf(interval, "%ld", incoming_message.data.bomb_info.interval);
                                execl("./bomb", "./bomb", interval, (char *)NULL);
                                perror("execl");
                            }

                            bombomber_count++;
                            outgoing_message.type = BOMBER_PLANT_RESULT;
                            outgoing_message.data.planted = 1;
                        }
                        else
                        {
                            outgoing_message.type = BOMBER_PLANT_RESULT;
                            outgoing_message.data.planted = 0;
                        }

                        if (send_message(bomber_pipes[i][0], &outgoing_message) == -1)
                        {
                            perror("BOMBER_PLANT send message error");
                            exit(EXIT_FAILURE);
                        }

                        omp omessage_print_PLANT;
                        omessage_print_PLANT.m = &outgoing_message;
                        omessage_print_PLANT.pid = bombers[i].bomber_pid;
                        print_output(NULL, &omessage_print_PLANT, NULL, NULL);

                        break;
                    }
                }
            }
        }

        usleep(1000);
    }
    while (bombs_exploded != bombomber_count)
    {
        for (int i = 0; i < bombomber_count; i++)
        {
            bomb_pollfds[i].fd = bomb_pipes[i][0];
            bomb_pollfds[i].events = POLLIN;
            bomb_pollfds[i].revents = 0;
        }

        if (bombomber_count != 0)
        {
            poll(bomb_pollfds, bombomber_count, 1);
        }

        for (int i = 0; i < bombomber_count; i++)
        {
            if (bomb_pollfds[i].revents & POLLIN)
            {
                im incoming_message;
                if (read_data(bomb_pipes[i][0], &incoming_message) == -1)
                {
                    perror("read bomb data error");
                }

                imp imessage_print;
                imessage_print.m = &incoming_message;
                imessage_print.pid = bombs[i].pid;
                print_output(&imessage_print, NULL, NULL, NULL);

                if (incoming_message.type == BOMB_EXPLODE)
                {
                    unsigned int r = bombs[i].radius;
                    coordinate location = bombs[i].position;
                    for (int d = 0; d < (r + 1); d++)
                    {
                        if ((location.y + d) == map_height)
                        {
                            break;
                        }
                        if (map[location.y + d][location.x].type == CELL_WITH_OBSTACLE)
                        {
                            map[location.y + d][location.x].remaining_durability -= 1;
                            obsd obstacle;
                            obstacle.position.x = location.x;
                            obstacle.position.y = location.y + d;
                            obstacle.remaining_durability = map[location.y + d][location.x].remaining_durability;
                            print_output(NULL, NULL, &obstacle, NULL);
                            if (map[location.y + d][location.x].remaining_durability == 0)
                            {
                                map[location.y + d][location.x].type = EMPTY_CELL;
                            }

                            break;
                        }
                        else if (map[location.y + d][location.x].type == CELL_WITH_BOMBER || map[location.y + d][location.x].type == CELL_WITH_BOMB_AND_BOMBER)
                        {
                            for (int y = 0; y < bomber_count; y++)
                            {
                                if (bombers[y].position.x == location.x && bombers[y].position.y == location.y + d)
                                {
                                    bombers[y].state = DIE;
                                    map[location.y + d][location.x].type = EMPTY_CELL ? map[location.y + d][location.x].type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int d = 1; d < (r + 1); d++)
                    {
                        if ((location.y - d) == -1)
                        {
                            break;
                        }
                        if (map[location.y - d][location.x].type == CELL_WITH_OBSTACLE)
                        {
                            map[location.y - d][location.x].remaining_durability -= 1;
                            if (map[location.y - d][location.x].remaining_durability == 0)
                            {
                                map[location.y - d][location.x].type = EMPTY_CELL;
                            }
                            break;
                        }
                        else if (map[location.y - d][location.x].type == CELL_WITH_BOMBER || map[location.y - d][location.x].type == CELL_WITH_BOMB_AND_BOMBER)
                        {
                            for (int y = 0; y < bomber_count; y++)
                            {
                                if (bombers[y].position.x == location.x && bombers[y].position.y == location.y - d)
                                {
                                    bombers[y].state = DIE;
                                    map[location.y - d][location.x].type = EMPTY_CELL ? map[location.y - d][location.x].type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int d = 0; d < (r + 1); d++)
                    {
                        if ((location.x + d) == map_width)
                        {
                            break;
                        }
                        if (map[location.y][location.x + d].type == CELL_WITH_OBSTACLE)
                        {

                            map[location.y][location.x + d].remaining_durability -= 1;
                            if (map[location.y][location.x + d].remaining_durability == 0)
                            {
                                map[location.y][location.x + d].type = EMPTY_CELL;
                            }
                            break;
                        }
                        else if (map[location.y][location.x + d].type == CELL_WITH_BOMB_AND_BOMBER || map[location.y][location.x + d].type == CELL_WITH_BOMBER)
                        {
                            for (int y = 0; y < bomber_count; y++)
                            {
                                if (bombers[y].position.x == location.x + d && bombers[y].position.y == location.y)
                                {
                                    bombers[y].state = DIE;
                                    map[location.y][location.x + d].type = EMPTY_CELL ? map[location.y][location.x + d].type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int d = 0; d < (r + 1); d++)
                    {
                        if ((location.x - d) == -1)
                        {
                            break;
                        }
                        if (map[location.y][location.x - d].type == CELL_WITH_OBSTACLE)
                        {
                            map[location.y][location.x - d].remaining_durability -= 1;
                            if (map[location.y][location.x - d].remaining_durability == 0)
                            {
                                map[location.y][location.x - d].type = EMPTY_CELL;
                            }
                            break;
                        }

                        else if (map[location.y][location.x - d].type == CELL_WITH_BOMBER || map[location.y][location.x - d].type == CELL_WITH_BOMB_AND_BOMBER)
                        {
                            for (int y = 0; y < bomber_count; y++)
                            {
                                if (bombers[y].position.x == location.x - d && bombers[y].position.y == location.y)
                                {
                                    bombers[y].state = DIE;
                                    map[location.y][location.x - d].type = EMPTY_CELL ? map[location.y][location.x - d].type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    bombs_exploded++;
                    close(bomb_pipes[i][0]);
                    waitpid(bombs[i].pid, &child_status_bomb[i], 0);
                }
            }
        }
    }
    return;
}