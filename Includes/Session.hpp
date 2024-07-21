#pragma once

#include "../include.hpp"

using boost::asio::ip::tcp;

class Session 
{
private:
	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];

public:
	Session(boost::asio::io_service& io_service);

	tcp::socket& GetSocket();

	void start();
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error);
};