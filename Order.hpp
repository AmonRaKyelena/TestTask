#pragma once

#include "include.hpp"

enum class OrderType { BUY, SELL };

struct Order {
    int id;
    OrderType type;
    int volume;
    double price;
    int client_id;
    std::chrono::system_clock::time_point timeOrder;

    Order(int id, OrderType type, int volume, double price, int client_id)
        : id(id), type(type), volume(volume), price(price), client_id(client_id), timeOrder(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())) {}
};