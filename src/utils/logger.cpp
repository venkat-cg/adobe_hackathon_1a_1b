#include "logger.h"
#include <iostream>

void log(const std::string& msg) {
    std::cout << "[LOG]: " << msg << std::endl;
}
