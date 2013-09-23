#pragma once

#include <rx/core.hpp>

#include <boost/utility/result_of.hpp>

namespace rx
{
  template<class Source, class Args>
  class WhereObservable;

  template<class Pred>
  class Where
  {
  public:
    template<class Next, class Args>
    friend class WhereObserver;

    Where(Pred const& pred_)
      : pred(pred_)
    {}
  
    template<class Observable>
    struct observable_type
    {
      typedef WhereObservable<Observable, Where<Pred> > type;
    };
  
    template<class Observable>
    typename observable_type<Observable>::type operator()(Observable next) const
    {
      return typename observable_type<Observable>::type(next, *this);
    }

  private:
    Pred pred;
  };

  template<class Next, class Args>
  class WhereObserver : private Next
  {
  public:

    WhereObserver(Next next, Args const& args_)
      : Next(next)
      , args(args_)
    {}

    template<class T>
    void onnext(T const& value)
    {
      // try
      //   {
          if (args.pred(value))
            {
              Next::onnext(value);
            }
      //   }
      // catch (std::exception const& error)
      //   {
      //     Next::onerror(error.what());
      //   }
    }

    using Next::onerror;
    using Next::oncompleted;

  private:
    Args args;
  };

  template<class Source, class Args>
  class WhereObservable : public Source
  {
  public:

    WhereObservable(Source source, Args const& args_)
      : Source(source)
      , args(args_)
    {}

    template<class Next>
    typename Source::disposable_type subscribe(Next next)
    {
      return Source::subscribe(WhereObserver<Next, Args>(next, args));
    }

  private:
    Args args;
  };

  template<class Pred>
  Where<Pred> where(Pred const& pred)
  {
    return Where<Pred>(pred);
  }

}
