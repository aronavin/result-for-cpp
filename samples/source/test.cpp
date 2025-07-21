#include <string>
#include <iostream>

#include "result.h"

namespace io {

class error {

public:

    error(const std::string& message, const int code) : message_(message), code_(code) {}

    const std::string& message() const { return message_; }

    int code() const { return code_; }

    friend std::ostream& operator<<(std::ostream& os, const error& err) {

        return os  << err.message() << " (" << err.code() << ")";
    }

private:

    std::string message_;

    int code_;
};

template<typename T = void, typename E = error>
using result = result::result<T, E>;

auto fail(const std::string& message, int code) {

    return ::result::make_error(error(message, code));
}

}

io::result<int> test(int value) {

    if (value < 0) {

        return io::fail("Negative value not allowed", -1);
    }

    return value;
}


int main() {

    auto result = test(0);

    if (!result) {

       std::cout << result.error() << std::endl;
       
       return -1;
    }

    std::cout << "success" << std::endl;

    return 0;
}