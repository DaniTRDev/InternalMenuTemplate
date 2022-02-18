#include "Common.hpp"
#include "File.hpp"
#include "FileManager.hpp"

namespace change_me
{
	Folder::Folder(std::shared_ptr<FileManager> FileManager, std::filesystem::path FilePath)
		: Folder(FileManager->GetBaseDir() / FilePath)
	{
		m_FileManager = FileManager;
		m_IsProjectFile = true;
	}

	Folder::Folder(std::filesystem::path FolderPath)
		: m_FolderPath(FileManager::EnsureFolderExists(FolderPath))
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