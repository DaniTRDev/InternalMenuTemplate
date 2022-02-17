#pragma once
#include "common.hpp"
#include "file_manager/File.hpp"
#include "file_manager/Folder.hpp"

namespace change_me
{
	class FileManager;
	inline std::shared_ptr<FileManager> g_file_manager;

	class FileManager
	{
	public:

		FileManager(std::filesystem::path BaseDir)
			: m_BaseDir(BaseDir)
		{
			FileManager::EnsureFolderExists(m_BaseDir);
		}
		virtual ~FileManager()
		{
		}

		static std::shared_ptr<FileManager> GetInstance(std::filesystem::path BaseDir)
	    {
			static auto Ptr = std::make_shared<FileManager>(BaseDir); /*make sure the Ptr is only craeted once*/
			return Ptr;
		}
		static std::shared_ptr<FileManager> GetInstance()
		{
			if (!g_file_manager)
				throw std::exception("Error, you must initialize FileManager first!");

			return g_file_manager;
		}

		std::filesystem::path GetBaseDir()
		{
			return m_BaseDir;
		}

		File GetProjectFile(std::filesystem::path FilePath)
		{
			return File(GetInstance(), FilePath);
		}
		Folder GetProjectFolder(std::filesystem::path FolderPath)
		{
			return Folder(GetInstance(), FolderPath);
		}

		static std::filesystem::path EnsureFileCanBeCreated(const std::filesystem::path filePath)
		{
			FileManager::EnsureFolderExists(filePath.parent_path());

			return filePath;
		}
		static std::filesystem::path EnsureFolderExists(const std::filesystem::path folderPath)
		{
			bool create_path = !std::filesystem::exists(folderPath);

			if (!std::filesystem::is_directory(folderPath))
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