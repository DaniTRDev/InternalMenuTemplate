#pragma once

namespace change_me
{
	class FileManager;

	class File
	{
		friend class FileManager;
		FileManager* m_FileManager{};

		bool m_IsProjectFile = false;

		std::filesystem::path m_FilePath;
	protected:
		File(FileManager* fileManager, std::filesystem::path filePath);

	public:
		File(std::filesystem::path filePath);
		virtual ~File() = default;

		const std::filesystem::path GetPath() const;

	private:

	};

}