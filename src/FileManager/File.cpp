#include "Common.hpp"

namespace change_me
{
	File::File(std::filesystem::path FilePath)
		: m_FilePath(FileManager::EnsureFileCanBeCreated(FileManager::Get().GetBaseDir() / FilePath)), 
		m_IsProjectFile(false), m_FileSize(0)
	{
		m_IsProjectFile = true;
	}

	const std::filesystem::path File::GetPath() const
	{
		return m_FilePath;
	}

	std::string File::GetFileName(bool Extension)
	{
		auto FileName = m_FilePath.filename().string();
		
		if (!Extension)
			FileName.erase(FileName.begin() + FileName.find_last_of("."), FileName.end());

		return FileName;
	}

	bool File::DoesFileExist()
	{
		struct stat buffer;
		return (stat(m_FilePath.string().c_str(), &buffer) == 0);
	}
}