#pragma once

namespace change_me
{
	template<typename T>
	class Singleton
	{

	public:

		inline static std::shared_ptr<T> Get()
		{
			if (!m_Data)
				m_Data = std::make_shared<T>();

			return m_Data;
		}

	private:

		static inline std::shared_ptr<T> m_Data;
	};

}