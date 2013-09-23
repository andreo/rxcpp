#pragma once

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

#include <deque>

namespace rx
{
  class EventLoopScheduler : boost::noncopyable
  {
  public:
    typedef boost::function<void()> task;

  private:
    typedef std::deque<task> queue_type;

    struct EmptyFunction
    {
      void operator()() const {}
    };

  public:

    class Disposable
    {
    private:
      Disposable(queue_type::size_type id,
                 EventLoopScheduler& scheduler)
        : m_id(id)
        , m_scheduler(scheduler)
      {}

    public:

      friend EventLoopScheduler;

      void dispose() {
        m_scheduler.removetask(m_id);
      }

    private:
      queue_type::size_type m_id;
      EventLoopScheduler& m_scheduler;
    };

    typedef Disposable disposable_type;

    EventLoopScheduler()
      : m_queue()
      , m_mutex()
      , m_start()
    {}

    disposable_type schedule(task const& t)
    {
      boost::mutex::scoped_lock lock(m_mutex);

      m_queue.push_back(t);

      return Disposable(m_start + m_queue.size() - 1, *this);
    }

    void pumpLoop()
    {
      task t;
      while (pop(t))
        {
          t();
        }
    }
    
  private:

    void removetask(queue_type::size_type id)
    {
      boost::mutex::scoped_lock lock(m_mutex);

      if (m_start <= id) {
        m_queue[id - m_start] = EmptyFunction();
      }
    }

    bool pop(task& t)
    {
      boost::mutex::scoped_lock lock(m_mutex);

      if (m_queue.empty())
        {
          return false;
        }
      else
        {
          t = m_queue.front();
          m_queue.pop_front();
          ++m_start;
          return true;
        }
    }

  private:
    boost::mutex m_mutex;
    queue_type::size_type m_start;
    queue_type m_queue;
  };
}
