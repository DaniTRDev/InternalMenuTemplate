#pragma once

/*don't add this one to Common.hpp because it will be constantly modified*/
namespace change_me
{
	class Pointers : public ComponentBase
	{
	public:

		Pointers();

		bool Initialize() override;
		bool Run() override;
		bool Uninitialize() override;

		PattermImpl<void*>* m_Test1;

	private:

	};
}
