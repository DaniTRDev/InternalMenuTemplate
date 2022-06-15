#include "Common.hpp"

namespace change_me
{
	Pattern::Pattern(std::string_view PatternStr, std::string_view Name) :
		m_Name(Name), m_PatternStr(PatternStr), m_Scanned(false)
	{
		if (PatternStr.length() == 0)
		{
			LOG(FATAL) << "The pattern " << Name << "has not a valid signature";
			return;
		}
		TransforPatternStrToBytes();
	}
	Pattern::Pattern()
	{}

	std::string_view Pattern::GetName()
	{
		return m_Name;
	}
	std::vector<std::uint8_t>& Pattern::GetPatternBytes()
	{
		return m_PatternBytes;
	}

	void Pattern::TransforPatternStrToBytes()
	{
		for (auto i = 0; i < m_PatternStr.length(); i++)
		{
			if (m_PatternStr[i] == ' ') /*skip spaces*/
				continue;

			if (m_PatternStr[i] == '?')
			{
				if (m_PatternStr[i + std::uint64_t(1)] == '?')
					i++;

				m_PatternBytes.push_back(0); /*we don't take care of wildcards*/

				continue;
			}

			char Bytes[] = { m_PatternStr[i], m_PatternStr[i + std::uint64_t(1)] }; /*store the 2bytes in a single char*/

			auto ConvertedBytes = strtoull(Bytes, nullptr, 16);

			m_PatternBytes.push_back(std::uint8_t(ConvertedBytes));

			i++;
		}
	}
}
