#pragma once
#include "common.hpp"
#include "FileManager.hpp"
#include <g3log/g3log.hpp>
#include <g3log/loglevels.hpp>
#include <g3log/logworker.hpp>

namespace change_me
{
	class Logger;
	inline std::shared_ptr<Logger> g_log;

	enum class LogColor
	{
		RESET,
		WHITE = 97,
		BLUE = 34,
		GREEN = 32,
		YELLOW = 33,
		RED = 31
	};

	class Logger final
	{

	public:
		Logger(std::string_view ConsoleTitle, File File, bool AttachConsole = true)
			: m_ConsoleTitle(ConsoleTitle), m_File(File), m_AttachConsole(AttachConsole),
			  m_DidConsoleExist(false), m_Worker(g3::LogWorker::createLogWorker()), m_ConsoleHandle(NULL),
			  m_OriginalConsoleMode(0)
		{}
		~Logger()
			{}

		static std::shared_ptr<Logger> GetInstance(std::string_view ConsoleTitle, File File, bool AttachConsole = true)
		{
			static auto Ptr = std::make_shared<Logger>(ConsoleTitle, File, AttachConsole);
			return Ptr;
		}
		static std::shared_ptr<Logger> GetInstance()
		{
			if (!g_log)
				throw std::runtime_error("Error, before calling this you must initialize logger!");

			return g_log;
		}

		void Initialize()
		{
			if (m_AttachConsole)
			{
				if (m_DidConsoleExist = ::AttachConsole(GetCurrentProcessId()), !m_DidConsoleExist)
					AllocConsole();

				if (m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE), m_ConsoleHandle != nullptr)
				{
					SetConsoleTitleA(m_ConsoleTitle.data());
					SetConsoleOutputCP(CP_UTF8);

					DWORD console_mode;
					GetConsoleMode(m_ConsoleHandle, &console_mode);
					m_OriginalConsoleMode = console_mode;

					// terminal like behaviour enable full color support
					console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
					// prevent clicking in terminal from suspending our main thread
					console_mode &= ~(ENABLE_QUICK_EDIT_MODE);

					SetConsoleMode(m_ConsoleHandle, console_mode);
				}
			}
			OpenOutStreams();
			InitializeG3log();
		}
		void Uninitialize()
		{
			if (m_DidConsoleExist)
				SetConsoleMode(m_ConsoleHandle, m_OriginalConsoleMode);	
			else 
				FreeConsole();

			UninitializeG3Log();
			CloseOutStreams();
		}

	private:

		void InitializeG3log()
		{
			m_Worker->addSink(std::make_unique<LogSink>(), &LogSink::Callback);
			g3::initializeLogging(m_Worker.get());
		}
		void UninitializeG3Log()
		{
			m_Worker.reset();
		}

		void OpenOutStreams()
		{
			if (m_AttachConsole)
				m_ConsoleOut.open("CONOUT$", std::ios_base::out | std::ios_base::app);

			m_ConsoleOut << m_File.GetPath() << std::endl;

			m_FileOut.open(m_File.GetPath(), std::ios_base::out | std::ios_base::trunc);
		}
		void CloseOutStreams()
		{
			if (m_AttachConsole)
				m_ConsoleOut.close();

			m_FileOut.close();
		}

	private:

		struct LogSink
		{
			void Callback(g3::LogMessageMover log)
			{
				if (Logger::GetInstance()->m_AttachConsole)
					Logger::GetInstance()->m_ConsoleOut << log.get().toString(LogSink::FormatConsole) << std::flush;

				Logger::GetInstance()->m_FileOut << log.get().toString(LogSink::FormatFile) << std::flush;
			}

			static LogColor GetColor(const LEVELS level)
			{
				switch (level.value)
				{
				case g3::kDebugValue:
					return LogColor::BLUE;
				case g3::kInfoValue:
					return LogColor::GREEN;
				case g3::kWarningValue:
					return LogColor::YELLOW;
				}
				return g3::internal::wasFatal(level) ? LogColor::RED : LogColor::WHITE;
			}

			static std::string FormatConsole(const g3::LogMessage& msg)
			{
				LogColor color = LogSink::GetColor(msg._level);
				std::stringstream out;

				out
					<< "[" << msg.timestamp("%H:%M:%S") << "]"
					<< "\x1b[" << (int)color << "m"
					<< "[" << std::setw(7) << msg.level() << "/"
					<< msg.file() << ":" << msg.line() << "]"
					<< "\x1b[" << (int)LogColor::RESET << "m"
					<< ": ";

				return out.str();
			}
			static std::string FormatFile(const g3::LogMessage& msg)
			{
				LogColor color = LogSink::GetColor(msg._level);
				std::stringstream out;

				out
					<< "[" << msg.timestamp("%H:%M:%S") << "]"
					<< "[" << std::setw(7) << msg.level() << "/"
					<< msg.file() << ":" << msg.line() << "]"
					<< ": ";

				return out.str();
			}
		};

	private:

		friend struct LogSink;

		bool m_AttachConsole;
		bool m_DidConsoleExist;

		std::string_view m_ConsoleTitle; 
		DWORD m_OriginalConsoleMode;
		HANDLE m_ConsoleHandle;

		std::ofstream m_ConsoleOut;
		std::ofstream m_FileOut;

		File m_File;

		std::unique_ptr<g3::LogWorker> m_Worker;

	};
}