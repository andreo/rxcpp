
#include <rx/iterate.hpp>
#include <rx/sum.hpp>
#include <rx/take.hpp>
#include <rx/select.hpp>
#include <rx/where.hpp>

#include <rx/concurrency.hpp>

#include <boost/lexical_cast.hpp>

#include <iostream>

template<class T>
class ConsoleWriter
{
public:

  typedef T value_type;

  void onnext(T const& value)
  {
    std::cout << value << std::endl;
  }

  void onerror(rx::error_type const& error)
  {
    std::cerr << error << std::endl;
  }

  void oncompleted()
  {
    std::cout << "completed" << std::endl;
  }
};

class write_to_console
{
public:

  template<class Observable>
  struct observable_type
  {
    typedef typename Observable::disposable_type type;
  };

  template<class Observable>
  typename Observable::disposable_type operator()(Observable observable) const
  {
    return observable.subscribe(ConsoleWriter<typename Observable::value_type>());
  }

};

void run()
{
  // auto cancel = rx::Disposable();
  // cancel.dispose();
  // cancel.dispose();
  // cancel.dispose();

  rx::EventLoopScheduler scheduler;

  // auto numbers
  //   = rx::iterate(0, 15, scheduler)
  //   | rx::take(5)
  //   | rx::sum()
  //   ;

  // std::cout << sizeof(rx::iterate(0, 15, scheduler)) << std::endl;
  // std::cout << sizeof(numbers) << std::endl;

  // auto c1 = numbers | write_to_console();
  // auto c2 = numbers | write_to_console();
  // auto c3 = numbers | write_to_console();
  // auto c4 = numbers | write_to_console();
  // auto c5 = numbers | write_to_console();

  // auto cn = c1;
  // cn.dispose();

  // c1.dispose();
  // c1.dispose();
  // c1.dispose();

  // c2.dispose();
  // c3.dispose();
  // c4.dispose();

  struct ToString
  {
    typedef std::string result_type;

    template<class T>
    result_type operator()(T value) const
    {
      return boost::lexical_cast<std::string>(value) + " **";
    }
  };

  struct Odd
  {
    template<class T>
    bool operator()(T const& value) const
    {
      throw std::runtime_error("custom error");
      return 0 == (value % 2);
    }
  };

  auto numbers2
    = rx::iterate(0, 15, scheduler)
    | rx::where(Odd())
    | rx::take(10)
    | rx::select(ToString());

  numbers2 | write_to_console();

  auto x = numbers2;
  x | rx::take(3) | write_to_console();

  scheduler.pumpLoop();
}

int main()
{
  try
    {
      run();
    }
  catch(std::exception const& error)
    {
      std::cerr << error.what() << std::endl;
    }
  return 0; 
}
