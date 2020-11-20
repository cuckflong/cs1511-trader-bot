#include<stdio.h>
#include<stdlib.h>
#include "trader_bot.h"
#include "functions.h"
#include<math.h>
#include<string.h>


void get_action(struct bot *bot, int *action, int *n){
    struct location *current_location = bot->location;
    struct location *end_location = bot->location;
    int typenum = 0, i=0, highest_score=0, highest_index=0, negfuel=0;
    int size = get_map_size(bot);
    int score_list[size][2];
    //give scores to every location
    do
    {
        typenum = current_location->type;
        switch(typenum)
        {
            case 0: //start
                score_list[i][0] = start_other_score(bot, current_location);
                break;
            case 1: //seller
                score_list[i][0] = seller_score(bot, current_location);
                break;
            case 2: //buyer
                score_list[i][0] = buyer_score(bot, current_location);
                break;
            case 3: //fuel station
                score_list[i][0] = fuel_station_score(bot, current_location);
                if(score_list[i][0] < 0){
                    negfuel++;
                }
                break;
            case 4: //dump
                score_list[i][0] = dump_score(bot, current_location);
                break;
            case 5: //other
                score_list[i][0] = start_other_score(bot, current_location);
                break;
        }
        //see if its a legit location
        score_list[i][1] = can_reach_fuelstation(bot, current_location, size, negfuel);
        //printf("[%-2d] Location: %-20s Score: %-7d Legit?: %d\n", i, current_location->name, score_list[i][0], score_list[i][1]);
        current_location = current_location->next;
        i++;
    }while(current_location != end_location);
    //all fuel station got negative score and so making all locations legit
    if(fuel_station_num(bot) == negfuel){
        i=0;
        while(i<size){
            score_list[i][1] = 1;
            i++;
        }
    }
    i=0;
    
    //get the index of the location with the highest score
    while(i<size){
        if(score_list[i][0]>=highest_score && score_list[i][1]==1){
            highest_score = score_list[i][0];
            highest_index = i;
        }
        i++;
    }
    //printf("\n");
    //printf("Highest score: %d [%d]\n", highest_score, highest_index);
    //the bot is on the highest score location if index is 0
    if(highest_index==0){
        //action of buying
        if(bot->location->type == 1 || bot->location->type == 3){
            *action = 1;
            if(bot->location->type == 3){
                *n = max_fuel_quantity(bot, bot->location);
                //prevent buying excess fuel
                if(bot->turns_left <= 2){
                    *n = 0;
                }
            }
            else{
                //prevent excessive buying
                if(abs(shortest_distance(bot->location, most_profit_buyer(bot, bot->location)))>(reachable_fuel_station_quantity(bot)+bot->fuel) || turns_required(bot, abs(shortest_distance(bot->location, most_profit_buyer(bot, bot->location))))+1>=bot->turns_left){
                    *n = 0;
                }
                else{
                    *n = max_availible_quantity(bot, bot->location, most_profit_buyer(bot, bot->location));
                }
            }
        }
        //action of selling
        else{
            *action = 2;
            if(bot->location->type == 2){
                if(bot->cargo != NULL){
                    struct cargo *c = bot->cargo;
                    while(strcmp(c->commodity->name, bot->location->commodity->name)!=0){
                        if(c->next!=NULL){
                            c = c->next;
                        }
                        else{
                            break;
                        }
                    }
                    if(c->quantity > bot->location->quantity){
                        *n = bot->location->quantity;
                    }
                    else{
                        *n = c->quantity;
                    }
                }
            }
            else{
                *n = 0;
            }
        
        }
        //Dump action required
        if(bot->location->type == 4){
            *action = 3;
        }
    }
    //Move to the location with the highest score
    else{
        *action = 0;
        *n = available_move(bot, shortest_distance_index(highest_index, size));
        if(bot->fuel ==0){
            *n = 0;
        }
    }
    //no more profitable move so stand still
    if(highest_index == 0 && highest_score == 0){
        *action = 0;
        *n = 0;
    }
}
