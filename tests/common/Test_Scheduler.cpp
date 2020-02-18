#include <catch.hpp>

#include <mw/core/common/Scheduler.h>

// Not the most elegant test, but it does the trick.
TEST_CASE("Scheduler")
{
    auto pScheduler = Scheduler::Create(3);
    
    auto increment = [](int& timesRun) { timesRun++; };

    int firstTask = 0;
    pScheduler->RunEvery(std::bind(increment, std::ref(firstTask)), std::chrono::milliseconds(50));

    int secondTask = 0;
    pScheduler->RunEvery(std::bind(increment, std::ref(secondTask)), std::chrono::milliseconds(150));

    int thirdTask = 0;
    pScheduler->RunEvery(std::bind(increment, std::ref(thirdTask)), std::chrono::milliseconds(500));

    int fourthTask = 0;
    pScheduler->RunOnce(std::bind(increment, std::ref(fourthTask)));

    std::this_thread::sleep_for(std::chrono::milliseconds(225));
    pScheduler.reset();

    REQUIRE(firstTask == 4);
    REQUIRE(secondTask == 1);
    REQUIRE(thirdTask == 0);
    REQUIRE(fourthTask == 1);
}