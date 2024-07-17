#include <iostream>
#include <boost/asio.hpp>
#include "json.hpp"

using boost::asio::ip::tcp;

const short port = 12345;

namespace Requests {
    const std::string Registration = "Registration";
    const std::string Hello = "Hello";
    const std::string SubmitOrder = "SubmitOrder";
    const std::string GetBalance = "GetBalance";
}

void SendMessage(tcp::socket& aSocket, const std::string& aId, const std::string& aRequestType, const nlohmann::json& aMessage)
{
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

std::string ReadMessage(tcp::socket& aSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}

std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    SendMessage(aSocket, "0", Requests::Registration, name);
    return ReadMessage(aSocket);
}

bool IsValidInteger(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

bool IsValidDouble(const std::string& str) {
    std::istringstream iss(str);
    double d;
    return iss >> d && iss.eof();
}

void SubmitOrder(tcp::socket& s, const std::string& user_id)
{
    std::string order_type;
    std::string volume_str;
    std::string price_str;
    int volume;
    double price;

    while (true) {
        std::cout << "Enter order type (BUY/SELL): ";
        std::cin >> order_type;

        if (std::cin.eof()) {
            std::cout << "Input interrupted.\n";
            std::exit(0);
        }

        if (order_type == "BUY" || order_type == "SELL") {
            break;
        }
        std::cout << "Invalid order type. Please enter 'BUY' or 'SELL'.\n";
    }

    while (true) {
        std::cout << "Enter volume: ";
        std::cin >> volume_str;

        if (std::cin.eof()) {
            std::cout << "Input interrupted.\n";
            std::exit(0);
        }

        if (IsValidInteger(volume_str)) {
            volume = std::stoi(volume_str);
            break;
        }
        std::cout << "Invalid volume. Please enter a valid integer.\n";
    }

    while (true) {
        std::cout << "Enter price: ";
        std::cin >> price_str;

        if (std::cin.eof()) {
            std::cout << "Input interrupted.\n";
            std::exit(0);
        }
        
        if (IsValidDouble(price_str)) {
            price = std::stod(price_str);
            break;
        }
        std::cout << "Invalid price. Please enter a valid number.\n";
    }

    nlohmann::json order;
    order["OrderType"] = order_type;
    order["Volume"] = volume;
    order["Price"] = price;

    SendMessage(s, user_id, Requests::SubmitOrder, order);
    std::cout << ReadMessage(s);
}

void GetBalance(tcp::socket& s, const std::string& user_id)
{
    SendMessage(s, user_id, Requests::GetBalance, "");
    std::cout << ReadMessage(s);
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        // Регистрируем пользователя - отправляем на сервер имя, сервер возвращает ID.
        std::string my_id = ProcessRegistration(s);
        std::cout << "Registered with ID: " << my_id << std::endl;

        while (true)
        {
            std::cout << "Menu:\n"
                         "1) Hello Request\n"
                         "2) Submit Order\n"
                         "3) Get Balance\n"
                         "4) Exit\n"
                         << std::endl;

            short menu_option_num;
            std::cin >> menu_option_num;
            switch (menu_option_num)
            {
                case 1:
                {
                    SendMessage(s, my_id, Requests::Hello, "");
                    std::cout << ReadMessage(s) << std::endl;
                    break;
                }
                case 2:
                {
                    SubmitOrder(s, my_id);
                    break;
                }
                case 3:
                {
                    GetBalance(s, my_id);
                    break;
                }
                case 4:
                {
                    return 0;
                }
                default:
                {
                    std::cout << "Unknown menu option\n" << std::endl;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
