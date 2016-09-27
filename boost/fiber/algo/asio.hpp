// An reference implementation for integrating Boost.{Asio,Fiber}.
// Public domain, credit me if you prefer to do so.
// TODOs require Asio refactor.

#ifndef BOOST_FIBER_ALGO_ASIO_H
#define BOOST_FIBER_ALGO_ASIO_H

#include "boost/config.hpp"

#include "boost/asio/io_service.hpp"
#include "boost/asio/steady_timer.hpp"

#include "boost/fiber/algo/algorithm.hpp"
#include "boost/fiber/context.hpp"
#include "boost/fiber/detail/config.hpp"
#include "boost/fiber/scheduler.hpp"
#include "boost/fiber/type.hpp"

#include <atomic>

#include <iostream>

namespace boost
{
namespace fibers
{
namespace algo
{
class BOOST_FIBERS_DECL asio_queue_data
{
private:
    friend class asio;
    using rqueue_t = scheduler::ready_queue_t;
    rqueue_t rqueue_;
    std::mutex rqueue_mtx_;
    std::condition_variable rqueue_condvar_;
    boost::asio::io_service &svc_;
    boost::asio::steady_timer timer_;
    bool polling_;

public:
    asio_queue_data(boost::asio::io_service &svc)
        : svc_(svc), timer_(svc_), polling_(false)
    {
    }
};

class BOOST_FIBERS_DECL asio : public algorithm
{
private:
    using lqueue_t = scheduler::ready_queue_t;

    lqueue_t lqueue_;
    std::mutex mtx_;
    std::condition_variable cnd_;
    boost::fibers::algo::asio_queue_data &data_;
    bool stop_;

public:
    asio(boost::fibers::algo::asio_queue_data &data) : data_(data), stop_(false)
    {
    }

    asio(asio const &) = delete;

    void awakened(context *ctx) noexcept
    {
        if(ctx->is_context(type::pinned_context))
        {
            // recognize when we're passed this thread's main fiber (or an
            // implicit library helper fiber): never put those on the shared
            // queue
            lqueue_.push_back(*ctx);
        }
        else
        {
            ctx->detach();
            std::unique_lock<std::mutex> lk(data_.rqueue_mtx_);
            // worker fiber, enqueue on shared queue
            data_.rqueue_.push_back(*ctx);
            data_.rqueue_condvar_.notify_one();
        }
    }

    context *pick_next() noexcept
    {
        context *ctx(nullptr);
        std::unique_lock<std::mutex> lk(data_.rqueue_mtx_);
        if(!stop_ && !data_.rqueue_.empty())
        {
            // pop an item from the ready queue
            ctx = &data_.rqueue_.front();
            data_.rqueue_.pop_front();
            lk.unlock();
            BOOST_ASSERT(nullptr != ctx);
            context::active()->attach(ctx);
            // attach context to current scheduler via the active fiber
            // of this thread
        }
        else
        {
            lk.unlock();
            if(!lqueue_.empty())
            {
                // nothing in the ready queue, return main or dispatcher fiber
                ctx = &lqueue_.front();
                lqueue_.pop_front();
            }
        }
        return ctx;
    }

    bool has_ready_fibers() const noexcept
    {
        std::unique_lock<std::mutex> lock(data_.rqueue_mtx_);
        return !data_.rqueue_.empty() || !lqueue_.empty();
    }

    void suspend_until(
        std::chrono::steady_clock::time_point const &time_point) noexcept
    {
        std::unique_lock<std::mutex> lock(data_.rqueue_mtx_);
        if(data_.polling_)
        {
            data_.rqueue_condvar_.wait_until(lock, time_point);
        }
        else
        {
            data_.polling_ = true;
            lock.unlock();
            // TODO: this is supposed to be polling all tasks with timeout
            data_.timer_.expires_at(time_point);
            if(data_.svc_.run_one())
            {
                data_.svc_.poll();
            }
            else
            {
                stop_ = true;
            }
            data_.polling_ = false;
        }
    }

    void notify() noexcept
    {
        if(data_.polling_)
        {
            // TODO: a better method to interrupt
            data_.svc_.post([] {});
        }
        else
        {
            // This is called when a remote thread adds pending task.
            data_.rqueue_condvar_.notify_one();
        }
    }
};
}
}
}

#endif // BOOST_FIBER_ALGO_ASIO_H
