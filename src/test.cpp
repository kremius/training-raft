#include "test.h"

#include <experimental/coroutine>

#include <iostream>

void foo() {
    std::cout << "Hello" << std::endl;
    co_await std::experimental::suspend_always();
    std::cout << "World" << std::endl;
}

void test() {
    // Do nothing
}
