#pragma once
#include "common.hpp"
#include "FileManager.hpp"

namespace name
{
	class FileManager;

	class File
	{
		friend FileManager;
		FileManager* m_FileManager{};

		bool m_IsProjectFile = false;

		std::filesystem::path m_FilePath;
	protected:
		File(FileManager* fileManager, std::filesystem::path filePath)
			: m_FileManager(fileManager), m_IsProjectFile(true)
		{
			File(fileManager->GetBaseDir() / filePath);
		}

	public:
		File(std::filesystem::path filePath)
			: m_FilePath(filePath)
		{
			FileManager::EnsureFileCanBeCreated(m_FilePath);
		}

		virtual ~File()
		{
			
		}

		const std::filesystem::path GetPath() const
		{
			return m_FilePath;
		}

	private:

	};

}