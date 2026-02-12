#include <yamail/resource_pool.hpp>
#include <yamail/resource_pool/handle.hpp>
#include <yamail/resource_pool/time_traits.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/use_future.hpp>

#include <chrono>
#include <future>
#include <functional>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

namespace asio = boost::asio;
namespace rp = yamail::resource_pool;

using sync_pool_t = rp::sync::pool<int>;
using async_pool_t = rp::async::pool<std::string>;
using handle_t = async_pool_t::handle;
using callback_token_t = std::function<void (boost::system::error_code, handle_t)>;

using callback_return_t = decltype(std::declval<async_pool_t&>().get_auto_waste(
    std::declval<asio::io_context&>(),
    std::declval<callback_token_t>(),
    rp::time_traits::duration(0)));

using future_return_t = decltype(std::declval<async_pool_t&>().get_auto_waste(
    std::declval<asio::io_context&>(),
    asio::use_future,
    rp::time_traits::duration(0)));

using yield_return_t = decltype(std::declval<async_pool_t&>().get_auto_waste(
    std::declval<asio::io_context&>(),
    std::declval<asio::yield_context>(),
    rp::time_traits::duration(0)));

static_assert(std::is_move_constructible_v<sync_pool_t>, "sync pool must stay move constructible");
static_assert(std::is_move_constructible_v<async_pool_t>, "async pool must stay move constructible");
static_assert(std::is_same_v<callback_return_t, void>, "callback token API must return void");
static_assert(std::is_same_v<future_return_t, std::future<handle_t>>, "use_future API contract changed");
static_assert(std::is_same_v<yield_return_t, handle_t>, "yield_context API contract changed");

template class rp::sync::pool<int>;
template class rp::async::pool<std::string>;

int main() {
    asio::io_context io;
    async_pool_t pool(1, 4);
    bool first_done = false;
    bool second_done = false;
    std::string reused_value;

    pool.get_auto_waste(io, [&] (const boost::system::error_code& ec, handle_t first) {
        if (ec) {
            std::cerr << "first acquire failed: " << ec.message() << std::endl;
            return;
        }
        first_done = true;
        if (first.empty()) {
            first.reset(std::string("resource-from-first-acquire"));
        }
        pool.get_auto_waste(io, [&] (const boost::system::error_code& second_ec, handle_t second) {
            if (second_ec) {
                std::cerr << "second acquire failed: " << second_ec.message() << std::endl;
                return;
            }
            second_done = true;
            if (!second.empty()) {
                reused_value = second.get();
            }
            second.recycle();
        }, std::chrono::seconds(1));
        first.recycle();
    }, std::chrono::seconds(1));

    io.run();

    if (!first_done || !second_done) {
        std::cerr << "async acquire/release regression check did not complete" << std::endl;
        return 1;
    }
    if (reused_value != "resource-from-first-acquire") {
        std::cerr << "resource was not reused after release" << std::endl;
        return 1;
    }
    std::cout << "async regression smoke passed" << std::endl;
    return 0;
}
