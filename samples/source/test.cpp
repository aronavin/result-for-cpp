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

}


#define io_failure(message, code) (result::make_error(io::error(message, code)))

io::result<int> get_value(int value) {

    if (value < 0) {

        return io_failure("Negative value not allowed", -1);
    }

    return value * (rand() % 100);
}

io::result<std::string> convert_to_string(int value) {

    return std::to_string(value);
}


int main() {

    auto result = get_value(5).and_then([] (int res) {

        return convert_to_string(res);
    });

    if (!result) {

       std::cout << result.error() << std::endl;
       
       return -1;
    }

    std::cout << "result: " << result.value() << std::endl;

    return 0;
}