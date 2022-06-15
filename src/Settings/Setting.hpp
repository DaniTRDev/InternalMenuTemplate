#pragma once

namespace change_me
{

	template<class T, T DefaultValue>
	class Setting
	{
	public:

		inline Setting(std::string_view Name) : m_Name(Name)
		{
			if (!Name.length())
				throw std::runtime_error("Error, Setting name cannot be NULL!");

			m_Val = DefaultValue;
		}

		inline T* GetPtr()
		{
			return &m_Val;
		}
		inline const T& Get()
		{
			return m_Val;
		}

		inline void Save(nlohmann::json& Node)
		{
			Node[m_Name.data()] = m_Val;
		}
		inline void Load(nlohmann::json& Node)
		{
			if (Node[m_Name.data()].is_null())
				return; 

			m_Val = Node[m_Name.data()].get<T>();
		}

		inline T operator =(T Value)
		{
			m_Val = Value;

			return m_Val;
		}

		inline bool operator==(T Value)
		{
			return m_Val == Value;
		}
		inline bool operator!=(T Value)
		{
			return m_Val != Value;
		}

		inline T operator*(T Value)
		{
			return m_Val * Value;
		}
		inline T operator*=(T Value)
		{
			m_Val *= Value;
			return m_Val;
		}

		inline T operator/(T Value)
		{
			return m_Val / Value;
		}
		inline T operator/=(T Value)
		{
			m_Val /= Value;
			return m_Val;
		}

		inline T operator+(T Value)
		{
			return m_Val + Value;
		}
		inline T operator+=(T Value)
		{
			m_Val += Value;
			return m_Val;
		}

		inline T operator-(T Value)
		{
			return m_Val - Value;
		}
		inline T operator-=(T Value)
		{
			m_Val -= Value;
			return m_Val;
		}

		inline char* Name()
		{
			return const_cast<char*>(m_Name.data());
		}

	private:

		T m_Val;
		std::string_view m_Name;
	};

	template<class T>
	class SettingArray : public std::vector<T>
	{
	public:

		inline SettingArray(std::string_view Name) : m_Name(Name), std::vector<T>()
		{
			if (!Name.length())
				throw std::runtime_error("Error, Setting name cannot be NULL!");
		}

		inline T* GetPtr()
		{
			return this->data();
		}
		inline const T& Get(std::size_t Index)
		{
			return this->data[Index];
		} /*need checks*/

		inline void Save(nlohmann::json& Node)
		{
			for (std::size_t i = 0; i < this->size(); i++)
				Node[m_Name.data()].push_back(this->operator[](i)); /*ensure nlohmann's cast properly*/
			
		}
		inline void Load(nlohmann::json& Node)
		{
			if (Node[m_Name.data()].is_null())
				return;

			for (std::size_t i = 0; i < Node[m_Name.data()].size(); i++)
			{
				if (Node[m_Name.data()][i].is_null()) /*there's no data to load, avoid it to not get crashed*/
					continue;

				this->push_back(Node[m_Name.data()][i]); /*ensure nlohmann's cast properly*/
			}
		}

	private:

		std::string_view m_Name;
	};

}

