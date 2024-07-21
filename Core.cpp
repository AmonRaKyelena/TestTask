#include "include.hpp"

std::string Core::RegisterNewUser(const std::string& userName) 
{
	size_t newUserId = mUsers.size();
	mUsers[newUserId] = userName;
	mBalances[newUserId] = {0, 0};

	return std::to_string(newUserId);
}

void Core::ClearOrders()
{
	mActiveOrdersSell.clear();
	mActiveOrdersBuy.clear();
}

std::string Core::GetUserName(const std::string& userId) 
{
	const auto userIt = mUsers.find(std::stoi(userId));
	if (userIt == mUsers.end()) 
	{
		return "Error! Unknown User\n";
	} 
	else
	{
		return userIt->second;
	}
}

std::string Core::SubmitOrder(const std::string& userId, const std::string& type, int volume, int price) 
{
	if (userId.empty() || type.empty() || volume <= 0 || price <= 0) 
	{
		return "Error! Invalid input parameters.\n";
	}

	int client_id;
	try 
	{
		client_id = std::stoi(userId);
	} 
	catch (const std::invalid_argument&) 
	{
		return "Error! Invalid user ID.\n";
	}

	if (mUsers.find(client_id) == mUsers.end()) 
	{
		return "Error! Unknown User.\n";
	}

	OrderType orderType;

	if (type == "BUY") 
	{
		orderType = OrderType::BUY;
	} 
	else if (type == "SELL") 
	{
		orderType = OrderType::SELL;
	} 
	else 
	{
		return "Error! Invalid order type.\n";
	}

	int orderId = mNextOrderId++;
	Order order(orderId, orderType, volume, price, client_id);

	std::string result = MatchOrder(order);
	return result;
}

std::string Core::GetBalance(const std::string& userId) 
{
	int client_id;
	try 
	{
		client_id = std::stoi(userId);
	} 
	catch (const std::invalid_argument&) 
	{
		return "Error! Invalid user ID.\n";
	}

	const auto balanceIt = mBalances.find(client_id);
	if (balanceIt == mBalances.end()) 
	{
		return "Error! Unknown User.\n";
	} 
	else 
	{
		auto balance = balanceIt->second;
		return "USD: " + std::to_string(balance.first) + ", RUB: " + std::to_string(balance.second) + "\n";
	}
}

void Core::PrintOrders() {
    std::cout << "Active BUY orders:" << std::endl;
    for (const auto& [price, order] : mActiveOrdersBuy) {
        auto orderTimeNs = order.timeOrder.time_since_epoch().count();
        std::cout << "Order ID: " << order.id << ", Price: " << price << ", Volume: " << order.volume << ", Client ID: " << order.client_id << ", Time: " << orderTimeNs << " ns" << std::endl;
    }
    std::cout << "Active SELL orders:" << std::endl;
    for (const auto& [price, order] : mActiveOrdersSell) {
        auto orderTimeNs = order.timeOrder.time_since_epoch().count();
        std::cout << "Order ID: " << order.id << ", Price: " << price << ", Volume: " << order.volume << ", Client ID: " << order.client_id << ", Time: " << orderTimeNs << " ns" << std::endl;
    }
}

std::string Core::MatchOrder(Order& order) 
{
	std::string result;

	if(order.type == OrderType::BUY)
	{
		for(auto it = mActiveOrdersSell.begin(); it != mActiveOrdersSell.end();)
		{
			if(order.price >= it->first)
			{
				result += ExecuteTrade(order, it);
				if(it->second.volume == 0)
					it = mActiveOrdersSell.erase(it);
				if (order.volume == 0) break;
			} 
			else 
                ++it;
		}
	}
	else if(order.type == OrderType::SELL)
	{
		for(auto it = mActiveOrdersBuy.rbegin(); it != mActiveOrdersBuy.rend();)
		{
			if(order.price <= it->first)
			{
				auto base = it.base();
				result += ExecuteTrade(order, --base);
				if (base->second.volume == 0) {
					it = std::make_reverse_iterator(mActiveOrdersBuy.erase(base));
				}
				if (order.volume == 0) break;
			} 
			else 
				++it;
		}
	}
	if (result.empty()) 
	{
        result = "No matching order found\n";
    } 
	if (order.volume > 0) 
	{
        if (order.type == OrderType::BUY) 
            mActiveOrdersBuy.emplace(order.price, order);
		else if (order.type == OrderType::SELL) 
            mActiveOrdersSell.emplace(order.price, order);
    }
	return result;
}

std::string Core::ExecuteTrade(Order& order, std::multimap<int, Order>::iterator it)
{
	int tradeVolume = std::min(order.volume, it->second.volume);
	UpdateBalances(order.client_id, it->second.client_id, tradeVolume, it->first, order.type);
	order.volume -= tradeVolume;
	it->second.volume -= tradeVolume;
	return "Trade executed: " + std::to_string(tradeVolume) + " USD at " + std::to_string(it->first) + " RUB\n";
}


void Core::UpdateBalances(int buyerId, int sellerId, int volume, int price, OrderType orderType) {
	int rubAmount = volume * price;
	if (orderType == OrderType::BUY) {
		mBalances[buyerId].first += volume;  
		mBalances[buyerId].second -= rubAmount;  
		mBalances[sellerId].first -= volume;  
		mBalances[sellerId].second += rubAmount;
	} else if (orderType == OrderType::SELL) {
		mBalances[buyerId].first -= volume;  
		mBalances[buyerId].second += rubAmount;  
		mBalances[sellerId].first += volume;  
		mBalances[sellerId].second -= rubAmount;  
	}
}

Core& GetCore() {
    static Core core;
    return core;
}
