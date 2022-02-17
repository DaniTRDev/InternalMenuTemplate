#include "common.hpp"
#include "File.hpp"
#include "FileManager.hpp"

namespace change_me
{
	Folder::Folder(FileManager* fileManager, std::filesystem::path filePath)
		: Folder(fileManager->GetBaseDir() / filePath)
	{
		m_FileManager = fileManager;
		m_IsProjectFile = true;
	}

	Folder::Folder(std::filesystem::path folderPath)
		: m_FolderPath(FileManager::EnsureFolderExists(folderPath))
	{

	}

	File Folder::GetFile(std::filesystem::path filePath) const
	{
		if (filePath.is_absolute())
			throw std::exception("Folder#GetFile requires a relative path.");

		return File(
			m_FolderPath / filePath
		);
	}

	const std::filesystem::path Folder::GetPath() const
	{
		return m_FolderPath;
	}
}