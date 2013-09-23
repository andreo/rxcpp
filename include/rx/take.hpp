#pragma once

#include <rx/core.hpp>

namespace rx
{
  template<class Source>
  class TakeObservable;

  class take
  {
  public:
    template<class Next>
    friend class TakeObserver;

    take(int n_)
      : n(n_)
    {}
  
    template<class Observable>
    struct observable_type
    {
      typedef TakeObservable<Observable> type;
    };
  
    template<class Observable>
    TakeObservable<Observable> operator()(Observable next) const
    {
      return TakeObservable<Observable>(next, *this);
    }

  private:
    int n;
  };

  template<class Next>
  class TakeObserver : private Next
  {
  public:

    TakeObserver(Next next, take const& args_)
      : Next(next)
      , args(args_)
    {}

    template<class T>
    void onnext(T const& value)
    {
      if (0 < args.n)
        {
          Next::onnext(value);
          --args.n;
          if (0 == args.n)
            {
              Next::oncompleted();
            }
        }
    }

    using Next::onerror;

    void oncompleted()
    {
      if (0 < args.n)
        {
          Next::oncompleted();
        }
    }

  private:
    take args;
  };

  template<class Source>
  class TakeObservable : public Source
  {
  public:
    TakeObservable(Source source, take const& args_)
      : Source(source)
      , args(args_)
    {}

    template<class Next>
    typename Source::disposable_type subscribe(Next next)
    {
      return Source::subscribe(TakeObserver<Next>(next, args));
    }

  private:
    take args;
  };

}
