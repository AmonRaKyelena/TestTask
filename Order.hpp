#pragma once

#include "include.hpp"

enum class OrderType { BUY, SELL };

struct Order 
{
    int id;
    OrderType type;
    int volume;
    int price;
    int client_id;
    std::chrono::time_point<std::chrono::system_clock> timeOrder;

    Order(int id, OrderType type, int volume, int price, int client_id)
        : id(id), type(type), volume(volume), price(price), client_id(client_id), 
        timeOrder(std::chrono::system_clock::now()) {}
};