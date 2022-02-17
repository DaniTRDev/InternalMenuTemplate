#pragma once

namespace change_me
{
	class File;
	class FileManager;

	class Folder
	{
		friend class FileManager;
		FileManager* m_FileManager{};

		bool m_IsProjectFile = false;

		std::filesystem::path m_FolderPath;

	protected:
		Folder(FileManager* fileManager, std::filesystem::path filePath);

	public:
		Folder(std::filesystem::path folderPath);
		virtual ~Folder() = default;

		File GetFile(std::filesystem::path filePath) const;
		const std::filesystem::path GetPath() const;

	private:

	};

}