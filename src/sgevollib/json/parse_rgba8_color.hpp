#ifndef SGEVOLLIB_JSON_PARSE_RGBA8_COLOR_HPP_INCLUDED
#define SGEVOLLIB_JSON_PARSE_RGBA8_COLOR_HPP_INCLUDED

#include <sge/image/color/rgba8.hpp>
#include <sge/parse/json/value.hpp>


namespace sgevollib
{
namespace json
{
sge::image::color::rgba8 const
parse_rgba8_color(
	sge::parse::json::value const &);
}
}

#endif
