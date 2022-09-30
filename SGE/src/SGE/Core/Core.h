#ifndef CORE_H
#define CORE_H

#pragma once

#define BIT(x) (1 << x)
namespace SGE  {
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ...)
	{
		std::unique_ptr<T>(std::forward<Args>(args)...);
	}


	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Ref<T> CreateRef(Args&& ...args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}
#endif