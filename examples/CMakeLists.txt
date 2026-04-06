add_executable(choice-demo main.cpp)
add_executable(any-bitgen-demo any_bitgen_demo.cpp)
target_link_libraries(choice-demo PRIVATE rng::rng)
target_link_libraries(any-bitgen-demo PRIVATE rng::rng)
