#include "Common.hpp"

namespace change_me
{
	Folder::Folder(std::filesystem::path FilePath)
		: m_FolderPath(FileManager::EnsureFolderExists(FileManager::Get().GetBaseDir() / FilePath))
	{
		m_IsProjectFile = true;
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