#ifndef SGEVOL_TRIG_LERP_HPP_INCLUDED
#define SGEVOL_TRIG_LERP_HPP_INCLUDED

#include <cmath>
#include <fcppt/math/pi.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_floating_point.hpp>

namespace sgevol
{

template< typename T > 
typename 
boost::enable_if< boost::is_floating_point< T >, T >::type
trig_lerp( 
	T const t,
	T const x,
	T const y )
{
	T const f =
		static_cast<T>(0.5) * 
			(
			static_cast<T>(1) -
				std::cos( 
					fcppt::math::pi<T>() * 
					t
				)
			);
	return 
		x * f + y * ( static_cast<T>(1) - f );
}

}

#endif
