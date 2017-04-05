#pragma once

#include <tuple>

// Not work with generic lambdas
namespace utils {
	template <typename T>
	struct LambdaArgs : LambdaArgs<decltype(&T::operator())> {};

	template <typename R, typename... Args>
	struct FunctionArgsBase {
		using args = std::tuple<Args...>;
		using arity = std::integral_constant<unsigned, sizeof...(Args)>;
		using result = R;
	};

	template <typename R, typename... Args>
	struct LambdaArgs<R(*)(Args...)> : FunctionArgsBase<R, Args...> {};
	template <typename R, typename C, typename... Args>
	struct LambdaArgs<R(C::*)(Args...)> : FunctionArgsBase<R, Args...> {};
	template <typename R, typename C, typename... Args>
	struct LambdaArgs<R(C::*)(Args...) const> : FunctionArgsBase<R, Args...> {};


	template<typename TFunc>
	constexpr auto lambda_args(TFunc)
	{
		return LambdaArgs<TFunc>{};
	}
}