
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

			return std::make_shared<T>();
		}
		template<typename T>
		inline std::shared_ptr<T> GetComponent(std::size_t Index)
		{
			if (Index >= m_Components.size())
				return std::make_shared<T>();

			return std::static_pointer_cast<T>(m_Components[Index]);
		}

	public:

		void InitializeComponents();
		void RunComponents();
		void UninitializeComponents();

	private:

		std::vector<std::shared_ptr<ComponentBase>> m_Components;

	}; extern std::shared_ptr<ComponentManager> g_ComponentMgr; 
}