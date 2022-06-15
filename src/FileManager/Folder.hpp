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

	private:

		friend class FileManager;

		bool m_IsProjectFile;

		std::filesystem::path m_FolderPath;

	};

}