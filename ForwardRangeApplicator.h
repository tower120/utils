#pragma once

#include <functional>
#include <utility>

#include "RangeApplicatorBase.h"
#include "Applied.h"
#include "match.h"


/*

utils::ForwardRangeApplicator<int>::Applied iterate(const utils::ForwardRangeApplicator<int>& applicator) {
	return applicator.apply_range(list);
}

iterate([](int i){
	std::cout << i;
});

*/


namespace utils {

	template<class T>
	class ForwardRangeApplicator : public RangeApplicatorBase<T>
								 , public AppliedMaker {
		using Base = RangeApplicatorBase<T>;
		using Self = ForwardRangeApplicator<T>;

		using Base::iterate;
	public:

		template<class Fn, class = std::enable_if_t< !std::is_same< std::decay_t<Fn>, Self>::value > >
		ForwardRangeApplicator(Fn&& fn)
			:Base(std::forward<Fn>(fn)) {}

		template<class Range>
		Applied apply_range(Range&& range) const {
			iterate(std::forward<Range>(range));
			return makeApplied();
		}

	};

}