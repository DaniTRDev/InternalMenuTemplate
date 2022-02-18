#pragma once

namespace change_me
{
	class File;
	class FileManager;

	class Folder
	{

	public:
		Folder(std::filesystem::path FolderPath);

		File GetFile(std::filesystem::path FilePath) const;
		const std::filesystem::path GetPath() const;

	protected:
		Folder(std::shared_ptr<FileManager> FileManager, std::filesystem::path FilePath);

	private:

		friend class FileManager;
		std::shared_ptr<FileManager> m_FileManager;

		bool m_IsProjectFile;

		std::filesystem::path m_FolderPath;

	};

}