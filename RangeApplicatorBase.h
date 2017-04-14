#pragma once

#include <functional>

#include "lambda_args.h"

namespace utils {

	namespace FlowControl {
		static const constexpr bool Break = false;
		static const constexpr bool Continue = true;
	}

	template<class T>
	class RangeApplicatorBase {
		using Self = RangeApplicatorBase<T>;
	protected:
		const std::function<bool(T)> fn;

		template<class Fn>
		decltype(auto) makeFn(Fn&& fn, std::false_type is_void) const {
			return std::forward<Fn>(fn);
		}
		template<class Fn>
		decltype(auto) makeFn(Fn&& fn, std::true_type is_void) const {
			return[f = std::forward<Fn>(fn)](T value) {
				f(value);
				return FlowControl::Continue;
			};
		}

		template<class Fn>
		decltype(auto) makeFn(Fn&& fn) const {
			using R = typename LambdaArgs< std::decay_t<Fn> >::result;
			return makeFn(std::forward<Fn>(fn), std::is_same<void, R>{});
		}

		template<class Range>
		void iterate(Range&& range) const {
			/*auto && __range  = range;
			auto __begin 	 = range.begin();
			const auto __end = range.end();
			for (; __begin != __end; ++__begin) {
				const auto r = fn(element);
				if (r == FlowControl::Break) {
					break;
				}
			}*/

			for (auto&& element : std::forward<Range>(range)) {
				const auto r = fn(element);
				if (r == FlowControl::Break) {
					break;
				}
			}
		}

		template<class Element, class ...Elements>
		void iterate_sequence_forward(Element&& element, Elements&&... elements) const {
			const auto r = fn(std::forward<Element>(element));
			if (r == FlowControl::Break) {
				return;
			} else {
				iterate_sequence_forward(std::forward<Elements>(elements)...);
			}
		}
        template<class Element>
        void iterate_sequence_forward(Element&& element) const {
            fn(std::forward<Element>(element));
        }



        template<class Tup, std::size_t... I>
        void iterate_sequence_reverse(Tup&& t, std::index_sequence<I...>) const {
            iterate_sequence_forward(
                std::get<sizeof...(I) -1 - I>( std::forward<Tup>(t) )...
            );
        }
        template<class ...Elements>
        void iterate_sequence_reverse(Elements&&... elements) const {
            iterate_sequence_reverse(
                std::forward_as_tuple(std::forward<Elements>(elements)...),
                std::index_sequence_for<Elements...>{}
            );
        }


        /*template<class Element, class ...Elements>
        void iterate_sequence_reverse(Element&& element, Elements&&... elements) const {
            iterate_sequence_reverse(std::forward<Elements>(elements)...);

            const auto r = fn(std::forward<Element>(element));
            if (r == FlowControl::Break) {
                return;
            } else {
                iterate_sequence_reverse(std::forward<Elements>(elements)...);
            }
        }
        template<class Element>
        void iterate_sequence_reverse(Element&& element) const {
            fn(std::forward<Element>(element));
        }*/



		template<class Fn, class = std::enable_if_t< !std::is_same< std::decay_t<Fn>,  Self>::value > >
		RangeApplicatorBase(Fn&& fn)
			: fn(makeFn(std::forward<Fn>(fn))){};
	};
}