#pragma once

namespace change_me
{
	class FileManager;

	class File
	{
	public:

		File(std::filesystem::path FilePath);

		const std::filesystem::path GetPath() const;
		std::string GetFileName(bool Extension = false);

		bool DoesFileExist();

	private:

		friend class FileManager;

		bool m_IsProjectFile;
		std::filesystem::path m_FilePath;

		std::fstream m_FileStream;

		std::size_t m_FileSize;

	};

}