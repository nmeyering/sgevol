#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <fcppt/noncopyable.hpp>

template<typename T>
class locked_value
{
FCPPT_NONCOPYABLE(locked_value)
private:
	typedef
	boost::mutex
	lockable;

	typedef
	boost::lock_guard<lockable>
	lock;
public:
	typedef
	T
	value_type;

	explicit
	locked_value(
		value_type const &_value)
	:
		value_(
			_value)
	{
	}

	explicit
	locked_value()
	{
	}

	value_type const
	value() const
	{
		lock lock_(
			mutex_);
		return value_;
	}

	void
	value(
		value_type const &t)
	{
		lock lock_(
			mutex_);
		value_ = t;
	}
private:
	value_type value_;
	mutable lockable mutex_;
};

