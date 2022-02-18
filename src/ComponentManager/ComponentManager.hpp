
namespace change_me
{
	class ComponentManager
	{
	public:

		static std::shared_ptr<ComponentManager> GetInstance();

		void AddComponent(std::shared_ptr<ComponentBase> Component);
		void RemoveComponent(std::string_view Name);
		void RemoveComponent(std::size_t Index);

		template<typename T>
		inline std::shared_ptr<T> GetComponent(const char* Name)
		{
			for (auto& Comp : m_Components)
			{
				if (Comp->GetName() == Name)
					return std::static_pointer_cast<T>(Comp);
			}

			/*just a simple log before the crash, so we can know what we did wrong*/
			return std::make_shared<T>();
		}
		template<typename T>
		inline std::shared_ptr<T> GetComponent(std::size_t Index)
		{
			if (Index >= m_Components.size())
			{/*just a simple log before the crash, so we can know what we did wrong*/
				LOG(WARNING) << "The component at index" << Index << " has not been created!";
				return std::make_shared<T>();
			}

			return std::static_pointer_cast<T>(m_Components[Index]);
		}

	public:

		void InitializeComponents();
		void RunComponents();
		void UninitializeComponents();

	private:

		std::vector<std::shared_ptr<ComponentBase>> m_Components;

	}; extern std::shared_ptr<ComponentManager> g_ComponentMgr; /*WE NEED TO DECLARE THIS AS A RAW PTR */
}