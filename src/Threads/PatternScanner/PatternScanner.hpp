#pragma once

namespace change_me
{
	class PatternScanner : public ThreadPoolBase
	{
	public:

		PatternScanner();

		using OnScannedPatternCB_t = std::function<void(PointerMath& Ptr)>;

		bool Initialize();
		bool Run();

		void UnitializeThread() override;

		void OnModule(std::shared_ptr<Module> Module, std::function<void()> Callback);
		void Add(Pattern Patt, OnScannedPatternCB_t CB = 0);

		void DoScan(); /*sets m_DoScan*/
		bool AreCriticalPatternsScanned();

	private:

		void Scan(); /*the real scan function*/
		bool ScanPattern(std::pair<OnScannedPatternCB_t, Pattern> Patt, std::shared_ptr<Module> Mod);

		std::map<std::shared_ptr<Module>, std::vector<std::pair<OnScannedPatternCB_t, Pattern>>> m_Patterns;
		std::shared_ptr<Module> m_CurrentModule;

		bool m_AreCriticalPatternsScanned; /*for the patterns in Pointers::Initialize, this will tell the hooking when to start*/
		bool m_DoScan;

	}; 
	extern std::shared_ptr<PatternScanner> g_PatternScanner;

}
