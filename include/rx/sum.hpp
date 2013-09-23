#pragma once

#include <rx/core.hpp>

namespace rx
{

  template<class T, class Next>
  struct SumObserver : private Next
  {
  public:
    SumObserver(Next next)
      : Next(next)
      , sum()
    {}

    void onnext(T const& value)
    {
      sum += value;
    }

    using Next::onerror;

    void oncompleted()
    {
      Next::onnext(sum);
      Next::oncompleted();
    }

  private:
    T sum;
  };

  template<class Source>
  class SumObservable : public Source
  {
  public:

    SumObservable(Source source)
      : Source(source)
    {}

    template<class Next>
    typename Source::disposable_type subscribe(Next next)
    {
      return Source::subscribe(SumObserver<typename Source::value_type, Next>(next));
    }
  };

  class sum
  {
  public:
  
    template<class Observable>
    struct observable_type
    {
      typedef SumObservable<Observable> type;
    };
  
    template<class Observable>
    SumObservable<Observable> operator()(Observable next) const
    {
      return SumObservable<Observable>(next);
    }
  };

}
