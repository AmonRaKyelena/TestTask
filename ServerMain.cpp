#include "include.hpp"

int main() 
{
    try 
    {
        boost::asio::io_service io_service;
        Server s(io_service, 12345);
        io_service.run();
    } 
    catch (std::exception& e) 
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
