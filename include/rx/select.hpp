#pragma once

#include <rx/core.hpp>

#include <boost/utility/result_of.hpp>

namespace rx
{
  template<class Source, class Args>
  class SelectObservable;

  template<class Selector>
  class Select
  {
  public:
    template<class Next, class Args>
    friend class SelectObserver;

    typedef typename Selector::result_type output_type;

    Select(Selector const& selector_)
      : selector(selector_)
    {}
  
    template<class Observable>
    struct observable_type
    {
      typedef SelectObservable<Observable, Select<Selector> > type;
    };
  
    template<class Observable>
    typename observable_type<Observable>::type operator()(Observable next) const
    {
      return typename observable_type<Observable>::type(next, *this);
    }

  private:
    Selector selector;
  };

  template<class Next, class Args>
  class SelectObserver : private Next
  {
  public:

    SelectObserver(Next next, Args const& args_)
      : Next(next)
      , args(args_)
    {}

    template<class T>
    void onnext(T const& value)
    {
      try
        {
          Next::onnext(args.selector(value));
        }
      catch (std::exception const& error)
        {
          Next::onerror(error.what());
        }
    }

    using Next::onerror;
    using Next::oncompleted;

    // void onerror(error_type const& error)
    // {
    //   Next::onerror(error);
    // }

    // void oncompleted()
    // {
    //   Next::oncompleted();
    // }

  private:
    Args args;
  };

  template<class Source, class Args>
  class SelectObservable : public Source
  {
  public:
    typedef typename Args::output_type value_type;

    SelectObservable(Source source, Args const& args_)
      : Source(source)
      , args(args_)
    {}

    template<class Next>
    typename Source::disposable_type subscribe(Next next)
    {
      return Source::subscribe(SelectObserver<Next, Args>(next, args));
    }

  private:
    Args args;
  };

  template<class Selector>
  Select<Selector> select(Selector const& selector)
  {
    return Select<Selector>(selector);
  }

}
