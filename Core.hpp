#pragma once

#include "include.hpp"

enum class OrderType;

class Order;

class Core {
private:
    std::map<size_t, std::string> mUsers;
    std::map<int, std::pair<int, int>> mBalances; 
    std::deque<Order> mActiveOrders;
    int mNextOrderId = 0;

	std::string MatchOrder(Order& order);
    
	void UpdateBalances(int buyerId, int sellerId, int volume, double price, OrderType orderType);
	
public:
	std::string RegisterNewUser(const std::string& userName);
	std::string GetUserName(const std::string& userId);
	std::string SubmitOrder(const std::string& userId, const std::string& type, int volume, double price);
	std::string GetBalance(const std::string& userId);

	void ClearOrders();

	
};

Core& GetCore();

