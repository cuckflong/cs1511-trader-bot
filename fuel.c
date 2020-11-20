#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "trader_bot.h"
#include "functions.h"
#include<string.h>

int can_reach_fuelstation(struct bot *bt, struct location *l, int size, int negfuel);
double average_fuel_cost(struct bot *bt);
int max_fuel_quantity(struct bot *bt, struct location *f);
int remaining_fuel_quantity(struct bot *bt);
int cannot_reach_fuel_station(struct bot *bt);
int reachable_fuel_station_quantity(struct bot *bt);
int fuel_station_num(struct bot *bt);

//return 1 if the bot will have enough fuel to reach a fuel station after going to that location
//else return 0
int can_reach_fuelstation(struct bot *bt, struct location *l, int size, int negfuel){
    int move = 0;
    move += abs(shortest_distance(bt->location, l));
    struct location *forward_location = l;
    struct location *backward_location = l;
    //make sure the fuel station the bot looks for has sufficient fuel supply
    while((forward_location->type != 3 || move>forward_location->quantity) && (backward_location->type !=3 || move>backward_location->quantity) && move<size){
        move++;
        forward_location = forward_location->next;
        backward_location = backward_location->previous;
    }
    //the bot can no longer reach a fuel station or the quantity in fuel station is not sufficient
    if(bt->fuel>remaining_fuel_quantity(bt) || cannot_reach_fuel_station(bt)==1 || negfuel == fuel_station_num(bt)){
        //printf("FXXKING Yolo!!!  ");
        return 1;
    }
    if(move > bt->fuel){
        return 0;
    }
    else{
        return 1;
    }
}

//calculate the average fuel cost
double average_fuel_cost(struct bot *bt){
    struct location *end_location = bt->location;
    struct location *current_location = bt->location;
    int number_of_fuelstations=0, total_fuel_price=0;
    do
    {
        if(current_location->type == 3){
            number_of_fuelstations++;
            total_fuel_price += current_location->price;
        }
        current_location = current_location->next;
    }while(current_location != end_location);
    return (double)(total_fuel_price)/(double)(number_of_fuelstations);
}

//calculate the maximum quantity of fuel the bot can buy
int max_fuel_quantity(struct bot *bt, struct location *f){
    int fuel_to_max = bt->fuel_tank_capacity - bt->fuel;
    if(fuel_to_max > f->quantity){
        return f->quantity;
    }
    else{
        return fuel_to_max;
    }
}

//the remaining fuel quantity of the world
int remaining_fuel_quantity(struct bot *bt){
    struct location *current_location = bt->location;
    struct location *end_location = bt->location;
    int total_quantity = 0;
    do
    {
        if(current_location->type == 3){
            total_quantity += current_location->quantity;
        }
        current_location = current_location->next;
    }while(current_location != end_location);
    return total_quantity;
}

//return 0 if the bot can reach a fuel station with its fuel remaining
//while the quantity of the fuel station is more than what the bot need to go there
//else return 1
int cannot_reach_fuel_station(struct bot *bt){
    struct location *forward_location = bt->location;
    struct location *backward_location = bt->location;
    int move = 0;
    while(move<=bt->fuel){
        if(forward_location->type==3 && forward_location->quantity>move){
            return 0;
        }
        if(backward_location->type==3 && backward_location->quantity>move){
            return 0;
        }
        move++;
        forward_location = forward_location->next;
        backward_location = backward_location->previous;
    }
    return 1;
}

//check the highest quantity of fuel from a reachable fuel station
int reachable_fuel_station_quantity(struct bot *bt){
    int i=0, fuel_quantity=0;
    struct location *forward_location = bt->location;
    struct location *backward_location = bt->location;
    do
    {
        if(forward_location->type == 3){
            if(forward_location->quantity>fuel_quantity){
                fuel_quantity = forward_location->quantity;
            }
        }
        if(backward_location->type == 3){
            if(backward_location->quantity>fuel_quantity){
                fuel_quantity = backward_location->quantity;
            }
        }
        forward_location = forward_location->next;
        backward_location = backward_location->previous;
        i++;
    }
    while(i<=bt->fuel);
    return fuel_quantity;
}

//the number of fuel station in the world
int fuel_station_num(struct bot *bt){
    struct location *curr = bt->location;
    int number = 0;
    do
    {
        if(curr->type == 3){
            number++;
        }
        curr = curr->next;
    }while(curr != bt->location);
    return number;
}
