#pragma once
#include "common.hpp"
#include "FileManager.hpp"
#include <g3log/g3log.hpp>
#include <g3log/loglevels.hpp>
#include <g3log/logworker.hpp>

namespace change_me
{
	class Logger;
	inline Logger* g_log{};

	class Logger
	{
		bool m_AttachConsole;
		bool m_DidConsoleExist;
		HANDLE m_ConsoleHandle{};
		std::string_view m_ConsoleTitle;
		std::ofstream m_ConsoleOut;
		DWORD m_ConsoleMode;

		File m_File;
		std::ofstream m_FileOut;

		std::unique_ptr<g3::LogWorker> m_Worker;

	public:
		Logger(std::string_view consoleTitle, File file, bool attachConsole = true)
			: m_ConsoleTitle(consoleTitle), m_File(file), m_AttachConsole(attachConsole),
			  m_DidConsoleExist(false), m_Worker(g3::LogWorker::createLogWorker())
		{
			InitConsole(m_AttachConsole);
			OpenLogFile();

			m_Worker->addSink(std::make_unique<LogSink>(), &LogSink::Callback);
			g3::initializeLogging(m_Worker.get());

			g_log = this;
		}

		~Logger()
		{
			g_log = nullptr;

			m_Worker.reset();
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

				m_ConsoleOut.open("CONOUT$", std::ios_base::out | std::ios_base::app);
			}
		}

	private:
		void OpenLogFile()
		{
			m_FileOut.open(m_File.GetPath(), std::ios_base::out | std::ios_base::trunc);				
		}

		struct LogSink
		{
			enum class Color
			{
				RESET,
				WHITE = 97,
				BLUE = 34,
				GREEN = 32,
				YELLOW = 33,
				RED = 31
			};

			static Color GetColor(const LEVELS level)
			{
				switch (level.value)
				{
				case g3::kDebugValue:
					return Color::BLUE;
				case g3::kInfoValue:
					return Color::GREEN;
				case g3::kWarningValue:
					return Color::YELLOW;
				}
				return g3::internal::wasFatal(level) ? Color::RED : Color::WHITE;
			}

			void Callback(g3::LogMessageMover log)
			{
				g3::LogMessage msg = log.get();

				g_log->m_ConsoleOut << msg.toString(LogSink::FormatConsole) << std::flush;
			}

			static std::string FormatConsole(const g3::LogMessage& msg)
			{
				Color color = LogSink::GetColor(msg._level);
				std::stringstream out;

				out
					<< "[" << msg.timestamp("%H:%M:%S") << "]"
					<< "\x1b[" << (int)color << "m"
					<< "[" << std::setw(7) << msg.level() << "/"
					<< msg.file() << ":" << msg.line() << "]"
					<< "\x1b[" << (int)Color::RESET << "m"
					<< ": ";

				return out.str();
			}

			static std::string FormatFile(const g3::LogMessage& msg)
			{

			}
		};
	};
}