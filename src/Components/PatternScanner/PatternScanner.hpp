#pragma once

namespace change_me
{
	class Pattern
	{
	public:
		Pattern(std::string_view Name, std::string_view PatternStr);

		std::string_view GetName();
		std::vector<std::uint8_t>& GetPatternBytes();

		/*didnt follow the naming style here for an easy understanding of the code*/
		using OnAdjustPtr_t = std::function<void(PointerMath& Ptr)>;
		OnAdjustPtr_t OnAdjustPtr; /*called just after getting the addr of the pattern*/

		using OnGetPtrsFromPtr_t = std::function<void(PointerMath Ptr)>; /*this one's not a reference because we don't want
																		   to modify it accidentally and cause errors*/
		OnGetPtrsFromPtr_t OnGetPtrsFromPtr; /*called after OnAdjustPtr. If we for example have a
									  g_GameClass and we have the  ID3D11Device, but we don't
									  know the ID3D11DeviceContext, we can use this to get it
									  from the ID3D11Device*/

		using OnFinalPtr_t = std::function<void(PointerMath& Ptr)>;
		OnFinalPtr_t OnFinalPtr; /*called after OnGetPtrsFromPtr, MUST be only overidden by PattermImpl and it's inherits*/

	private:

		void TransforPatternStrToBytes();

	private:

		std::string_view m_Name;
		
		std::string_view m_PatternStr;
		std::vector<std::uint8_t> m_PatternBytes; 
	};

	template<typename T>
	class PattermImpl : Pattern
	{
	public:

		/*we need to declare the methods here because if we do in the .cpp the compiler will complain*/
		inline PattermImpl(std::string_view Name, std::string_view PatternStr) : Pattern(Name, PatternStr)
			{}

		inline std::enable_if_t<std::is_pointer<T>::value, T> Get()
		{
			if (m_Ptr.As<std::uintptr_t>() != 0)
				return m_Ptr.As<T>;

			LOG(WARNING) << "The pointer " << m_Name << " isn't initialised!";
		}

	private:

		PointerMath m_Ptr; /*just a friendly var we will use to cast the ptr*/

	};


	class PatternScanner : public ComponentBase
	{
	public:

		PatternScanner();

		bool Initialize() override;
		bool Run() override;
		bool Uninitialize() override;

		/*we dont use std::shared_ptr because it will be a pain in the ass to add new ptrs, too much writting*/
		void Add(Pattern* Pattern, std::shared_ptr<Module> Module);

		void Scan();

	private:

		std::vector<std::pair<Pattern*, std::shared_ptr<Module>>> m_Patterns;

		std::size_t m_PatternHints; /*num of patterns that got correctly scanned*/
	};

}
