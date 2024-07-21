#include "../../include.hpp"


Session::Session(boost::asio::io_service& io_service)
		: socket_(io_service) {}

tcp::socket& Session::GetSocket() 
{
	return socket_;
}

void Session::start() 
{
	socket_.async_read_some(boost::asio::buffer(data_, max_length),
		boost::bind(&Session::handle_read, this,
			boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
}

void Session::handle_write(const boost::system::error_code& error) 
{
	if (!error) 
	{
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			boost::bind(&Session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	} 
	else 
	{
		delete this;
	}
}

void Session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) 
{
	if (!error) 
	{
		data_[bytes_transferred] = '\0';

		try 
		{
			auto j = nlohmann::json::parse(data_);
			auto reqType = j["ReqType"];
			std::string reply = "Error! Unknown request type\n";

			if (reqType == "Registration") 
				reply = GetCore().RegisterNewUser(j["Message"]);
			else if (reqType == "Hello") 
				reply = "Hello, " + GetCore().GetUserName(j["UserId"]) + "!\n";
			else if (reqType == "SubmitOrder") 
				reply = GetCore().SubmitOrder(j["UserId"], j["Message"]["OrderType"], j["Message"]["Volume"], j["Message"]["Price"]);
			else if(reqType == "ShowActiveOrders")
				reply = GetCore().PrintOrders();
			else if (reqType == "GetBalance") 
				reply = GetCore().GetBalance(j["UserId"]);

			boost::asio::async_write(socket_, boost::asio::buffer(reply, reply.size()),
				boost::bind(&Session::handle_write, this, boost::asio::placeholders::error));
		} 
		catch (nlohmann::json::exception& e) 
		{
			std::cerr << "JSON parse error: \n" << e.what() << std::endl;
			std::string reply = "Error! JSON parse error\n";
			boost::asio::async_write(socket_, boost::asio::buffer(reply, reply.size()),
				boost::bind(&Session::handle_write, this, boost::asio::placeholders::error));
		}
	} 
	else 
	{
		delete this;
	}
}