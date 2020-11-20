#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "trader_bot.h"
#include "functions.h"
#include <string.h>

int buyer_score(struct bot *bt, struct location *b);
int seller_score(struct bot *bt, struct location *s);
int start_other_score(struct bot *bt, struct location *l);
int dump_score(struct bot *bt, struct location *l);
int fuel_station_score(struct bot *bt, struct location *f);

int buyer_score(struct bot *bt, struct location *b){
    int fuel_cost = (int)(ceil(average_fuel_cost(bt)));
    int buyer_distance = abs(shortest_distance(bt->location, b));
    int turns = turns_required(bt, buyer_distance);
    int profit = selling_profit(bt, b);
    //check if the bot has enough fuel and turns to complete the trade
    if(abs(shortest_distance(b, bt->location)) > (reachable_fuel_station_quantity(bt)+bt->fuel) || turns_required(bt, abs(shortest_distance(b, bt->location))) + 1 > bt->turns_left){
        return-(buyer_distance)*fuel_cost;
    }
    //check if the bot will get stuck because of other bots
    if(buyer_location_bots_num(b) > b->quantity || buyer_location_bots_quantity(b)-product_quantity(bt, b) >= b->quantity){
        return -fuel_cost*buyer_distance;
    }
    //punishment on the score by the turns required to complete the trade
    int score = (profit-fuel_cost*buyer_distance)/turns;
    return score;
}

int seller_score(struct bot *bt, struct location *s){
    int fuel_cost = (int)(ceil(average_fuel_cost(bt)));
    struct location *buyer = most_profit_buyer(bt, s);
    int turns = 0;
    int seller_distance = abs(shortest_distance(bt->location, s));
    turns += turns_required(bt, seller_distance);
    //no profitable buyer
    if(buyer == NULL){
        return 0-seller_distance*fuel_cost;
    }
    int buyer_distance = abs(shortest_distance(s, buyer));
    turns += turns_required(bt, buyer_distance);
    int profit = max_availible_profit_real(bt, s, buyer);
    //extra punishment by the turns required to go from seller to buyer
    int score = ((profit-(buyer_distance+seller_distance)*fuel_cost)/(turns+turns_required(bt, buyer_distance)-1));
    //check if the bot has enough fuel and turns to complete the trade
    if(abs(shortest_distance(s, most_profit_buyer(bt, s))) + abs(shortest_distance(bt->location, s)) > (reachable_fuel_station_quantity(bt)+bt->fuel) || turns_required(bt, abs(shortest_distance(s, most_profit_buyer(bt, s)))) + turns_required(bt, abs(shortest_distance(bt->location, s))) + 1 >= bt->turns_left){
         return-(buyer_distance+seller_distance)*fuel_cost;
    }
    //prevent buying if the total quantity others bots are holding is already more than the demand of all buyers
    if(total_buyer_quantity(s) <= all_bots_quantity(s)-product_quantity(bt, s)){
        return -(buyer_distance+seller_distance)*fuel_cost;
    }
    return score;
}

int fuel_station_score(struct bot *bt, struct location *f){
    int fuel_cost = (int)(ceil(average_fuel_cost(bt)));
    int score = 0;
    int fuel_to_max = max_fuel_quantity(bt, f);
    int distance = abs(shortest_distance(bt->location, f));
    if(fuel_to_max + bt->fuel > (bt->turns_left-1)*bt->maximum_move){
        fuel_to_max = (bt->turns_left-1)*bt->maximum_move - bt->fuel;
    }
    //prevent giving extra score to fuel station with low fuel quantity
    if(f->quantity > distance){
        score = ((fuel_to_max+distance)*f->price - distance*fuel_cost)/(turns_required(bt, distance)*(bt->fuel+1));
    }
    else{
        score = (fuel_to_max*f->price - distance*fuel_cost)/(turns_required(bt, distance)*(bt->fuel+1));
    }
    //punishment too high causing the score down to zero but don't worry, legit will solve it
    if(score == 0){
        score = 1;
    }
    return score;
}

//waste of time
int start_other_score(struct bot *bt, struct location *l){
    int fuel_cost = (int)(ceil(average_fuel_cost(bt)));
    int distance = abs(shortest_distance(bt->location, l));
    return -fuel_cost*distance;
}

int dump_score(struct bot *bt, struct location *l){
    //try to dump as soon as possible
    if(need_dump(bt)){
        return 2147483647;
    }
    int fuel_cost = (int)(ceil(average_fuel_cost(bt)));
    int distance = abs(shortest_distance(bt->location, l));
    return -fuel_cost*distance;
}
