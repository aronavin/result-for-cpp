add_executable(test_result source/test.cpp)

include_directories("../../include")

target_link_libraries(test_result PRIVATE result)
add_test(NAME result_tests COMMAND test_result)