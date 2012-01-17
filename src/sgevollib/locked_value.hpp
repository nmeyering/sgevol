#ifndef SGEVOLLIB_LOCKED_VALUE_HPP_INCLUDED
#define SGEVOLLIB_LOCKED_VALUE_HPP_INCLUDED

#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <fcppt/config/external_end.hpp>


namespace sgevollib
{
template<typename T>
class locked_value
{
FCPPT_NONCOPYABLE(locked_value);
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
}

#endif
