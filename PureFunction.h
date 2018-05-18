#pragma once

#include <array>

namespace utils{
        
    template<class>
    class PureFunction;

    template<class Ret, class ...Args>
    class PureFunction<Ret(Args...)>{
        Ret (*fn)(Args...);

        template<class Fn>
        void emplace_lambda(const Fn& fn) {
            //  The closure type for a lambda-expression with no lambda-capture has a public non-virtual
            // non-explicit const conversion function to pointer to function having the same parameter
            // and return types as the closure type’s function call operator. The value returned by this
            // conversion function shall be the address of a function that, when invoked, has the same effect
            // as invoking the closure type’s function call operator.
            //
            // https://stackoverflow.com/a/28746827
        
            this->fn = fn;
        }
    public:
        template<class Fn, typename = std::enable_if_t< std::is_invocable_v<Fn> > >
        void operator=(const Fn& fn){
            emplace_lambda(fn);
        }

        Ret operator()(Args... args) const {
            return fn(args...);
        }
    };
        
}
