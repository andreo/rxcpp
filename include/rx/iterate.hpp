#pragma once

#include <rx/core.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <iostream>

namespace rx
{
  template<class Next>
  class IterateState : private Next
  {
  public:
    IterateState(Next next, int from_, int to_)
      : Next(next)
      , from(from_)
      , to(to_)
    {}

    void start()
    {
      try
        {
          for (int i=from; i<to; ++i) {
            Next::onnext(i);
          }
          Next::oncompleted();
        }
      catch(std::exception const& error)
        {
          Next::onerror(error.what());
        }
    }

  private:
    int from;
    int to;
  };

  template<class Scheduler>
  class Iterate
  {
  public:
    Iterate(int from_, int to_, Scheduler& scheduler_)
      : from(from_)
      , to(to_)
      , scheduler(scheduler_)
    {}

    typedef int value_type;
    typedef typename Scheduler::disposable_type disposable_type;

    template<class Next>
    disposable_type subscribe(Next next)
    {
      // std::cout << "IterateState<Next>: " << sizeof(IterateState<Next>) << std::endl;
      auto state = boost::make_shared<IterateState<Next> >(next, from, to);
      return scheduler.schedule(boost::bind(&IterateState<Next>::start, state));
    }

  private:
    int from;
    int to;
    Scheduler& scheduler;
  };

  template<class Scheduler>
  Iterate<Scheduler> iterate(int from, int to, Scheduler& scheduler)
  {
    return Iterate<Scheduler>(from, to, scheduler);
  }
}
