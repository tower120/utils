#pragma once

#include <functional>
#include <utility>


#include "RangeApplicatorBase.h"

#include "Applied.h"
#include "match.h"
#include "lambda_args.h"

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
    if (i > 2) return utils::FlowControl::Break;
	std::cout << i;
    return utils::FlowControl::Continue;
}});

*/


namespace utils {

	namespace IterateDirection {
		struct Forward {} forward;
		struct Reverse {} reverse;
	}

	template<class T>
	class RangeApplicator : public RangeApplicatorBase<T>
						  , public AppliedMaker {
        using Base = RangeApplicatorBase<T>;
        using Self = RangeApplicator<T>;
		const bool reverse = false;

        using Base::fn;
        using Base::iterate;
	public:

		template<class Fn, class = std::enable_if_t< !std::is_same< std::decay_t<Fn>,  Self>::value > >
		RangeApplicator(Fn&& fn)
			:Base(std::forward<Fn>(fn)) {}

		template<class Fn>
		RangeApplicator(IterateDirection::Reverse, Fn&& fn)
			: Base(std::forward<Fn>(fn))
			, reverse(true) {}

		template<class Fn>
		RangeApplicator(IterateDirection::Forward, Fn&& fn)
			: Base(std::forward<Fn>(fn))
			, reverse(false) {}


		template<class Range, class RangeRev>
		Applied apply_range(Range&& range, RangeRev&& renge_rev) const {
			if (!reverse) {
                iterate(std::forward<Range>(range));
			} else {
                iterate(std::forward<RangeRev>(renge_rev));
			}

			return makeApplied();
		}

		template<class Range>
		Applied apply_range(Range&& range) const {
			if (!reverse) {
                iterate(std::forward<Range>(range));
			}
			else {

				auto i = range.end();
				const auto begin = range.begin();
				while (i != begin)
				{
					--i;
                    /*fn(*i);*/

                    const auto r = fn(*i);
                    if (r == FlowControl::Break){
                        break;
                    }
				}
			}

			return makeApplied();
		}


		template<class ...Fns>
		Applied apply_match(Fns&&... fns) const {
			if (!reverse) {
                iterate(utils::match(IterateDirection::Forward{}, std::forward<Fns>(fns)...));
			} else {
                iterate(utils::match(IterateDirection::Reverse{}, std::forward<Fns>(fns)...));
			}

			return makeApplied();
		}

	};
	
}