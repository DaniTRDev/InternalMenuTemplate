#pragma once

namespace change_me
{
	template<typename T>
	class Singleton
	{

	public:

		Singleton() {}

		Singleton(Singleton const&) = delete;
		void operator=(Singleton const&) = delete;

		inline static T& Get()
		{
			static T Ptr;

			return Ptr;
		}

		inline static std::shared_ptr<T>& GetPtr()
		{
			static auto& Ref = Get();
			static auto Ptr = std::shared_ptr<T>(&Ref);
			return Ptr;
		}
	};

}