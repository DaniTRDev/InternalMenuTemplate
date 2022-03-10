#pragma once

namespace change_me
{
	class FileManager;

	class File
	{
	public:

		File(std::filesystem::path FilePath);

		const std::filesystem::path GetPath() const;

		bool DoesFileExist();

	protected:
		File(std::shared_ptr<FileManager> FileManager, std::filesystem::path FilePath);

	private:

		friend class FileManager;

		bool m_IsProjectFile;

		std::filesystem::path m_FilePath;

	};

}