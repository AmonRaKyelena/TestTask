#pragma once

#include "include.hpp"

using boost::asio::ip::tcp;

class Session;

class Server {
private:
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;

public:
	Server(boost::asio::io_service& io_service, short port);

	void start_accept();
	void handle_accept(Session* new_session, const boost::system::error_code& error);

};
