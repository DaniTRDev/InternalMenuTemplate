#pragma once

namespace change_me
{
	class Hooking;
	enum class HookType : std::uint16_t
	{
		Detour, 
		VMT
	};

	class Hook
	{
	public:
		friend class Hooking;

		inline Hook(std::string_view Name, HookType Type) : m_Name(Name), m_Type(Type), m_Target(0),
			m_Created(false), m_Enabled(false)
			{}

		virtual void CreateHook(PointerMath Target) = 0;
		virtual void DestroyHook() = 0;

		virtual void ReHook() = 0;

		virtual void EnableHook() = 0;
		virtual void DisableHook() = 0;

	protected:

		PointerMath m_Target;

		std::string_view m_Name;
		HookType m_Type;

		bool m_Created;
		bool m_Enabled;
	};

	class DHook : public Hook
	{
	public:

		inline DHook(std::string_view Name, void* Detour) : Hook(Name, HookType::Detour),
			m_Detour(PointerMath(std::uintptr_t(Detour))), m_Original(0)
			{}

		inline void CreateHook(PointerMath Target) override
		{
			if (m_Created)
			{
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] already created!";
				return;
			}
			else if (Target.As<std::uintptr_t>() == 0)
			{
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] the target address is not valid!";
				return;
			}
			else if (m_Detour.As<std::uintptr_t>() == 0)
			{
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] the detour address is not valid!";
				return;
			}
			else if (m_Name.length() == 0)
			{
				LOG(WARNING) << "Hook at address [" << AddColorToStream(LogColor::MAGENTA)
					<< Target.As<std::uintptr_t>() << ResetStreamColor << "] doesn't have a valid name!";
				return;
			}

			m_Target = Target;

			void* Orig = nullptr;
			auto Res = MH_CreateHook(m_Target.As<void*>(), m_Detour.As<void*>(), &Orig);

			if (Res != MH_OK)
			{
				LOG(WARNING) << "Could not create hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name<< ResetStreamColor << "], error: " << MH_StatusToString(Res);
				return;
			}

			m_Original = PointerMath(std::uintptr_t(Orig));

			LOG(INFO) << "Created hook [" << AddColorToStream(LogColor::MAGENTA)
				<< m_Name << ResetStreamColor << "]";

			m_Created = true;
		}
		inline void DestroyHook() override
		{
			if ((!m_Enabled) && m_Created)
			{
				auto Res = MH_RemoveHook(m_Target.As<void*>());

				if (Res != MH_OK)
				{
					LOG(WARNING) << "Could not destroy hook [" << AddColorToStream(LogColor::MAGENTA)
						<< m_Name << ResetStreamColor << "], error: " << MH_StatusToString(Res);
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
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] hasn't been created!";
				return;
			}
			else if (m_Enabled)
			{
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] already enabled";
				return;
			}

			auto Res = MH_EnableHook(m_Target.As<void*>());

			if (Res != MH_OK)
			{
				LOG(WARNING) << "Could not enable hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "], error: " << MH_StatusToString(Res);
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
					LOG(WARNING) << "Could not disable hook " << AddColorToStream(LogColor::MAGENTA)
						<< m_Name << ResetStreamColor << " ,error: " << MH_StatusToString(Res);
					return;
				}

				LOG(INFO) << "Disabled hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "]";

				m_Enabled = false;
			}
		}

		template<typename T>
		inline std::enable_if_t<std::is_pointer<T>::value, T> Get()
		{
			if (!m_Created || !m_Enabled)
			{
				LOG(WARNING) << "Could not get original of hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "], the hook has not been enabled or created!";
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

		inline VMTHook(std::string_view Name, std::size_t NumFuncs) : Hook(Name, HookType::VMT),
			m_NumFuncs(NumFuncs), m_OriginalTable(nullptr), m_NewTable(nullptr)
		{}

		inline void CreateHook(PointerMath Target) override
		{
			if (m_Created)
			{
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] already created";
				return;
			}
			else if (Target.As<std::uintptr_t>() == 0)
			{
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] the target address is not valid!";
				return;
			}
			else if (m_NumFuncs == 0)
			{
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] the number of methods is not valid!";
				return;
			}
			else if (m_Name.size() == 0)
			{
				LOG(WARNING) << "Hook at address [" << AddColorToStream(LogColor::MAGENTA)
					<< Target.As<std::uintptr_t>() << ResetStreamColor << "] name is not valid!";
				return;
			}

			m_Target = Target;

			auto Object = m_Target.As<void***>();
			m_OriginalTable = *Object;

			m_NewTable = new void*[m_NumFuncs];

			std::copy_n(m_OriginalTable, m_NumFuncs, m_NewTable); /*copy the ptrs from the original table 
																  to our new table*/
			LOG(INFO) << "Created hook [" << AddColorToStream(LogColor::MAGENTA)
				<< m_Name << ResetStreamColor << "] with " << m_NumFuncs << " methods";

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
				LOG(WARNING) << "Could not setup hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "]::" << Name << ", the index is not valid!";
				return;
			}
			else if (!VFunc)
			{
				LOG(WARNING) << "Could not setup hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "]::" << Name << ", the function is not valid!";
				return;
			}
			else if (!m_Created)
			{
				LOG(WARNING) << "Could not setup hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "]::" << Name << " the hook has not been created!";
				return;
			}
			else if (m_Names.find(Index) != m_Names.end())
			{
				LOG(WARNING) << "Could not setup hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "]::" << Name << " the hook have already been setup up!";
				return;
			}

			m_NewTable[Index] = VFunc;
			m_Names.insert( { Index, Name } );

			LOG(INFO) << "Setup hook [" << AddColorToStream(LogColor::MAGENTA)
				<< m_Name << ResetStreamColor << "]::" << Name;
		}
		inline void UnSetupHook(std::size_t Index)
		{
			if ((Index >= m_NumFuncs) || (m_Names.find(Index) == m_Names.end()))
			{
				LOG(WARNING) << "Could not unsetup hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "]::" << " the index is not valid!";
				return;
			}
			else if (!m_Created)
			{
				LOG(WARNING) << "Could not setup hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "]::" << m_Names[Index] << " the hook has not been created!";
				return;
			}

			m_NewTable[Index] = m_OriginalTable[Index];

			auto It = m_Names.find(Index);
			m_Names.erase(It);

			LOG(WARNING) << "Setup hook [" << AddColorToStream(LogColor::MAGENTA)
				<< m_Name << ResetStreamColor << "]::" << m_Names[Index];
		}

		inline void EnableHook()
		{
			if (!m_Created)
			{
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] hasn't been created!";
				return;
			}
			else if (m_Enabled)
			{
				LOG(WARNING) << "Hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "] already enabled";
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
				LOG(WARNING) << "Could not get original of hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "]::" << ", the index is not valid!";
				return nullptr;
			}
			else if (!m_Enabled)
			{
				LOG(WARNING) << "Could not get original of hook [" << AddColorToStream(LogColor::MAGENTA)
					<< m_Name << ResetStreamColor << "]::" << m_Names[Index] << ", the hook has not been enabled!";
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
	
	class DHookWrapper {};
	class VHookWrapper {};

	template<typename T, typename U>
	class HookWrapper :  public PatternImpl<U>
	{
	public:

		template<typename = std::enable_if_t<std::is_same<T, DHookWrapper>::value>>
		inline HookWrapper(std::string_view Pattern, std::string_view Name, void* Detour) : m_Hook(std::make_shared<DHook>(Name, Detour)),
			PatternImpl<U>(Pattern, Name)
		{
			this->OnFinalPtr = [&](PointerMath& Ptr)
			{
				this->m_Hook->CreateHook(Ptr);
				g_ComponentMgr->GetComponent<Hooking>("Hooking")->m_Hooks.push_back(this->m_Hook);
			};
		}

		template<typename = std::enable_if_t<std::is_same<T, VHookWrapper>::value>>
		inline HookWrapper(std::string_view Pattern, std::string_view Name, std::size_t NumFuncs) : m_VHook(std::make_shared<VMTHook>(Name, NumFuncs)),
			PattermImpl<U>(Name, Pattern)
		{
			this->OnFinalPtr = [&](PointerMath& Ptr)
			{
				this->m_VHook->CreateHook(Ptr);
				this->m_VHook->EnableHook();
			};
		}

		template<typename = std::enable_if_t<std::is_same<T, DHookWrapper>::value>>
		inline U Get()
		{
			return m_Hook->Get<U>();
		}

		template<typename = std::enable_if_t<std::is_same<T, VHookWrapper>::value>>
		inline U Get(std::size_t Index)
		{
			return m_VHook->Get<U>(Index);
		}

		template<typename = std::enable_if_t<std::is_same<T,VHookWrapper>::value>>
		inline void SetupHook(std::size_t Index, void * VFunc, std::string_view Name)
		{
			m_VHook->SetupHook(Index, VFunc, Name);
		}

		template<typename = std::enable_if_t<std::is_same<T, VHookWrapper>::value>>
		inline void UnSetupHook(std::size_t Index)
		{
			m_VHook->UnSetupHook(Index);
		}

	private:

		std::shared_ptr<DHook> m_Hook;	
		std::shared_ptr<VMTHook> m_VHook;
	};
}