#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "trader_bot.h"
#include "functions.h"
#include<string.h>

int shortest_distance(struct location *s, struct location *b);
int get_map_size(struct bot *bt);
int shortest_distance_index(int index, int size);
int available_move(struct bot* bt, int distance);
struct location *most_profit_buyer(struct bot *bt, struct location *s);
int turns_required(struct bot *bt, int moves);
int total_buyer_quantity(struct location *l);
int buyer_location_bots_num(struct location *l);
int buyer_location_bots_quantity(struct location *l);

//calculate the shortest distance between two locations
int shortest_distance(struct location *s, struct location *b){
    if(s == NULL || b == NULL){
        return 0;
    }
    int distance = 0;
    struct location *forward_location = s;
    struct location *backward_location = s;
    while(forward_location != b && backward_location != b){
        distance++;
        forward_location = forward_location->next;
        backward_location = backward_location->previous;
    }
    if(forward_location == b){
        return distance;
    }
    else{
        return -distance;
    }
}

//location of the most profitable buyer considering multi-bot factors and turns required to reach
struct location *most_profit_buyer(struct bot *bt, struct location *s){
    struct location *current_location = s;
    struct location *highest_profit_location = NULL;
    int fuel_cost = (int)(ceil(average_fuel_cost(bt)));
    int max_profit = 0;
    int profit = 0, distance = 0, turns = 0;
    do
    {
        if(current_location->type==2 && strcmp(current_location->commodity->name, s->commodity->name)==0){
            distance = abs(shortest_distance(s, current_location));
            turns = turns_required(bt, distance);
            //punishment of score by the amount of turns requried to complete the trade
            profit = (max_availible_profit_real(bt, s, current_location)-abs(shortest_distance(s, current_location))*fuel_cost)/turns;
            //check if the bot will get stuck there because of other bots
            if(buyer_location_bots_num(current_location) > current_location->quantity || buyer_location_bots_quantity(current_location) >= current_location->quantity){
                profit = 0;
            }
            //check if the bot has enough fuel and turns to complete the trade
            if(abs(shortest_distance(s, current_location)) + abs(shortest_distance(bt->location, s)) > (reachable_fuel_station_quantity(bt)+bt->fuel) || turns_required(bt, abs(shortest_distance(s, current_location))) + turns_required(bt, abs(shortest_distance(bt->location, s))) + 1 >= bt->turns_left){
                profit = 0;
            }
            //check if the bot can actually earn money
            if(current_location->price <= s->price){
                profit = 0;
            }
            if(profit > max_profit){
                max_profit = profit;
                highest_profit_location = current_location;
            }
        }
        current_location = current_location->next;
    }while(current_location != s);
    return highest_profit_location;
}

//the number of locations
int get_map_size(struct bot *bt){
    struct location *end_location = bt->location;
    struct location *current_location = bt->location;
    int size = 0;
    do
    {
        size++;
        current_location = current_location->next;
    }while(current_location != end_location);
    return size;
}

//calculate the shortest distance to the location base on its index
int shortest_distance_index(int index, int size){
    int foward_index=1, backward_index=size-1, distance=1;
    while(foward_index != index && backward_index != index){
        distance++;
        foward_index++;
        backward_index--;
    }
    if(foward_index==index){
        return distance;
    }
    else{
        return -distance;
    }
}

//calculate the distance the bot should move to the location
int available_move(struct bot* bt, int distance){
    if(distance>0){
        if(distance>bt->maximum_move){
            return bt->maximum_move;
        }
        else{
            return distance;
        }
    }
    else{
        if(distance<-bt->maximum_move){
            return -bt->maximum_move;
        }
        else{
            return distance;
        }
    }
}

//calculate the turns required to complete the given amount of moves
int turns_required(struct bot *bt, int moves){
    if(moves == 0){
        return 1;
    }
    if(moves % bt->maximum_move == 0){
        if(moves/bt->maximum_move == 0){
            return 1;
        }
        return moves/bt->maximum_move;
    }
    return (moves/bt->maximum_move)+1;
}

//calculate the total quantity the buyers of the given location's product have
int total_buyer_quantity(struct location *l){
    struct location *current_location = l;
    int total_quantity = 0;
    do
    {
        if(current_location->type == 2 && strcmp(current_location->commodity->name, l->commodity->name) == 0){
            total_quantity += current_location->quantity;
        }
        current_location = current_location->next;
    }while(current_location != l);
    return total_quantity;
}

//calculate the number of bots with the given location's product on the given location
int buyer_location_bots_num(struct location *l){
    struct bot_list *list = l->bots;
    struct bot *curr_bot = NULL;
    struct cargo *curr_cargo = NULL;
    int botnum = 0;
    while(list != NULL){
        curr_bot = list->bot;
        curr_cargo = curr_bot->cargo;
        while(curr_cargo != NULL){
            if(strcmp(curr_cargo->commodity->name, l->commodity->name) == 0){
                botnum++;
            }
            curr_cargo = curr_cargo->next;
        }
        list = list->next;
    }
    return botnum;
}

//calculate the total quantity of the given location's product among all bots on the given location
int buyer_location_bots_quantity(struct location *l){
    struct bot_list *list = l->bots;
    struct bot *curr_bot = NULL;
    struct cargo *curr_cargo = NULL;
    int total_quantity = 0;
    while(list != NULL){
        curr_bot = list->bot;
        curr_cargo = curr_bot->cargo;
        while(curr_cargo != NULL){
            if(strcmp(curr_cargo->commodity->name, l->commodity->name) == 0){
                total_quantity += curr_cargo->quantity;
            }
            curr_cargo = curr_cargo->next;
        }
        list = list->next;
    }
    return total_quantity;
}
