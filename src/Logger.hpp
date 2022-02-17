#pragma once
#include "common.hpp"
#include "FileManager.hpp"

namespace change_me
{
	class Logger;
	inline Logger* g_log{};

	class Logger
	{
		bool m_AttachConsole;
		bool m_DidConsoleExist = false;
		HANDLE m_ConsoleHandle{};
		std::string_view m_ConsoleTitle;
		std::ofstream m_ConsoleOut;
		DWORD m_ConsoleMode;

		File m_FilePath;
		std::ofstream m_FileOut;

	public:
		Logger(std::string_view consoleTitle, File filePath, bool attachConsole = true)
			: m_ConsoleTitle(consoleTitle), m_FilePath(filePath), m_AttachConsole(attachConsole)
		{
			InitConsole(m_AttachConsole);
			OpenLogFile();

			g_log = this;
		}
		~Logger()
		{
			g_log = nullptr;

			DestroyConsole();
		}

		void DestroyConsole()
		{
			if (m_DidConsoleExist)
			{
				SetConsoleMode(m_ConsoleHandle, m_ConsoleMode);

				return;
			}
			FreeConsole();
		}

		void InitConsole(bool attachConsole)
		{
			if (m_AttachConsole && (m_DidConsoleExist = ::AttachConsole(GetCurrentProcessId())) == false)
				AllocConsole();

			if (m_AttachConsole && (m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE)) != nullptr)
			{
				SetConsoleTitleA(m_ConsoleTitle.data());
				SetConsoleOutputCP(CP_UTF8);

				DWORD console_mode;
				GetConsoleMode(m_ConsoleHandle, &console_mode);
				m_ConsoleMode = console_mode;

				// terminal like behaviour enable full color support
				console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
				// prevent clicking in terminal from suspending our main thread
				console_mode &= ~(ENABLE_QUICK_EDIT_MODE);

				SetConsoleMode(m_ConsoleHandle, console_mode);

				m_ConsoleOut.open("$CONOUT", std::ios_base::out | std::ios_base::app);
			}
		}

	private:
		void OpenLogFile()
		{
			
		}

	};
}