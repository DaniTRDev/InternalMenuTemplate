#pragma once
#include "common.hpp"
#include "File.hpp"
#include "FileManager.hpp"

namespace name
{
	class FileManager;

	class Folder
	{
		friend FileManager;
		FileManager* m_FileManager{};

		bool m_IsProjectFile = false;

		std::filesystem::path m_FolderPath;

	protected:
		Folder(FileManager* fileManager, std::filesystem::path filePath)
			: m_FileManager(fileManager), m_IsProjectFile(true)
		{
			Folder(fileManager->GetBaseDir() / filePath);
		}

	public:
		Folder(std::filesystem::path folderPath)
			: m_FolderPath(folderPath)
		{
			FileManager::EnsureFolderExists(m_FolderPath);
		}

		virtual ~Folder()
		{

		}

		File GetFile(std::filesystem::path filePath) const
		{
			if (filePath.is_absolute())
				throw std::exception("Folder#GetFile requires a relative path.");

			return File(
				m_FolderPath / filePath
			);
		}

		const std::filesystem::path GetPath() const
		{
			return m_FolderPath;
		}

	private:

	};

}