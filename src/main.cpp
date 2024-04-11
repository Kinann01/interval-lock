#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "../include/interval_tree.hpp"
#include "../include/locker.hpp"

void run_test() {
    locker locker_;

    // the loops test for re-locking/memory management
    for (std::size_t i = 0; i < 100; ++i) {
        for (std::size_t j = 0; j < 100; ++j) {
            // these two shouldn't block each other
            auto lock = locker_.lock_exclusive(0 + i, 10 + i);
            auto lock2 = locker_.lock_exclusive(10 + i, 20 + i);

            // these two shouldn't block each other
            auto lock3 = locker_.lock_shared(20 + i, 30 + i);
            auto lock4 = locker_.lock_shared(20 + i, 30 + i);
        }
    }

        {
        std::vector<exclusive_lock> locks;

        std::size_t width = 1'000'000; // big width so we test against bitmap-like solutions

        // this almost randomly creates non-overlapping exclusive locks
        for (std::size_t i = 0; i < 7000; i += 7) {
            std::size_t offset = (i % 1000) * width;
            // these shouldn't block
            locks.emplace_back(locker_.lock_exclusive(offset, offset + width));
        }
    }

    {
        std::vector<shared_lock> locks;

        std::size_t width = 1'000'000; // big width so we test against bitmap-like solutions

        // this almost randomly creates overlapping shared locks
        for (std::size_t i = 0; i < 7000; i += 7) {
            std::size_t offset = (i % 500) * width;
            // these two shouldn't block each other
            locks.emplace_back(locker_.lock_shared(offset, offset + width));
        }
    }

    {
        // moves work
        auto lock1 = locker_.lock_exclusive(0, 10);
        auto lock2 = exclusive_lock{};


        for (std::size_t i = 0; i < 1000; ++i){
            std::swap(lock2, lock1);
        }
    }

    {
        // moves work
        auto lock1 = locker_.lock_shared(0, 10);
        auto lock2 = shared_lock{};

        for (std::size_t i = 0; i < 1000; ++i) {
            std::swap(lock2, lock1);
        }
    }

    {
        // explicit unlocks work
        auto lock1 = locker_.lock_exclusive(0, 10);
        lock1.unlock();

        auto lock2  = locker_.lock_shared(0, 10);
        lock2.unlock();

        lock1 = locker_.lock_exclusive(0, 10);
    }

    {
        // two lockers are independent
        locker other_locker;
        auto lock1 = locker_.lock_exclusive(0, 10);

        auto lock2 = other_locker.lock_exclusive(0, 10);
        lock2.unlock();

        auto lock3 = other_locker.lock_shared(0, 10);

        (void)lock1, (void)lock3;
    }
}

int main() {
    run_test();
    std::cout << "OK" << std::endl;
}