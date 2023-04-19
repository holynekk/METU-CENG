#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/wait.h>

#include "logging.h"
#include "message.h"
#include "constants.h"

void assign_od_objects(od object, int cell_type, int p_x, int p_y, int *object_count) {
    object.position.x = p_x;
    object.position.y = p_y;
    object.type = cell_type;
    *object_count += 1; // !!!! Important !!!!
}

int main(void) {
    int map_width, map_height, obstacle_count, bomber_count;
    scanf("%d %d %d %d", &map_width, &map_height, &obstacle_count, &bomber_count);

    obj map[map_height][map_width];
    for (int i = 0; i < map_height; i++) {
        for (int j = 0; j < map_width; j++) {
            map[i][j].type = EMPTY_CELL;
        }
    }
    int obstacle_x, obstacle_y, obstacle_durability;
    for (int i = 0; i < obstacle_count; i++) {
        scanf("%d %d %d", &obstacle_x, &obstacle_y, &obstacle_durability);
        map[obstacle_y][obstacle_x].type = CELL_WITH_OBSTACLE;
        map[obstacle_y][obstacle_x].remaining_durability = obstacle_durability;
    }

    bomb bombs[64];
    bomber bombers[bomber_count];

    char bmbr_exc[bomber_count][64];
    char dummy_args[10][10];
    char *bomber_args[bomber_count][24];

    int bomber_pipes[bomber_count][2], bomb_pipes[64][2], child_process_bomb[64], child_process_bomber[bomber_count];

    for (int i = 0; i < bomber_count; i++) {
        bombers[i].status = ALIVE;
        PIPE(bomber_pipes[i]);
        scanf("%d %d %d", &bombers[i].position.x, &bombers[i].position.y, &bombers[i].arg_count);
        map[bombers[i].position.y][bombers[i].position.x].type = CELL_WITH_BOMBER;
        scanf("%s", bmbr_exc[i]);
        bomber_args[i][0] = bmbr_exc[i];
        for (int j = 1; j < bombers[i].arg_count; j++) {
            scanf("%s", dummy_args[j]);
            bomber_args[i][j] = (char *) malloc(sizeof(char) * (strlen(*(dummy_args + j)) + 1));
            strcpy(bomber_args[i][j], dummy_args[j]);
        }
        bomber_args[i][bombers[i].arg_count] = NULL;
        if (fork() != 0) {
            bombers[i].bomber_pid = getpid();
            close(bomber_pipes[i][1]);
        } else {
            bombers[i].bomber_pid = getpid();
            dup2(bomber_pipes[i][1], 1);
            dup2(bomber_pipes[i][1], 0);
            close(bomber_pipes[i][0]);
            close(bomber_pipes[i][1]);
            execv(bmbr_exc[i], bomber_args[i]);
        }
    }

    struct pollfd bomber_polls[bomber_count], bomb_polls[64];
    int curr_bomber = 0, bombs_exploded = 0, num_bombers_alive = bomber_count;

    while (num_bombers_alive) {
        for (int i = 0; i < curr_bomber; i++) {
            bomb_polls[i].revents = 0;
            bomb_polls[i].events = POLLIN;
            bomb_polls[i].fd = bomb_pipes[i][0];
        }
        if (curr_bomber != 0) {
            poll(bomb_polls, curr_bomber, 1);
        }
        for (int i = 0; i < curr_bomber; i++) {
            if (bomb_polls[i].revents & POLLIN) {
                im incmng_msg;
                if (read_data(bomb_pipes[i][0], &incmng_msg) == -1) {
                    perror("[FAIL] Bomb data could not read!");
                }
                imp incmng_msg_prnt = {.m = &incmng_msg, .pid = bombs[i].pid};
                print_output(&incmng_msg_prnt, NULL, NULL, NULL);
                if (incmng_msg.type == BOMB_EXPLODE) {
                    coordinate location = bombs[i].position;
                    int radius = bombs[i].radius;
                    obj m_obj;
                    for (int distance = 0; distance < (radius + 1) && (location.y + distance) != map_height; distance++) {
                        m_obj = map[location.y + distance][location.x];
                        if (m_obj.type == CELL_WITH_OBSTACLE) {
                            m_obj.remaining_durability -= 1;
                            obsd obstacle = {.position.x = location.x, .position.y = location.y + distance, .remaining_durability = m_obj.remaining_durability};
                            print_output(NULL, NULL, &obstacle, NULL);
                            if (m_obj.remaining_durability == 0) {
                                m_obj.type = EMPTY_CELL;
                            }
                            break;
                        } else if (m_obj.type == CELL_WITH_BOMBER || m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                            for (int y = 0; y < bomber_count; y++) {
                                if (bombers[y].position.x == location.x && bombers[y].position.y == location.y + distance) {
                                    bombers[y].status = DIE;
                                    m_obj.type = EMPTY_CELL ? m_obj.type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int distance = 1; distance < (radius + 1) && (location.y - distance) != -1; distance++) {
                        m_obj = map[location.y - distance][location.x];
                        if (m_obj.type == CELL_WITH_OBSTACLE) {
                            if (--m_obj.remaining_durability == 0) {
                                m_obj.type = EMPTY_CELL;
                            }
                            break;
                        }
                        else if (m_obj.type == CELL_WITH_BOMBER || m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                            for (int y = 0; y < bomber_count; y++) {
                                if (bombers[y].position.x == location.x && bombers[y].position.y == location.y - distance) {
                                    bombers[y].status = DIE;
                                    m_obj.type = EMPTY_CELL ? m_obj.type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int distance = 0; distance < (radius + 1) && (location.x - distance) != -1; distance++) {
                        m_obj = map[location.y][location.x - distance];
                        if (m_obj.type == CELL_WITH_OBSTACLE) {
                            if (--m_obj.remaining_durability == 0) {
                                m_obj.type = EMPTY_CELL;
                            }
                            break;
                        } else if (m_obj.type == CELL_WITH_BOMBER || m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                            for (int y = 0; y < bomber_count; y++) {
                                if (bombers[y].position.x == location.x - distance && bombers[y].position.y == location.y) {
                                    bombers[y].status = DIE;
                                    m_obj.type = EMPTY_CELL ? m_obj.type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int distance = 0; distance < (radius + 1) && (location.x + distance) != map_width; distance++) {
                        m_obj = map[location.y][location.x + distance];
                        if (m_obj.type == CELL_WITH_OBSTACLE) {
                            if (--m_obj.remaining_durability == 0) {
                                m_obj.type = EMPTY_CELL;
                            }
                            break;
                        } else if (m_obj.type == CELL_WITH_BOMBER || m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                            for (int y = 0; y < bomber_count; y++) {
                                if (bombers[y].position.x == location.x + distance && bombers[y].position.y == location.y) {
                                    bombers[y].status = DIE;
                                    m_obj.type = EMPTY_CELL ? m_obj.type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    bombs_exploded++;
                    close(bomb_pipes[i][0]);
                    waitpid(bombs[i].pid, &child_process_bomb[i], 0);
                }
            }
        }

        for (int i = 0; i < bomber_count; i++) {
            bomber_polls[i].fd = bomber_pipes[i][0];
            bomber_polls[i].events = POLLIN;
            bomber_polls[i].revents = 0;
        }
        poll(bomber_polls, bomber_count, -1);

        for (int i = 0; i < bomber_count; i++) {
            if (num_bombers_alive == 1) {
                num_bombers_alive = 0;
                om otgng_msg = {.type = BOMBER_WIN};
                omp otgng_msg_prnt = {.m = &otgng_msg, .pid = bombers[i].bomber_pid};
                if (send_message(bomber_pipes[i][0], &otgng_msg) == -1) {
                    perror("[FAIL] Error while sending message with BOMBER_WIN status!");
                    exit(EXIT_FAILURE);
                }
                print_output(NULL, &otgng_msg_prnt, NULL, NULL);
                close(bomber_pipes[i][0]);
                waitpid(bombers[i].bomber_pid, &child_process_bomber[i], 0);
                break;
            }

            if ((bomber_polls[i].revents & POLLIN) && bombers[i].status != KILLED) {
                if (bombers[i].status == DIE) {
                    om otgng_msg = {.type = BOMBER_DIE};
                    bombers[i].status = KILLED;
                    num_bombers_alive--;
                    if (send_message(bomber_pipes[i][0], &otgng_msg) == -1)
                    {
                        perror("[FAIL] Error while sending message with BOMBER_DIE status!");
                        exit(EXIT_FAILURE);
                    }
                    omp otgng_msg_prnt = {.m = &otgng_msg, .pid = bombers[i].bomber_pid};
                    print_output(NULL, &otgng_msg_prnt, NULL, NULL);
                    close(bomber_pipes[i][0]);
                    waitpid(bombers[i].bomber_pid, &child_process_bomber[i], 0);
                }
                im incmng_msg;
                if (bombers[i].status == ALIVE) {
                    om otgng_msg;
                    if (read_data(bomber_pipes[i][0], &incmng_msg) == -1) {
                        perror("[FAIL] Bomber data could not read!");
                        exit(EXIT_FAILURE);
                    }
                    imp incmng_msg_prnt = {.m = &incmng_msg, .pid = bombers[i].bomber_pid};
                    print_output(&incmng_msg_prnt, NULL, NULL, NULL);
                    int bomber_position_x = bombers[i].position.x, bomber_position_y = bombers[i].position.y;
                    switch (incmng_msg.type) {
                        case BOMBER_START:
                            otgng_msg.type = BOMBER_LOCATION;
                            otgng_msg.data.new_position.x = bombers[i].position.x;
                            otgng_msg.data.new_position.y = bombers[i].position.y;
                            send_message(bomber_pipes[i][0], &otgng_msg);
                            omp omessage_print_START = {.m = &otgng_msg, .pid = bombers[i].bomber_pid};
                            print_output(NULL, &omessage_print_START, NULL, NULL);
                            break;

                        case BOMBER_SEE:
                            otgng_msg.type = BOMBER_VISION;
                            int obj_count = 0;
                            od objects[64];
                            obj m_obj;
                            for (int distance = bomber_position_y; distance < bomber_position_y + 4 && distance != map_height; distance++) {
                                m_obj = map[distance][bomber_position_x];
                                if (m_obj.type == CELL_WTH_BOMB) {
                                    assign_od_objects(objects[obj_count], BOMB, bomber_position_x, distance, &obj_count);
                                }
                                else if (m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                                    assign_od_objects(objects[obj_count], BOMB, bomber_position_x, distance, &obj_count);
                                    if (distance != bomber_position_y) {
                                        assign_od_objects(objects[obj_count], BOMBER, bomber_position_x, distance, &obj_count);
                                    }
                                } else if (m_obj.type == CELL_WITH_BOMBER) {
                                    if (distance != bomber_position_y) {
                                        assign_od_objects(objects[obj_count], BOMBER, bomber_position_x, distance, &obj_count);
                                    }
                                } else if (m_obj.type == CELL_WITH_OBSTACLE) {
                                    assign_od_objects(objects[obj_count], OBSTACLE, bomber_position_x, distance, &obj_count);
                                    break;
                                }
                            }
                            for (int distance = bomber_position_y - 1; distance > bomber_position_y - 4 && distance != -1; distance--) {
                                m_obj = map[distance][bomber_position_x];
                                if (m_obj.type == CELL_WTH_BOMB || m_obj.type == CELL_WITH_BOMBER) {
                                    assign_od_objects(objects[obj_count], BOMB ? m_obj.type == CELL_WTH_BOMB : BOMBER, bomber_position_x, distance, &obj_count);
                                } else if (m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                                    assign_od_objects(objects[obj_count], BOMB, bomber_position_x, distance, &obj_count);
                                    assign_od_objects(objects[obj_count], BOMBER, bomber_position_x, distance, &obj_count);
                                } else if (m_obj.type == CELL_WITH_OBSTACLE) {
                                    assign_od_objects(objects[obj_count], OBSTACLE, bomber_position_x, distance, &obj_count);
                                    break;
                                }
                            }
                            for (int distance = bomber_position_x - 1; distance > bomber_position_x - 4 && distance != -1; distance--) {
                                m_obj = map[bomber_position_y][distance];
                                if (m_obj.type == CELL_WTH_BOMB || m_obj.type == CELL_WITH_BOMBER) {
                                    assign_od_objects(objects[obj_count], BOMB ? m_obj.type == CELL_WTH_BOMB : BOMBER, distance, bomber_position_y, &obj_count);
                                } else if (m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                                    assign_od_objects(objects[obj_count], BOMB, distance, bomber_position_y, &obj_count);
                                    assign_od_objects(objects[obj_count], BOMBER, distance, bomber_position_y, &obj_count);
                                } else if (m_obj.type == CELL_WITH_OBSTACLE) {
                                    assign_od_objects(objects[obj_count], OBSTACLE, distance, bomber_position_y, &obj_count);
                                    break;
                                }
                            }
                            for (int distance = bomber_position_x + 1; distance < bomber_position_x + 4 && distance != map_width; distance++) {
                                m_obj = map[bomber_position_y][distance];
                                if (m_obj.type == CELL_WTH_BOMB || m_obj.type == CELL_WITH_BOMBER) {
                                    assign_od_objects(objects[obj_count], BOMB ? m_obj.type == CELL_WTH_BOMB : BOMBER, distance, bomber_position_y, &obj_count);
                                } else if (m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                                    assign_od_objects(objects[obj_count], BOMB, distance, bomber_position_y, &obj_count);
                                    assign_od_objects(objects[obj_count], BOMBER, distance, bomber_position_y, &obj_count);
                                } else if (m_obj.type == CELL_WITH_OBSTACLE) {
                                    assign_od_objects(objects[obj_count], OBSTACLE, distance, bomber_position_y, &obj_count);
                                    break;
                                }
                            }
                            otgng_msg.data.object_count = obj_count;

                            if (send_message(bomber_pipes[i][0], &otgng_msg) == -1) {
                                perror("[FAIL] Error while sending message with BOMBER_SEE status!");
                                exit(EXIT_FAILURE);
                            }
                            if (send_object_data(bomber_pipes[i][0], obj_count, objects) == -1) {
                                perror("[FAIL] Error while sending object data with BOMBER_SEE status!");
                                exit(EXIT_FAILURE);
                            }

                            omp omessage_print_SEE = {.m = &otgng_msg, .pid = bombers[i].bomber_pid};
                            print_output(NULL, &omessage_print_SEE, NULL, objects);

                            break;

                        case BOMBER_MOVE:
                            otgng_msg.type = BOMBER_LOCATION;
                            coordinate target_position = incmng_msg.data.target_position;
                            if (target_position.x >= 0 && target_position.y >= 0 && target_position.x < map_width && target_position.y < map_height) {
                                if ((target_position.x == bomber_position_x && (target_position.y == bomber_position_y + 1 || target_position.y == bomber_position_y - 1)) ||
                                    (target_position.y == bomber_position_y && (target_position.x == bomber_position_x + 1 ||target_position.x == bomber_position_x - 1))) {
                                    if (map[target_position.y][target_position.x].type != CELL_WITH_OBSTACLE || map[target_position.y][target_position.x].type != CELL_WITH_BOMBER) {
                                        if (map[target_position.y][target_position.x].type == CELL_WTH_BOMB) {
                                            map[target_position.y][target_position.x].type = CELL_WITH_BOMB_AND_BOMBER;
                                            if (map[bombers[i].position.y][bombers[i].position.x].type == CELL_WITH_BOMB_AND_BOMBER) {
                                                map[bombers[i].position.y][bombers[i].position.x].type = CELL_WTH_BOMB;
                                            } else {
                                                map[bombers[i].position.y][bombers[i].position.x].type = EMPTY_CELL;
                                            }
                                        } else {
                                            map[target_position.y][target_position.x].type = CELL_WITH_BOMBER;
                                            if (map[bombers[i].position.y][bombers[i].position.x].type == CELL_WITH_BOMB_AND_BOMBER) {
                                                map[bombers[i].position.y][bombers[i].position.x].type = CELL_WTH_BOMB;
                                            } else {
                                                map[bombers[i].position.y][bombers[i].position.x].type = EMPTY_CELL;
                                            }
                                        }
                                        bombers[i].position.x = target_position.x;
                                        bombers[i].position.y = target_position.y;
                                        otgng_msg.data.new_position = target_position;
                                    }
                                }
                            } else {
                                otgng_msg.data.new_position.x = bomber_position_x;
                                otgng_msg.data.new_position.y = bomber_position_y;
                            }
                            if (send_message(bomber_pipes[i][0], &otgng_msg) == -1) {
                                perror("[FAIL] Error while sending message with BOMBER_MOVE status!");
                                exit(EXIT_FAILURE);
                            }
                            omp omessage_print_MOVE = {.m = &otgng_msg, .pid = bombers[i].bomber_pid};
                            print_output(NULL, &omessage_print_MOVE, NULL, NULL);
                            break;

                        case BOMBER_PLANT:
                            if (map[bombers[i].position.y][bombers[i].position.x].type != CELL_WITH_BOMB_AND_BOMBER) {
                                map[bombers[i].position.y][bombers[i].position.x].type = CELL_WITH_BOMB_AND_BOMBER;
                                bombs[curr_bomber].radius = incmng_msg.data.bomb_info.radius;
                                bombs[curr_bomber].position = bombers[i].position;

                                PIPE(bomb_pipes[curr_bomber]);

                                if (fork() != 0) {
                                    close(bomb_pipes[curr_bomber][1]);
                                    bombs[curr_bomber].pid = getpid();
                                } else {
                                    char interval[10];
                                    bombs[curr_bomber].pid = getpid();
                                    dup2(bomb_pipes[curr_bomber][1], 1);
                                    dup2(bomb_pipes[curr_bomber][1], 0);
                                    close(bomb_pipes[curr_bomber][0]);
                                    close(bomb_pipes[curr_bomber][1]);
                                    sprintf(interval, "%ld", incmng_msg.data.bomb_info.interval);
                                    execl("./bomb", "./bomb", interval, (char *) NULL);
                                    perror("execl");
                                }
                                curr_bomber++;
                                otgng_msg.data.planted = 1;
                            } else {
                                otgng_msg.data.planted = 0;
                            }
                            otgng_msg.type = BOMBER_PLANT_RESULT;
                            if (send_message(bomber_pipes[i][0], &otgng_msg) == -1) {
                                perror("[FAIL] Error while sending message with BOMBER_PLANT status!");
                                exit(EXIT_FAILURE);
                            }
                            omp omessage_print_PLANT = {.m = &otgng_msg, .pid = bombers[i].bomber_pid};
                            print_output(NULL, &omessage_print_PLANT, NULL, NULL);
                            break;
                    }
                }
            }
        }
        usleep(1000);
    }
    while (bombs_exploded != curr_bomber) {
        for (int i = 0; i < curr_bomber; i++) {
            bomb_polls[i].revents = 0;
            bomb_polls[i].events = POLLIN;
            bomb_polls[i].fd = bomb_pipes[i][0];
        }
        if (curr_bomber != 0) {
            poll(bomb_polls, curr_bomber, 1);
        }
        for (int i = 0; i < curr_bomber; i++) {
            if (bomb_polls[i].revents & POLLIN) {
                im incmng_msg;
                if (read_data(bomb_pipes[i][0], &incmng_msg) == -1) {
                    perror("[FAIL] Bomb data could not read!");
                }
                imp incmng_msg_prnt = {.m = &incmng_msg, .pid = bombs[i].pid};
                print_output(&incmng_msg_prnt, NULL, NULL, NULL);
                if (incmng_msg.type == BOMB_EXPLODE) {
                    int radius = bombs[i].radius;
                    coordinate b_location = bombs[i].position;
                    for (int distance = 0; distance < (radius + 1) && (b_location.y + distance) != map_height; distance++) {
                        obj m_obj = map[b_location.y + distance][b_location.x];
                        if (m_obj.type == CELL_WITH_OBSTACLE) {
                            m_obj.remaining_durability -= 1;
                            m_obj.type = EMPTY_CELL ? m_obj.remaining_durability == 0 : m_obj.type;
                            obsd obstacle = {.position.x = b_location.x, .position.y = b_location.y + distance, .remaining_durability = m_obj.remaining_durability};
                            print_output(NULL, NULL, &obstacle, NULL);
                            break;
                        } else if (m_obj.type == CELL_WITH_BOMBER || m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                            for (int y = 0; y < bomber_count; y++) {
                                if (bombers[y].position.x == b_location.x && bombers[y].position.y == b_location.y + distance) {
                                    bombers[y].status = DIE;
                                    m_obj.type = EMPTY_CELL ? m_obj.type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int distance = 1; distance < (radius + 1) && (b_location.y - distance) != -1; distance++) {
                        obj m_obj = map[b_location.y - distance][b_location.x];
                        if (m_obj.type == CELL_WITH_OBSTACLE) {
                            if (--m_obj.remaining_durability == 0) {
                                m_obj.type = EMPTY_CELL;
                            }
                            break;
                        } else if (m_obj.type == CELL_WITH_BOMBER || m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                            for (int y = 0; y < bomber_count; y++) {
                                if (bombers[y].position.x == b_location.x && bombers[y].position.y == b_location.y - distance) {
                                    bombers[y].status = DIE;
                                    m_obj.type = EMPTY_CELL ? m_obj.type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int distance = 0; distance < (radius + 1) && (b_location.x - distance) != -1; distance++) {
                        obj m_obj = map[b_location.y][b_location.x - distance];
                        if (m_obj.type == CELL_WITH_OBSTACLE) {
                            if (--m_obj.remaining_durability == 0) {
                                m_obj.type = EMPTY_CELL;
                            }
                            break;
                        } else if (m_obj.type == CELL_WITH_BOMBER || m_obj.type == CELL_WITH_BOMB_AND_BOMBER) {
                            for (int y = 0; y < bomber_count; y++) {
                                if (bombers[y].position.x == b_location.x - distance && bombers[y].position.y == b_location.y) {
                                    bombers[y].status = DIE;
                                    m_obj.type = EMPTY_CELL ? m_obj.type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    for (int distance = 0; distance < (radius + 1) && (b_location.x + distance) != map_width; distance++) {
                        obj m_obj = map[b_location.y][b_location.x + distance];
                        if (m_obj.type == CELL_WITH_OBSTACLE) {
                            if (--m_obj.remaining_durability == 0) {
                                m_obj.type = EMPTY_CELL;
                            }
                            break;
                        } else if (m_obj.type == CELL_WITH_BOMB_AND_BOMBER || m_obj.type == CELL_WITH_BOMBER) {
                            for (int y = 0; y < bomber_count; y++) {
                                if (bombers[y].position.x == b_location.x + distance && bombers[y].position.y == b_location.y) {
                                    bombers[y].status = DIE;
                                    m_obj.type = EMPTY_CELL ? m_obj.type == CELL_WITH_BOMBER : CELL_WTH_BOMB;
                                }
                            }
                        }
                    }
                    bombs_exploded++;
                    close(bomb_pipes[i][0]);
                    waitpid(bombs[i].pid, &child_process_bomb[i], 0);
                }
            }
        }
    }
    return 0;
}