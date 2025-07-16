# result-for-cpp

A minimal, C++ `result<T, E>` type for error handling without exceptions. Inspired by Rust’s `Result`.


## Helpers

    - `make_ok()` and `make_error()`.

## Example

```cpp
#include "result.h"
#include <string>
#include <iostream>

result::result<std::string, std::string> open_file(bool okay) {
    if (okay)
        return result::make_ok(std::string("file.txt"));
    else
        return result::make_error(std::string("failed to open file"));
}

int main() {
    auto res = open_file(true);
    if (res) {
        std::cout << "Opened: " << res.value() << "\n";
    } else {
        std::cout << "Error: " << res.error() << "\n";
    }
}
```