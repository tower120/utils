#pragma once

#include "lambda_args.h"

/*
std::string s = "12";
cout << match(s
	,[](int& i) { return "int"; }
	,[](bool& b) { return "bool"; }
	,[](std::string& s) -> auto& { s += " GAV"; return s; }
	,[](auto j) { cout << "default one"; return j; }
);

*/

namespace utils {

	// forward declarations
	template<class T, class Case, class ...Cases>
	decltype(auto) match(T&& value, const Case& _case, const Cases&... cases);
	template<class T, class Case>
	decltype(auto) match(T&& value, const Case& _case);


	namespace details {
		template<class T, class Case, class ...OtherCases>
		decltype(auto) match_call(const Case& _case, T&& value, std::true_type, const OtherCases&... other) {
			return _case(std::forward<T>(value));
		}

		template<class T, class Case, class ...OtherCases>
		decltype(auto) match_call(const Case& _case, T&& value, std::false_type, const OtherCases&... other) {
			return match(std::forward<T>(value), other...);
		}
	}


	template<class T, class Case, class ...Cases>
	decltype(auto) match(T&& value, const Case& _case, const Cases&... cases) {
		using namespace std;
		using args = typename LambdaArgs<Case>::args;
		using arg = tuple_element_t<0, args>;
		using match = is_same<decay_t<arg>, decay_t<T>>;
		return details::match_call(_case, std::forward<T>(value), match{}, cases...);
	}

	// the last one is default
	template<class T, class Case>
	decltype(auto) match(T&& value, const Case& _case) {
		return _case(std::forward<T>(value));
	}

}