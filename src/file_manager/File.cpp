#include "common.hpp"
#include "File.hpp"
#include "FileManager.hpp"

namespace change_me
{
	File::File(FileManager* fileManager, std::filesystem::path filePath)
		: m_FileManager(fileManager), m_IsProjectFile(true)
	{
		File(fileManager->GetBaseDir() / filePath);
	}

	File::File(std::filesystem::path filePath)
		: m_FilePath(FileManager::EnsureFileCanBeCreated(filePath))
	{

	}

	const std::filesystem::path File::GetPath() const
	{
		return m_FilePath;
	}
}