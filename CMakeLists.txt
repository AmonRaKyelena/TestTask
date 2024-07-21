CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(ClienServerEcn)

FIND_PACKAGE(Boost 1.40 COMPONENTS system unit_test_framework REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/Includes)
include_directories(${PROJECT_SOURCE_DIR}src/Client)
include_directories(${PROJECT_SOURCE_DIR}src/Server)
include_directories(${PROJECT_SOURCE_DIR}src/Tests)

SET(CMAKE_CXX_STANDARD 17)
SET(SERVER_SOURCES
	src/Server/Server.cpp
    src/Server/Core.cpp
    src/Server/Session.cpp
	src/Server/ServerMain.cpp
    Includes/Common.hpp
    Includes/json.hpp
    Includes/Core.hpp
    Includes/Server.hpp
    Includes/Session.hpp
    Includes/Order.hpp
	Includes/Common.hpp
)
SET(CLIENT_SOURCES
	src/Client/Client.cpp
	Includes/Common.hpp
	Includes/json.hpp
	Includes/Core.hpp
	Includes/Server.hpp
	Includes/Session.hpp
	Includes/Order.hpp
)
SET(TESTS_SOURCES
	src/Tests/UnitTests.cpp
	src/Server/Core.cpp
	Includes/Common.hpp
	Includes/json.hpp
	Includes/Core.hpp
	Includes/Server.hpp
	Includes/Session.hpp
	Includes/Order.hpp
)

ADD_EXECUTABLE(Server ${SERVER_SOURCES})
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(Client ${CLIENT_SOURCES})
TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(Tests ${TESTS_SOURCES})
TARGET_LINK_LIBRARIES(Tests PRIVATE Threads::Threads ${Boost_LIBRARIES})