#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <string>

namespace rx
{

  struct Disposable
  {
  private:
    class State
    {
    public:
      State()
        : m_disposed(false)
      {}

      volatile bool m_disposed;
    };

  public:
    Disposable()
      : m_state(boost::make_shared<State>())
    {}

    void dispose() const {
      m_state->m_disposed = true;
    }

    bool isdisposed() const {
      return m_state->m_disposed;
    }

  private:
    boost::shared_ptr<State> m_state;
  };

  typedef std::string error_type;

  template<class Observable, class ObservableFactory>
  typename ObservableFactory::template observable_type<Observable>::type operator|(Observable ob, ObservableFactory factory)
  {
    return factory(ob);
  }

  template<class T, class Next>
  class observer
  {
  public:
    typedef T value_type;

    observer(Next next)
      : m_next(next)
    {}

    void next(T const& value)
    {
      m_next.onnext(value);
    }

    void error(error_type const& error)
    {
      m_next.onerror(error);
    }

    void completed()
    {
      m_next.oncompleted();
    }

  private:
    Next m_next;
  };

  template<class SourceObservable>
  class observable
  {
  public:
    typedef typename SourceObservable::value_type value_type;

  protected:
    SourceObservable m_source;
  };
}
