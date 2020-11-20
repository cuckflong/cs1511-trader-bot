#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "trader_bot.h"
#include "functions.h"
#include <string.h>

int max_availible_quantity(struct bot *bt, struct location *s, struct location *b);
int selling_profit(struct bot *bt, struct location *b);
int max_availible_profit_real(struct bot *bt, struct location *s, struct location *b);


//the maximum profit the bot can from the given buyer and seller
int max_availible_profit_real(struct bot *bt, struct location *s, struct location *b){
    int maxquantity = max_availible_quantity(bt, s, b);
    if(maxquantity < 0){
        maxquantity = 0;
    }
    //printf("maxquantity: %d\n", maxquantity);
    //printf("buyer price: %d, seller price: %d\n", b->price, s->price);
    return maxquantity*(b->price-s->price);
}

//the maximum quantity of product the bot can buy from a seller considering the demand of the given buyer
int max_availible_quantity(struct bot *bt, struct location *s, struct location *b){
    if(b == NULL){
        return 0;
    }
    int product_quantitynum = product_quantity(bt, b);
    int maxquantity = max_quantity(bt, s);
    if(maxquantity > (b->quantity - buyer_location_bots_quantity(b))){
        return (b->quantity - buyer_location_bots_quantity(b)) - product_quantitynum;
    }
    else{
        return maxquantity-product_quantitynum;
    }
}

//the profit from selling to the given buyer based on the bot's cargo
int selling_profit(struct bot *bt, struct location *b){
    if(bt->cargo == NULL){
        return 0;
    }
    struct cargo *c = bt->cargo;
    int profit = 0;
    while(strcmp(c->commodity->name, b->commodity->name)!=0){
        if(c->next!=NULL){
            c = c->next;
        }
        else{
            break;
        }
    }
    if(strcmp(c->commodity->name, b->commodity->name)==0){
        if(c->quantity > b->quantity){
            return (b->quantity)*(b->price);
        }
        else{
            return (c->quantity)*(b->price);
        }
    }
    else{
        return 0;
    }
}
