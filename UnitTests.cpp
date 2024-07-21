#define BOOST_TEST_MODULE ExampleTest
#include <boost/test/included/unit_test.hpp>
#include "include.hpp"

BOOST_AUTO_TEST_SUITE(CoreTests)

BOOST_AUTO_TEST_CASE(TestMatchOrder1) {
    Core& core = GetCore();
    std::string EveBuyerId = core.RegisterNewUser("Eve");
    std::string FrankBuyerId = core.RegisterNewUser("Frank");
	std::string IvanBuyerId  = core.RegisterNewUser("Ivan");
	std::string AlekseiBuyerId  = core.RegisterNewUser("Aleksei");
	std::string AlenaSellerId = core.RegisterNewUser("Alena");
	
    core.SubmitOrder(EveBuyerId, "BUY", 100, 28);
    core.SubmitOrder(FrankBuyerId, "BUY", 30, 30);
    core.SubmitOrder(IvanBuyerId, "BUY", 22, 25);
    core.SubmitOrder(AlekseiBuyerId, "BUY", 9, 40);
    core.SubmitOrder(AlenaSellerId, "SELL", 40, 5);
	core.SubmitOrder(AlenaSellerId, "SELL", 59, 26);

    BOOST_CHECK_EQUAL(core.GetBalance(EveBuyerId), "USD: 60, RUB: -1680\n");
    BOOST_CHECK_EQUAL(core.GetBalance(FrankBuyerId), "USD: 30, RUB: -900\n");
	BOOST_CHECK_EQUAL(core.GetBalance(IvanBuyerId), "USD: 0, RUB: 0\n");
    BOOST_CHECK_EQUAL(core.GetBalance(AlekseiBuyerId), "USD: 9, RUB: -360\n");
    BOOST_CHECK_EQUAL(core.GetBalance(AlenaSellerId), "USD: -99, RUB: 2940\n");
    
    core.ClearOrders();
}

BOOST_AUTO_TEST_CASE(TestMatchOrder2) {
    Core& core = GetCore();
    std::string EveBuyerId = core.RegisterNewUser("Eve");
    std::string FrankBuyerId = core.RegisterNewUser("Frank");
	std::string IvanBuyerId  = core.RegisterNewUser("Ivan");
	std::string AlekseiBuyerId  = core.RegisterNewUser("Aleksei");
	std::string AlenaSellerId = core.RegisterNewUser("Alena");

    core.SubmitOrder(EveBuyerId, "SELL", 100, 30);
    core.SubmitOrder(FrankBuyerId, "SELL", 30, 30);
    core.SubmitOrder(IvanBuyerId, "SELL", 22, 25);
	core.SubmitOrder(EveBuyerId, "SELL", 10, 15);
    core.SubmitOrder(AlekseiBuyerId, "SELL", 9, 40);
    core.SubmitOrder(AlenaSellerId, "BUY", 40, 50);

    BOOST_CHECK_EQUAL(core.GetBalance(EveBuyerId), "USD: -18, RUB: 390\n");
    BOOST_CHECK_EQUAL(core.GetBalance(FrankBuyerId), "USD: 0, RUB: 0\n");
	BOOST_CHECK_EQUAL(core.GetBalance(IvanBuyerId), "USD: -22, RUB: 550\n");
    BOOST_CHECK_EQUAL(core.GetBalance(AlekseiBuyerId), "USD: 0, RUB: 0\n");
    BOOST_CHECK_EQUAL(core.GetBalance(AlenaSellerId), "USD: 40, RUB: -940\n");

    core.ClearOrders();
}

BOOST_AUTO_TEST_SUITE_END()


