#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "json.hpp"
#include <map>
#include <deque>
#include <vector>

using boost::asio::ip::tcp;

enum class OrderType { BUY, SELL };

struct Order {
    int id;
    OrderType type;
    int volume;
    double price;
    int client_id;

    Order(int id, OrderType type, int volume, double price, int client_id)
        : id(id), type(type), volume(volume), price(price), client_id(client_id) {}
};

class Core {
public:
    std::string RegisterNewUser(const std::string& userName) {
        size_t newUserId = mUsers.size();
        mUsers[newUserId] = userName;
        mBalances[newUserId] = {0, 0};

        return std::to_string(newUserId);
    }

    std::string GetUserName(const std::string& userId) {
        const auto userIt = mUsers.find(std::stoi(userId));
        if (userIt == mUsers.end()) {
            return "Error! Unknown User\n";
        } else {
            return userIt->second;
        }
    }

    std::string SubmitOrder(const std::string& userId, const std::string& type, int volume, double price) {
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

    std::string GetBalance(const std::string& userId) {
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

    void ClearOrders() {
        mActiveOrders.clear();
    }

private:
    std::map<size_t, std::string> mUsers;
    std::map<int, std::pair<int, int>> mBalances; // <UserId, <USD, RUB>>
    std::deque<Order> mActiveOrders;
    int mNextOrderId = 0;

    std::string MatchOrder(Order& order) {
        std::string result;
        auto it = mActiveOrders.begin();
        while (it != mActiveOrders.end()) {
            if ((order.type == OrderType::BUY && it->type == OrderType::SELL && order.price >= it->price) ||
                (order.type == OrderType::SELL && it->type == OrderType::BUY && order.price <= it->price)) {

                int tradeVolume = std::min(order.volume, it->volume);
                double tradePrice = (order.id < it->id) ? order.price : it->price;

                UpdateBalances(order.client_id, it->client_id, tradeVolume, tradePrice, order.type);

                order.volume -= tradeVolume;
                it->volume -= tradeVolume;

                result += "Trade executed: " + std::to_string(tradeVolume) + " USD at " + std::to_string(std::round(tradePrice)) + "\n";

                if (it->volume == 0) {
                    it = mActiveOrders.erase(it);
                } else {
                    ++it;
                }

                if (order.volume == 0) {
                    break;
                }
            } else {
                ++it;
            }
        }
        return result.empty() ? "No matching order found\n" : result;
    }

    void UpdateBalances(int buyerId, int sellerId, int volume, double price, OrderType orderType) {
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
};

Core& GetCore() {
    static Core core;
    return core;
}

class Session {
public:
    Session(boost::asio::io_service& io_service)
        : socket_(io_service) {}

    tcp::socket& socket() {
        return socket_;
    }

    void start() {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&Session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        data_[bytes_transferred] = '\0';

        try {
            auto j = nlohmann::json::parse(data_);
            auto reqType = j["ReqType"];
            std::string reply = "Error! Unknown request type\n";

            if (reqType == "Registration") {
                reply = GetCore().RegisterNewUser(j["Message"]);
            } else if (reqType == "Hello") {
                reply = "Hello, " + GetCore().GetUserName(j["UserId"]) + "!\n";
            } else if (reqType == "SubmitOrder") {
                reply = GetCore().SubmitOrder(j["UserId"], j["Message"]["OrderType"], j["Message"]["Volume"], j["Message"]["Price"]);
            } else if (reqType == "GetBalance") {
                reply = GetCore().GetBalance(j["UserId"]);
            }

            boost::asio::async_write(socket_, boost::asio::buffer(reply, reply.size()),
                boost::bind(&Session::handle_write, this, boost::asio::placeholders::error));
        } catch (nlohmann::json::exception& e) {
            std::cerr << "JSON parse error: \n" << e.what() << std::endl;
            std::string reply = "Error! JSON parse error\n";
            boost::asio::async_write(socket_, boost::asio::buffer(reply, reply.size()),
                boost::bind(&Session::handle_write, this, boost::asio::placeholders::error));
        }
    } else {
        delete this;
    }
}


    void handle_write(const boost::system::error_code& error) {
        if (!error) {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&Session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        } else {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class Server {
public:
    Server(boost::asio::io_service& io_service, short port)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
        std::cout << "Server started! Listening on port " << port << std::endl;
        start_accept();
    }

    void start_accept() {
        Session* new_session = new Session(io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&Server::handle_accept, this, new_session,
                boost::asio::placeholders::error));
    }

    void handle_accept(Session* new_session, const boost::system::error_code& error) {
        if (!error) {
            new_session->start();
            start_accept();
        } else {
            delete new_session;
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};

int main() {
    try {
        boost::asio::io_service io_service;
        Server s(io_service, 12345);
        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
