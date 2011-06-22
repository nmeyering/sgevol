#include <fcppt/container/grid/size_type.hpp>
#include <fcppt/container/grid/object.hpp>
#include <fcppt/assert.hpp>
#include <algorithm>

template
<
	typename Dest,
	typename Source,
	fcppt::container::grid::size_type N,
	typename Functor
>
fcppt::container::grid::object<Dest,N> const
zip_with(
	fcppt::container::grid::object<Source,N> const &g,
	fcppt::container::grid::object<Source,N> const &h,
	Functor const &f)
{
	FCPPT_ASSERT(
		g.dimension() == h.dimension());
	fcppt::container::grid::object<Dest,N> result(
		g.dimension());
	std::transform(
		g.begin(),
		g.end(),
		h.begin(),
		result.begin(),
		f);
	return result;
}