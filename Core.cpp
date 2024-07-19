#include "include.hpp"

std::string Core::RegisterNewUser(const std::string& userName) {
	size_t newUserId = mUsers.size();
	mUsers[newUserId] = userName;
	mBalances[newUserId] = {0, 0};

	return std::to_string(newUserId);
}

std::string Core::GetUserName(const std::string& userId) {
	const auto userIt = mUsers.find(std::stoi(userId));
	if (userIt == mUsers.end()) {
		return "Error! Unknown User\n";
	} else {
		return userIt->second;
	}
}
//переделать
std::string Core::SubmitOrder(const std::string& userId, const std::string& type, int volume, double price) {
	if (userId.empty() || type.empty() || volume <= 0 || price <= 0.0) {
		return "Error! Invalid input parameters.\n";
	}

	int client_id;
	try {
		client_id = std::stoi(userId);
	} catch (const std::invalid_argument&) {
		return "Error! Invalid user ID.\n";
	}

	if (mUsers.find(client_id) == mUsers.end()) {
		return "Error! Unknown User.\n";
	}

	OrderType orderType;
	if (type == "BUY") {
		orderType = OrderType::BUY;
	} else if (type == "SELL") {
		orderType = OrderType::SELL;
	} else {
		return "Error! Invalid order type.\n";
	}

	int orderId = mNextOrderId++;
	Order order(orderId, orderType, volume, price, client_id);

	std::string result = MatchOrder(order);

	if (order.volume > 0) {
		mActiveOrders.push_back(order);
	}

	return result;
}

std::string Core::GetBalance(const std::string& userId) {
	int client_id;
	try {
		client_id = std::stoi(userId);
	} catch (const std::invalid_argument&) {
		return "Error! Invalid user ID.\n";
	}

	const auto balanceIt = mBalances.find(client_id);
	if (balanceIt == mBalances.end()) {
		return "Error! Unknown User.\n";
	} else {
		auto balance = balanceIt->second;
		return "USD: " + std::to_string(balance.first) + ", RUB: " + std::to_string(balance.second) + "\n";
	}
}

void Core::ClearOrders() {
	mActiveOrders.clear();
}

//переделать
std::string Core::MatchOrder(Order& order) {
	std::string result;
	auto it = mActiveOrders.begin();
	while (it != mActiveOrders.end()) {
		if()
	}
	return result.empty() ? "No matching order found\n" : result;
}

//передалть static_cast
void Core::UpdateBalances(int buyerId, int sellerId, int volume, double price, OrderType orderType) {
	int rubAmount = static_cast<int>(volume * price);
	if (orderType == OrderType::BUY) {
		mBalances[buyerId].first += volume;  
		mBalances[buyerId].second -= rubAmount;  
		mBalances[sellerId].first -= volume;  
		mBalances[sellerId].second += rubAmount;  
	} else {
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

// if ((order.type == OrderType::BUY && it->type == OrderType::SELL && order.price >= it->price) ||
		// 	(order.type == OrderType::SELL && it->type == OrderType::BUY && order.price <= it->price)) {

		// 	int tradeVolume = std::min(order.volume, it->volume);
		// 	double tradePrice = (order.id < it->id) ? order.price : it->price;

		// 	UpdateBalances(order.client_id, it->client_id, tradeVolume, tradePrice, order.type);

		// 	order.volume -= tradeVolume;
		// 	it->volume -= tradeVolume;

		// 	result += "Trade executed: " + std::to_string(tradeVolume) + " USD at " + std::to_string(tradePrice) + "\n";

		// 	if (it->volume == 0) {
		// 		it = mActiveOrders.erase(it);
		// 	} else {
		// 		++it;
		// 	}

		// 	if (order.volume == 0) {
		// 		break;
		// 	}
		// } else {
		// 	++it;
		// }