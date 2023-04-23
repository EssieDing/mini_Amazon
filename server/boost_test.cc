#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <future>

boost::asio::thread_pool g_pool(10);

int f(int i) {
    std::cout << '(' + std::to_string(i) + ')';
    return i * i;
}

int main() {
    std::cout << std::unitbuf;
    std::future<int> answer;

    for (size_t i = 0; i != 50; ++i) {
        auto task = boost::bind(f, 10 * i);
        if (i == 42) {
            answer = post(g_pool, std::packaged_task<int()>(task));
        } else
        {
            post(g_pool, task);
        }
    }

    answer.wait(); // optionally make sure it is ready before blocking get()
    std::cout << "\n[Answer to #42: " + std::to_string(answer.get()) + "]\n";

    // wait for remaining tasks
    g_pool.join();
}