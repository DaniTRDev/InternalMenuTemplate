#pragma once
#include "common.hpp"
#include "file_manager/File.hpp"
#include "file_manager/Folder.hpp"

namespace change_me
{
	class FileManager;
	inline FileManager* g_file_manager{};

	class FileManager final
	{
		std::filesystem::path m_BaseDir;

	public:
		FileManager(std::filesystem::path baseDir)
			: m_BaseDir(baseDir)
		{
			FileManager::EnsureFolderExists(m_BaseDir);

			g_file_manager = this;
		}

		virtual ~FileManager()
		{
			g_file_manager = nullptr;
		}

		const std::filesystem::path GetBaseDir() const
		{
			return m_BaseDir;
		}

		File GetProjectFile(std::filesystem::path filePath)
		{
			return File(
				this,
				filePath
			);
		}

		Folder GetProjectFolder(std::filesystem::path folderPath)
		{
			return Folder(
				this,
				folderPath
			);
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

	};
}