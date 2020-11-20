#include<stdio.h>
#include<stdlib.h>
#include "trader_bot.h"
#include "functions.h"
#include<math.h>
#include<string.h>

int max_quantity(struct bot *b, struct location *l);
int cargo_rweight(struct bot *b);
int cargo_rvolume(struct bot *b);
int product_quantity(struct bot *bt, struct location *l);
int need_dump(struct bot *bt);
int all_bots_quantity(struct location *l);

//calculate the maximum quantity of the location's product the bot can hold/buy
int max_quantity(struct bot *b, struct location *l){
    int maxquantity=0, temp=0, rweight=cargo_rweight(b), rvolume=cargo_rvolume(b);
    temp = b->cash/l->price;
    if(temp >= l->quantity){
        maxquantity = l->quantity;
    }
    else{
        maxquantity = temp;
    }
    temp = rweight/l->commodity->weight;
    if(temp<maxquantity){
        maxquantity = temp;
    }
    temp = rvolume/l->commodity->volume;
    if(temp<maxquantity){
        maxquantity = temp;
    }
    return maxquantity;
}

//calculate cargo's remaining weight
int cargo_rweight(struct bot *b){
    if(b->cargo == NULL){
        return b->maximum_cargo_weight;
    }
    int totalweight=0;
    struct cargo* currentcargo = b->cargo;
    struct cargo* endcargo = b->cargo;
    do{
        totalweight += (currentcargo->quantity)*(currentcargo->commodity->weight);
        if(currentcargo->next!=NULL){
            currentcargo = currentcargo->next;
        }
        else{
            break;
        }
    }while(currentcargo != endcargo);
    return b->maximum_cargo_weight-totalweight;
}

//calculate cargo's remaining volume
int cargo_rvolume(struct bot *b){
    if(b->cargo == NULL){
        return b->maximum_cargo_volume;
    }
    int totalvolume=0;
    struct cargo* currentcargo = b->cargo;
    struct cargo* endcargo = b->cargo;
    do{
        totalvolume += (currentcargo->quantity)*(currentcargo->commodity->volume);
        if(currentcargo->next!=NULL){
            currentcargo = currentcargo->next;
        }
        else{
            break;
        }
    }while(currentcargo != endcargo);
    return b->maximum_cargo_volume-totalvolume;
}

//check the cargo's quantity of a given location's product
int product_quantity(struct bot *bt, struct location *l){
    int product_quantity = 0;
    if(bt->cargo == NULL){
        return 0;
    }
    struct cargo *currentcargo = bt->cargo;
    do{
        if(strcmp(currentcargo->commodity->name, l->commodity->name) == 0){
            product_quantity = currentcargo->quantity;
        }
        currentcargo = currentcargo->next;
    }while(currentcargo != NULL);
    return product_quantity;
}

//check if the bot needs to dump base on how many quantity remaining from all buyers
int need_dump(struct bot *bt){
    if(bt->cargo == NULL){
        return 0;
    }
    struct cargo *currentcargo = bt->cargo;
    struct location *current_location = bt->location;
    int total_quantity = 0;
    do
    {
        current_location = bt->location;
        total_quantity = 0;
        do
        {
            if(current_location->type == 2 && strcmp(current_location->commodity->name, currentcargo->commodity->name) == 0){
                total_quantity += current_location->quantity;
            }
            current_location = current_location->next;
        }while(current_location != bt->location);
        if(total_quantity<currentcargo->quantity/2){
            return 1;
        }
        currentcargo = currentcargo->next;
    }while(currentcargo != NULL);
    return 0;
}

//check the total quantity of all bots of the given location's product
int all_bots_quantity(struct location *l){
    struct bot *curr_bot = NULL;
    struct cargo *currentcargo = NULL;
    struct location *current_location = l;
    struct bot_list *list = NULL;
    int all_quantity = 0;
    do{
        list = current_location->bots;
        while(list != NULL){
            curr_bot = list->bot;
            currentcargo = curr_bot->cargo;
            while(currentcargo != NULL){
                if(strcmp(currentcargo->commodity->name, l->commodity->name) == 0){
                    all_quantity += currentcargo->quantity;
                }
                currentcargo = currentcargo->next;
            }
            list = list->next;
        }
        current_location = current_location->next;
    }while(current_location != l);
    return all_quantity;
}
