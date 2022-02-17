#include "common.hpp"
#include "File.hpp"
#include "FileManager.hpp"

namespace change_me
{
	File::File(std::shared_ptr<FileManager> FileManager, std::filesystem::path FilePath)
		: File(FileManager->GetBaseDir() / FilePath)
	{
		m_IsProjectFile = true;
	}

	File::File(std::filesystem::path FilePath)
		: m_FilePath(FileManager::EnsureFileCanBeCreated(FilePath)), m_IsProjectFile(false)
	{

	}

	const std::filesystem::path File::GetPath() const
	{
		return m_FilePath;
	}
}