#pragma once

#include <functional>
#include <utility>

#include "match.h"

#include <range/v3/core.hpp>
#include <range/v3/view/reverse.hpp>



/*

utils::RangeApplicator<int>::Applied iterate(const utils::RangeApplicator<int>& applicator) {
	using namespace utils::IterateDirection;

	return applicator.apply_match(
		[](Forward) {
			return view::all(list);
		}
		,[](Reverse) {
			return view::reverse( view::all(list) );
		}
	);

	return applicator.apply_range(list);

	return applicator.apply_range(list, view::reverse(list) );
}


iterate([](int i){
	std::cout << i; 
});
iterate({ utils::IterateDirection::Reverse(), [](int i) {
	std::cout << i;
} });

iterate(utils::IterateDirection::Reverse() | [](int i) {
	std::cout << i;
} );

*/


namespace utils {

	template<class>
	class RangeApplicator;

	namespace IterateDirection {
		struct Forward {};
		struct Reverse {};
	}

	template<class T>
	class RangeApplicator {
		using Self = RangeApplicator<T>;

		const std::function<void(T)> fn;
		const bool reverse = false;

	public:
		class Applied {
			friend class RangeApplicator<T>;
			Applied() {}

		public:
			Applied(const Applied&) = delete;
			Applied(Applied&&) = default;
		};


		template<class Fn, class = std::enable_if_t< !std::is_same< std::decay_t<Fn>,  Self>::value > >
		RangeApplicator(Fn&& fn)
			:fn(std::forward<Fn>(fn)) {}

		template<class Fn>
		RangeApplicator(IterateDirection::Reverse, Fn&& fn)
			: fn(std::forward<Fn>(fn))
			, reverse(true) {}

		template<class Fn>
		RangeApplicator(IterateDirection::Forward, Fn&& fn)
			: fn(std::forward<Fn>(fn))
			, reverse(false) {}



		template<class Range, class RangeRev>
		Applied apply_range(Range&& range, RangeRev&& renge_rev) const {
			if (!reverse) {
				for (auto&& element : range) {
					fn(element);
				}
			}
			else {
				for (auto&& element : renge_rev) {
					fn(element);
				}
			}

			return{};
		}

		template<class Range>
		Applied apply_range(Range&& range) const {
			using namespace ranges;

			if (!reverse) {
				for (auto&& element : range) {
					fn(element);
				}
			}
			else {
				for (auto&& element : view::reverse(range)) {
					fn(element);
				}
			}

			return {};
		}


		template<class ...Fns>
		Applied apply_match(Fns&&... fns) const {
			if (!reverse) {
				for (auto&& element : utils::match(IterateDirection::Forward{}, std::forward<Fns>(fns)...)) {
					fn(element);
				}
			}
			else {
				for (auto&& element : utils::match(IterateDirection::Reverse{}, std::forward<Fns>(fns)...)) {
					fn(element);
				}
			}

			return {};
		}

	};


	
}