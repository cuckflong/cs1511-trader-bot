#include<stdio.h>
#include "trader_bot.h"
#include<stdlib.h>

int nearest_fuel(struct bot *b);

//find the
//            station
//              v
int nearest_fuel(struct bot *b){
    int front=0, back=0;
    struct location* start_location = b->location;
    struct location* end_location = b->location;
    while((start_location->type != 3) && (end_location->type != 3)){
        front++;
        back--;
        start_location=start_location->next;
        end_location=end_location->previous;
        if(start_location->type == 3){
            return front;
        }
        if(end_location->type ==3){
            return back;
        }
    }
    return 0;
}

