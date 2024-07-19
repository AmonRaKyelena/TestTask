CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(ClienServerEcn)

FIND_PACKAGE(Boost 1.40 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

SET(CMAKE_CXX_STANDARD 17)
SET(SERVER_SOURCES
    Server.cpp
    Core.cpp
    Session.cpp
	ServerMain.cpp
    Common.hpp
    json.hpp
    Core.hpp
    Server.hpp
    Session.hpp
    Order.hpp
)
SET(CLIENT_SOURCES
	Client.cpp
	Common.hpp
	json.hpp
	Core.hpp
	Server.hpp
	Session.hpp
	Order.hpp
)

ADD_EXECUTABLE(Server ${SERVER_SOURCES})
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(Client ${CLIENT_SOURCES})
TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads ${Boost_LIBRARIES})