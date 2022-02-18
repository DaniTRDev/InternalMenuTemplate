#pragma once
#include "Common.hpp"
#include "FileManager/File.hpp"
#include "FileManager/Folder.hpp"

namespace change_me
{
	class FileManager;
	inline std::shared_ptr<FileManager> g_FileManager;

	class FileManager final
	{
	public:

		FileManager(std::filesystem::path baseDir)
			: m_BaseDir(baseDir)
		{
			FileManager::EnsureFolderExists(m_BaseDir);
		}
		virtual ~FileManager()
		{
		}

		static std::shared_ptr<FileManager> GetInstance(std::filesystem::path baseDir)
	    {
			static std::shared_ptr<FileManager> ptr = std::make_shared<FileManager>(baseDir);
			return ptr;
		}
		static std::shared_ptr<FileManager> GetInstance()
		{
			if (!g_FileManager)
				throw std::exception("Error, you must initialize FileManager first!");

			return g_FileManager;
		}

		const std::filesystem::path GetBaseDir()
		{
			return m_BaseDir;
		}

		File GetProjectFile(std::filesystem::path filePath)
		{
			if (filePath.is_absolute())
				throw std::exception("Project files are relative to the BaseDir, don't use absolute paths!");

			return File(GetInstance(), filePath);
		}
		Folder GetProjectFolder(std::filesystem::path folderPath)
		{
			if (folderPath.is_absolute())
				throw std::exception("Project folders are relative to the BaseDir, don't use absolute paths!");

			return Folder(GetInstance(), folderPath);
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