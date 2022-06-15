#pragma once

namespace change_me
{
	class DHookWrapper {};
	class VHookWrapper {};

	class Hook
	{
	public:
		friend class Hooking;

		inline Hook(std::string_view Name) : m_Name(Name), m_Target(0),
			m_Created(false), m_Enabled(false)
			{}

		virtual void CreateHook() = 0;
		virtual void DestroyHook() = 0;

		virtual void ReHook() = 0;

		virtual void EnableHook() = 0;
		virtual void DisableHook() = 0;

	protected:

		PointerMath m_Target;

		std::string_view m_Name;

		bool m_Created;
		bool m_Enabled;
	};
	class DHook : public Hook
	{
	public:

		inline DHook(std::string_view Name, PointerMath Target, void* Detour) : Hook(Name),
			m_Detour(PointerMath(std::uintptr_t(Detour))), m_Original(0)
		{
			if (Target.As<std::uintptr_t>() == 0)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) 
					<< "] the target address is not valid!";
				return;
			}

			m_Target = Target;
		}

		inline void CreateHook() override
		{
			if (m_Created)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) << "] already created!";
				return;
			}
			else if (m_Detour.As<std::uintptr_t>() == 0)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) 
					<< "] the detour address is not valid!";
				return;
			}
			else if (m_Name.length() == 0)
			{
				LOG(WARNING) << "Hook at address [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Target.As<std::uintptr_t>()) 
					<< "] doesn't have a valid name!";
				return;
			}

			void* Orig = nullptr;
			auto Res = MH_CreateHook(m_Target.As<void*>(), m_Detour.As<void*>(), &Orig);

			if (Res != MH_OK)
			{
				LOG(WARNING) << "Could not create hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) 
					<< "], error: " << MH_StatusToString(Res);
				return;
			}

			m_Original = PointerMath(std::uintptr_t(Orig));

			LOG(INFO) << "Created hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
				m_Name) << "]";

			m_Created = true;
		}
		inline void DestroyHook() override
		{
			if ((!m_Enabled) && m_Created)
			{
				auto Res = MH_RemoveHook(m_Target.As<void*>());

				if (Res != MH_OK)
				{
					LOG(WARNING) << "Could not destroy hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) 
						<< "], error: " << MH_StatusToString(Res);
					return;
				}

				m_Created = false;
			}
		}

		inline void ReHook() override
		{
			DisableHook();
			EnableHook();
		}

		inline void EnableHook() override
		{
			if (!m_Created)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) << "] hasn't been created!";
				return;
			}
			else if (m_Enabled)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) << "] already enabled";
				return;
			}

			auto Res = MH_EnableHook(m_Target.As<void*>());

			if (Res != MH_OK)
			{
				LOG(WARNING) << "Could not enable hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) << "], error: " 
					<< MH_StatusToString(Res);
				return;
			}

			m_Enabled = true;
		}
		inline void DisableHook() override
		{
			if (m_Enabled && m_Created)
			{
				auto Res = MH_DisableHook(m_Target.As<void*>());

				if (Res != MH_OK)
				{
					LOG(WARNING) << "Could not disable hook " << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) 
						<< " ,error: " << MH_StatusToString(Res);
					return;
				}

				LOG(INFO) << "Disabled hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) << "]";

				m_Enabled = false;
			}
		}

		template<typename T>
		inline std::enable_if_t<std::is_pointer<T>::value, T> Get()
		{
			if (!m_Created || !m_Enabled)
			{
				LOG(WARNING) << "Could not get original of hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) 
					<< "], the hook has not been enabled or created!";
				return nullptr;
			}

			return m_Original.As<T>();
		}
	private:

		PointerMath m_Detour;
		PointerMath m_Original;
	};
	class VMTHook : public Hook
	{
	public:

		inline VMTHook(std::string_view Name, PointerMath Target, std::size_t NumFuncs) : Hook(Name),
			m_NumFuncs(NumFuncs), m_OriginalTable(nullptr), m_NewTable(nullptr)
		{
			if (Target.As<std::uintptr_t>() == 0)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) 
					<< "] the target address is not valid!";
				return;
			}
			m_Target = Target;
		}

		inline void CreateHook() override
		{
			if (m_Created)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) 
					<< "] already created";
				return;
			}
			else if (m_NumFuncs == 0)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Name) << "] the number of methods is not valid!";
				return;
			}
			else if (m_Name.size() == 0)
			{
				LOG(WARNING) << "Hook at address [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA, m_Target.As<std::uintptr_t>()) 
					<< "] name is not valid!";
				return;
			}

			auto Object = m_Target.As<void***>();
			m_OriginalTable = *Object;

			m_NewTable = new void*[m_NumFuncs];

			std::copy_n(m_OriginalTable, m_NumFuncs, m_NewTable); /*copy the ptrs from the original table 
																  to our new table*/
			LOG(INFO) << "Created hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
				m_Name) << "] with " << m_NumFuncs << " methods";

			m_Created = true;
		}
		inline void DestroyHook()
		{
			if ((!m_Enabled) && m_Created)
			{
				delete m_NewTable;
				m_OriginalTable = nullptr;

				m_Created = false;
			}
		}

		inline void ReHook() override
		{
			DisableHook();
			EnableHook();
		}

		inline void SetupHook(std::size_t Index, void * VFunc, std::string_view Name)
		{
			if (Index >= m_NumFuncs)
			{
				LOG(WARNING) << "Could not setup hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "]::" << Name << ", the index is not valid!";
				return;
			}
			else if (!VFunc)
			{
				LOG(WARNING) << "Could not setup hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "]::" << Name << ", the function is not valid!";
				return;
			}
			else if (!m_Created)
			{
				LOG(WARNING) << "Could not setup hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "]::" << Name << " the hook has not been created!";
				return;
			}
			else if (m_Names.find(Index) != m_Names.end())
			{
				LOG(WARNING) << "Could not setup hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "]::" << Name << " the hook have already been setup up!";
				return;
			}

			m_NewTable[Index] = VFunc;
			m_Names.insert( { Index, Name } );

			LOG(INFO) << "Setup hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
				m_Name) << "]::" << Name;
		}
		inline void UnSetupHook(std::size_t Index)
		{
			if ((Index >= m_NumFuncs) || (m_Names.find(Index) == m_Names.end()))
			{
				LOG(WARNING) << "Could not unsetup hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "]::" << " the index is not valid!";
				return;
			}
			else if (!m_Created)
			{
				LOG(WARNING) << "Could not setup hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "]::" << m_Names[Index] << " the hook has not been created!";
				return;
			}

			m_NewTable[Index] = m_OriginalTable[Index];

			auto It = m_Names.find(Index);
			m_Names.erase(It);

			LOG(WARNING) << "Setup hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
				m_Name) << "]::" << m_Names[Index];
		}

		inline void EnableHook()
		{
			if (!m_Created)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "] hasn't been created!";
				return;
			}
			else if (m_Enabled)
			{
				LOG(WARNING) << "Hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "] already enabled";
				return;
			}

			*m_Target.As<void***>() = m_NewTable;

			m_Enabled = true;
		}
		inline void DisableHook()
		{
			if (m_Enabled && m_Created)
			{
			    *m_Target.As<void***>() = m_OriginalTable;
				m_Enabled = false;
			}
		}

		template<typename T>
		inline std::enable_if_t<std::is_pointer<T>::value, T> Get(std::size_t Index)
		{
			if ((Index >= m_NumFuncs) || (Index >= m_Name.size()))
			{
				LOG(WARNING) << "Could not get original of hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "]::" << ", the index is not valid!";
				return nullptr;
			}
			else if (!m_Enabled)
			{
				LOG(WARNING) << "Could not get original of hook [" << ADD_COLOR_TO_TEXT(LogColor::MAGENTA,
					m_Name) << "]::" << m_Names[Index] << ", the hook has not been enabled!";
				return  nullptr;
			}

			return reinterpret_cast<T>(m_OriginalTable[Index]);
		}
	private:

		std::map<std::size_t, std::string_view> m_Names;

		void** m_NewTable;
		void** m_OriginalTable;

		std::size_t m_NumFuncs;
	};

	template<typename HookType, typename PtrCast>
	class HookWrapper;

	/*/////////////// PointerImpl /////////////////*/

	extern void AddHook(std::shared_ptr<Hook> Hk); 
	/*a little trick we are going to use to get rid out of the "g_Hooking not defined" error*/

	template<typename PtrCast>
	class HookWrapper<DHookWrapper, PtrCast> : public PointerImpl<PtrCast>
	{
	public:

		inline HookWrapper(std::string_view Name, std::uintptr_t Address, void* Detour) :
			PointerImpl<PtrCast>(Name, Address)
		{
			m_Hook = std::make_shared<DHook>(Name, PointerMath(Address), Detour);
			AddHook(m_Hook);
		}

		inline HookWrapper(std::string_view Name, PointerMath Address, void* Detour) :
			PointerImpl<PtrCast>(Name, Address)
		{
			m_Hook = std::make_shared<DHook>(Name, Address, Detour);
			AddHook(m_Hook);
		}

		inline HookWrapper() : PointerImpl<PtrCast>()
		{}

		PtrCast Get()
		{
			return m_Hook->Get<PtrCast>();
		}

		inline std::shared_ptr<DHook>& GetHook()
		{
			return m_Hook;
		}

	private:

		std::shared_ptr<DHook> m_Hook;
	};

	template<typename PtrCast>
	class HookWrapper<VHookWrapper, PtrCast> : public PointerImpl<PtrCast>
	{
	public:

		inline HookWrapper(std::string_view Name, std::uintptr_t Address, std::size_t NumFuncs) :
			m_Hook(std::make_shared<VMTHook>(Name, NumFuncs)), PointerImpl<PtrCast>(Name, Address)
		{
			m_Hook = std::make_shared<VMTHook>(Name, PointerMath(Address), NumFuncs);
			AddHook(m_Hook);
		}

		inline HookWrapper(std::string_view Name, PointerMath Address, std::size_t NumFuncs) :
			m_Hook(std::make_shared<VMTHook>(Name, NumFuncs)), PointerImpl<PtrCast>(Name, Address)
		{
			m_Hook = std::make_shared<VMTHook>(Name, Address, NumFuncs);
			AddHook(m_Hook);
		}

		inline HookWrapper() : PointerImpl<PtrCast>()
		{}

		inline PtrCast Get(std::size_t Index)
		{
			return m_Hook->Get<PtrCast>(Index);
		}

		inline void SetupHook(std::size_t Index, void* VFunc, std::string_view Name)
		{
			m_Hook->SetupHook(Index, VFunc, Name);
		}

		inline void UnSetupHook(std::size_t Index)
		{
			m_Hook->UnSetupHook(Index);
		}

		inline std::shared_ptr<VMTHook> GetHook()
		{
			return m_Hook;
		}

	private:

		std::shared_ptr<VMTHook> m_Hook;
	};
}