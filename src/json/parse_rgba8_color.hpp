#ifndef SGEVOL_JSON_PARSE_RGBA8_COLOR_HPP_INCLUDED
#define SGEVOL_JSON_PARSE_RGBA8_COLOR_HPP_INCLUDED

#include <sge/parse/json/value.hpp>
#include <sge/image/color/rgba8.hpp>

namespace sgevol
{
namespace json
{
sge::image::color::rgba8 const
parse_rgba8_color(
	sge::parse::json::value const &);
}
}

#endif
