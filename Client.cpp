#include "include.hpp"

using boost::asio::ip::tcp;

const short clientPort = 12345;
//посмотреть в Common.hpp
namespace Requests 
{
    const std::string RegistrationClient = "Registration";
    const std::string HelloClient= "Hello";
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

void checkInputEOFAndExit() 
{
    //переделать на try catch
    if (std::cin.eof()) 
    {
        std::cout << "Input interrupted.\n";
        std::exit(0);
    }
}

std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    SendMessage(aSocket, "0", Requests::RegistrationClient, name);
    return ReadMessage(aSocket);
}

bool IsValidInteger(const std::string& str) 
{
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

int getValidatedInput(std::string value)
{
    std::string strValumeOrPrice;
    while (true) 
    {
        std::cout << "Enter " << value << ": ";
        std::cin >> strValumeOrPrice;

        checkInputEOFAndExit();
        
        if (IsValidInteger(strValumeOrPrice)) 
            return std::stoi(strValumeOrPrice);

        std::cout << "Invalid " << value << ". Please enter a valid number.\n";
    }
    return 0;
}

void SubmitOrder(tcp::socket& s, const std::string& userId)
{
    std::string orderType;
    int volume;
    int price;

    while (true) 
    {
        std::cout << "Enter order type (BUY/SELL): ";
        std::cin >> orderType;

        checkInputEOFAndExit();

        if (orderType == "BUY" || orderType == "SELL") 
            break;

        std::cout << "Invalid order type. Please enter 'BUY' or 'SELL'.\n";
    }

    volume = getValidatedInput("volume");
    price = getValidatedInput("price");
    
    nlohmann::json order;
    order["OrderType"] = orderType;
    order["Volume"] = volume;
    order["Price"] = price;

    SendMessage(s, userId, Requests::SubmitOrder, order);
    std::cout << ReadMessage(s);
}

void GetBalance(tcp::socket& s, const std::string& user_id)
{
    SendMessage(s, user_id, Requests::GetBalance, "");
    std::cout << ReadMessage(s);
}

int displayAndHandleMenu(const std::string& myId, tcp::socket& s)
{
    while (true)
    {
        std::cout << "Menu:\n"
                    "1) Hello Request\n"
                    "2) Submit Order\n"
                    "3) Get Balance\n"
                    "4) Exit"
                    << std::endl;

        std::string menuOptionNumSt;
        std::cin >> menuOptionNumSt;

        checkInputEOFAndExit();

        if (!IsValidInteger(menuOptionNumSt))
        {
            std::cout << "Invalid menu option. Please enter a valid number.\n" << std::endl;
            continue;
        }
            

        short menuOptionNum = std::stoi(menuOptionNumSt);

        switch (menuOptionNum)
        {
            case 1:
            {
                SendMessage(s, myId, Requests::HelloClient, "");
                std::cout << ReadMessage(s) << std::endl;
                break;
            }
            case 2:
            {
                SubmitOrder(s, myId);
                break;
            }
            case 3:
            {
                GetBalance(s, myId);
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
    return 0;
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(clientPort));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        std::string myId = ProcessRegistration(s);
        std::cout << "Registered with ID: " << myId << std::endl;
        if(displayAndHandleMenu(myId, s) == 0)
            return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}