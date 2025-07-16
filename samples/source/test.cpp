#include <string>
#include <iostream>

#include "result.h"


result::result<result::none, std::string> get_result(int status) {

    if (!status) {

        return result::make_error(std::string("failure result"));
    }

    return result::make_ok();
}

int main() {

    auto result = get_result(0);

    if (!result) {

       std::cout << result.error() << std::endl;
       
       return -1;
    }

    std::cout << "success" << std::endl;

    return 0;
}