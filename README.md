# result-for-cpp

A minimal, header-only result<T, E> implementation for C++11 — inspired by Rust’s Result. Not a full-featured clone — it's meant for embedded systems and places where exceptions are not an option.


## Example

```cpp

#include "result.h"

#include <iostream>
#include <string>

using result::result;
using result::make_error;

struct error {

    std::string message;

    int code;

    friend std::ostream& operator<<(std::ostream& os, const error& e) {
        
        return os << e.message << " (" << e.code << ")";
    }
};

result<std::string, error> open_file(bool ok) {

    if (ok) {

        return "file.txt";
    }

    return make_error(error{"failed to open file", -1});
}

int main() {

    auto result = open_file(true);

    if (result) {

        std::cout << "Opened: " << result.value() << "\n";
    } 
    else {

        std::cout << "Error: " << result.error() << "\n";
    }
}
```
