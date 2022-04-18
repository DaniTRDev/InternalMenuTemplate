#pragma once


namespace change_me
{
	class DHookWrapper;
	class VHookWrapper;

	class PointerType;
	class PatternType;

	class Pattern
	{
	public:

		friend class PatternScanner;

		Pattern(std::string_view PatternStr, std::string_view Name);
		Pattern();

		std::string_view GetName();
		std::vector<std::uint8_t>& GetPatternBytes();

	public:

		bool m_Scanned;

	private:

		void TransforPatternStrToBytes();

	private:

		std::string_view m_Name;

		std::string_view m_PatternStr;
		std::vector<std::uint8_t> m_PatternBytes;
	};

	template<typename T>
	class PointerImpl
	{
	public:
		friend class PatternScanner;

		inline PointerImpl(std::string_view Name, std::uintptr_t Address) : m_Name(Name), m_Ptr(Address)
		{}

		inline PointerImpl(std::string_view Name, PointerMath Address) : m_Name(Name), m_Ptr(Address)
		{}

		inline PointerImpl() : m_Ptr(0)
		{}

		inline std::enable_if_t<std::is_pointer<T>::value, T> GetPtr()
		{
			auto PtrAddr = this->m_Ptr.As<std::uintptr_t>();
			if (PtrAddr != 0)
				return reinterpret_cast<T>(PtrAddr);

			return nullptr;
		}

	private:

		PointerMath m_Ptr;
		std::string_view m_Name;
	};
}

