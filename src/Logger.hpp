#pragma once

namespace change_me
{
	class Logger;
	enum class LogColor
	{
		RESET,
		WHITE = 97,
		CYAN = 36,
		MAGENTA = 35,
		BLUE = 34,
		GREEN = 32,
		YELLOW = 33,
		RED = 31,
		BLACK = 30
	};

#define ADD_COLOR_TO_TEXT(color, Text) "\x1b[" << int(color) << "m" << Text << "\x1b[" << int(LogColor::RESET) << "m"

	class Logger : public Singleton<Logger>
	{

	public:
		Logger()
			: m_ConsoleTitle("NON-SET"), m_File(nullptr), m_AttachConsole(false),
			  m_DidConsoleExist(false), m_Worker(g3::LogWorker::createLogWorker()), m_ConsoleHandle(NULL),
			  m_OriginalConsoleMode(0)
		{}

		void Initialize(std::string_view ConsoleTitle, File file, bool AttachConsole = true)
		{
			m_ConsoleTitle = ConsoleTitle;
			m_File = new File(file.GetPath());
			m_AttachConsole = AttachConsole;

			if (m_AttachConsole)
			{
				if (m_DidConsoleExist = ::AttachConsole(GetCurrentProcessId()), !m_DidConsoleExist)
					AllocConsole();

				if (m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE), m_ConsoleHandle != nullptr)
				{
					SetConsoleTitleA(m_ConsoleTitle.data());
					SetConsoleOutputCP(CP_UTF8);

					DWORD ConsoleMode;
					GetConsoleMode(m_ConsoleHandle, &ConsoleMode);
					m_OriginalConsoleMode = ConsoleMode;

					// terminal like behaviour enable full color support
					ConsoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
					// prevent clicking in terminal from suspending our main thread
					ConsoleMode &= ~(ENABLE_QUICK_EDIT_MODE);

					SetConsoleMode(m_ConsoleHandle, ConsoleMode);
				}
			}
			OpenOutStreams();
			InitializeG3Log();
		}
		void Uninitialize()
		{
			UninitializeG3Log();
			CloseOutStreams();

			if (m_DidConsoleExist)
				SetConsoleMode(m_ConsoleHandle, m_OriginalConsoleMode);	

			if(m_AttachConsole)
				FreeConsole();
		}

	private:

		void InitializeG3Log()
		{
			m_Worker->addSink(std::make_unique<LogSink>(), &LogSink::Callback);
			g3::initializeLogging(m_Worker.get());
		}
		void UninitializeG3Log()
		{
			m_Worker->removeAllSinks();
			m_Worker.reset();
		}

		void OpenOutStreams()
		{
			if (m_AttachConsole)
				m_ConsoleOut.open("CONOUT$", std::ios_base::out |std::ios_base::app);

			m_FileOut.open(m_File->GetPath(), std::ios_base::out | std::ios_base::trunc);
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
				if (Logger::Get()->m_ConsoleOut.is_open())
					Logger::Get()->m_ConsoleOut << log.get().toString(LogSink::FormatConsole) << std::flush;

				Logger::Get()->m_FileOut << log.get().toString(LogSink::FormatFile) << std::flush;

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
					<< ADD_COLOR_TO_TEXT(color, "[" << std::setw(7) << msg.level() << "/"
					<< msg.file() << ":" << msg.line() << "]") 
					<< ":";


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
					<< ":";

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

		File * m_File;

		std::unique_ptr<g3::LogWorker> m_Worker;
	};
}