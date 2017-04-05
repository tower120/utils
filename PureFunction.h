#pragma once

#include <array>

namespace utils{
// 4 times smaller than std::function
// stack allocated
template<class Ret, class ...Args>
class PureFunction {
        using Self = PureFunction<Ret, Args...>;

public:
        struct Interface {
                virtual Ret operator()(Args...) const = 0;
        };

private:
        std::array<char, sizeof(Interface)> memory;

public:
        template<class Concrete>
        void emplace() {
                static_assert(std::is_base_of<Interface, Concrete>::value, "You must inherit Interface");
                static_assert(sizeof(Concrete) <= sizeof(memory),    "PureFunction insufficient capacity");

                void* ptr = memory.data();
                new (ptr) Concrete();
        }

        template<class Fn>
        void emplace_lambda(Fn fn) {
                class Concrete final: public Interface, protected Fn {
                public:
                        virtual Ret operator()(Args... args) const override  {
                                return Fn::operator()(args...);
                        }

                        Concrete(Fn fn) :Fn(fn) {}
                };
                static_assert(sizeof(Concrete) <= sizeof(memory), "PureFunction insufficient capacity");

                void* ptr = memory.data();
                new (ptr) Concrete(fn);
        }

        template<class Fn>
        void operator=(Fn fn) {
                emplace_lambda(fn);
        }


        Ret operator()(Args... args) const {
                const void* ptr = memory.data();
                const Interface& fn = *static_cast<const Interface*>(ptr);

                return fn(args...);
        }
};
}