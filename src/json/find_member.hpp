#ifndef SGEVOL_JSON_FIND_MEMBER_HPP_INCLUDED
#define SGEVOL_JSON_FIND_MEMBER_HPP_INCLUDED

#include "convert.hpp"
#include "member_has_type.hpp"
#include "is_null.hpp"
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/find_member_exn.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/member_name_equal.hpp>
#include <sge/parse/json/member_vector.hpp>
#include <fcppt/string.hpp>
#include "fcppt/exception.hpp"
#include <boost/range/numeric.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp> 
#include <boost/spirit/home/phoenix/operator/member.hpp> 
#include <boost/spirit/home/phoenix/bind.hpp> 
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/core/reference.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>
#include <algorithm>

namespace sgevol
{
namespace json
{
template<typename T>
T const 
find_member(
	sge::parse::json::object const &o,
	fcppt::string const &path)
{
	typedef
	std::vector<fcppt::string>
	string_vector;

	string_vector parts;

	boost::algorithm::split(
		parts,
		path,
		boost::algorithm::is_any_of(FCPPT_TEXT("/")));

	fcppt::string const last_element = 
		parts.back();

	parts.pop_back();

	sge::parse::json::object const *target = 
		boost::accumulate(
			parts,
			&o,
			&boost::phoenix::bind(
				&sge::parse::json::find_member_exn
				<
					sge::parse::json::object const,
					sge::parse::json::member_vector const
				>,
				boost::phoenix::bind(
					&sge::parse::json::object::members,
					boost::phoenix::arg_names::arg1),
				boost::phoenix::arg_names::arg2));

	sge::parse::json::member_vector::const_iterator it = 
		std::find_if(
			target->members.begin(),
			target->members.end(),
			sge::parse::json::member_name_equal(
				last_element));

	if (it == target->members.end())
		throw fcppt::exception(
			FCPPT_TEXT("Couldn't find member \"")+last_element+FCPPT_TEXT("\""));

	if(is_null(it->value))
		throw fcppt::exception(FCPPT_TEXT("The member \"")+path+FCPPT_TEXT("\" is null"));

	try
	{
		return 
			json::convert<T>(
				it->value);
	}
	catch (sge::parse::json::invalid_get const &e)
	{
		throw fcppt::exception(
			FCPPT_TEXT("Unable to parse \"")+
			it->name+
			FCPPT_TEXT("\": ")+
			e.string());
	}
}
}
}

#endif