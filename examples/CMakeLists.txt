set(EXAMPLE_FLAGS -Wall -Wextra -pedantic -Werror)

find_package(Boost 1.74 REQUIRED COMPONENTS context system coroutine date_time atomic)

add_executable(async_pool "async/pool.cc")
target_link_libraries(async_pool elsid::resource_pool)
target_compile_options(async_pool PRIVATE ${EXAMPLE_FLAGS})

add_executable(coro_pool "async/coro.cc")
target_link_libraries(coro_pool elsid::resource_pool Boost::coroutine Boost::context)
target_compile_options(coro_pool PRIVATE ${EXAMPLE_FLAGS})

add_executable(async_strand "async/strand.cc")
target_link_libraries(async_strand elsid::resource_pool)
target_compile_options(async_strand PRIVATE ${EXAMPLE_FLAGS})

add_executable(async_regression "async/regression_smoke.cc")
target_link_libraries(async_regression elsid::resource_pool Boost::coroutine Boost::context)
target_compile_options(async_regression PRIVATE ${EXAMPLE_FLAGS})

add_executable(sync_pool "sync/pool.cc")
target_link_libraries(sync_pool elsid::resource_pool)
target_compile_options(sync_pool PRIVATE ${EXAMPLE_FLAGS})
