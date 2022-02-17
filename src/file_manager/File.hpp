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
			: m_FileManager(fileManager), m_FilePath(fileManager->GetBaseDir() / filePath), m_IsProjectFile(true)
		{

		}

	public:
		File(std::filesystem::path filePath)
			: m_FilePath(filePath)
		{

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