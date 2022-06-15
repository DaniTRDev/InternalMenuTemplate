#pragma once
#include "../Common.hpp"
#include "File.hpp"
#include "Folder.hpp"

namespace change_me
{
	class FileManager;

	class FileManager : public Singleton<FileManager>
	{
	public:

		FileManager() : m_BaseDir("")
		{
		}
		~FileManager()
		{
		}

		void Initialize(std::filesystem::path BaseDir)
		{
			m_BaseDir = BaseDir;
			FileManager::EnsureFolderExists(m_BaseDir);
		}

		const std::filesystem::path GetBaseDir()
		{
			return m_BaseDir;
		}

		File GetProjectFile(std::filesystem::path filePath)
		{
			if (filePath.is_absolute())
				throw std::exception("Project files are relative to the BaseDir, don't use absolute paths!");

			return File(filePath);
		}
		Folder GetProjectFolder(std::filesystem::path folderPath)
		{
			if (folderPath.is_absolute())
				throw std::exception("Project folders are relative to the BaseDir, don't use absolute paths!");

			return Folder(folderPath);
		}

		static std::filesystem::path EnsureFileCanBeCreated(const std::filesystem::path filePath)
		{
			FileManager::EnsureFolderExists(filePath.parent_path());

			return filePath;
		}
		static std::filesystem::path EnsureFolderExists(const std::filesystem::path folderPath)
		{
			bool create_path = !std::filesystem::exists(folderPath);

			if (!create_path && !std::filesystem::is_directory(folderPath))
			{
				std::filesystem::remove(folderPath);
				create_path = true;
			}
			if (create_path)
				std::filesystem::create_directory(folderPath);

			return folderPath;
		}

	private:
		std::filesystem::path m_BaseDir;
	};
}