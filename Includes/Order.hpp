#pragma once

#include "../include.hpp"

struct Order 
{
    int id;
    std::string type;
    int volume;
    int price;
    int client_id;

    Order(int id, std::string type, int volume, int price, int client_id)
        : id(id), type(type), volume(volume), price(price), client_id(client_id) {}
};