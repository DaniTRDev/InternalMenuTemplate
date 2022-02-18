
namespace change_me
{
	class ComponentManager
	{
	public:

		static std::shared_ptr<ComponentManager> GetInstance();

		void AddComponent(std::shared_ptr<ComponentBase> Component);
		void RemoveComponent(std::string_view Name);
		void RemoveComponent(std::size_t Index);

		std::shared_ptr<ComponentBase> GetComponent(std::string_view Name);
		std::shared_ptr<ComponentBase> GetComponent(std::size_t Index);


		std::shared_ptr<ComponentBase> operator[](std::string_view Name);
		std::shared_ptr<ComponentBase> operator[](std::size_t Index);

	public:

		void InitializeComponents();
		void RunComponents();
		void UninitializeComponents();

	private:

		std::vector<std::shared_ptr<ComponentBase>> m_Components;

	}; extern std::shared_ptr<ComponentManager> g_ComponentMgr;
}