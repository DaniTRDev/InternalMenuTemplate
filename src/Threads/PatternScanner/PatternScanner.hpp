#pragma once

namespace change_me
{
	class PatternScanner : public Singleton<PatternScanner>
	{
	public:

		PatternScanner();

		using OnModuleCB_t = void();
		using OnFinishedScanCB_t = void();
		using OnScannedPatternCB_t = void(PointerMath& Ptr);

		using PatternVec_t = std::vector<std::pair<OnScannedPatternCB_t*, Pattern>>;

		bool Initialize();

		void OnModule(std::shared_ptr<Module> Module, OnModuleCB_t* Callback, OnFinishedScanCB_t* OnFinishedScanCB = 0);
		void Add(Pattern Patt, OnScannedPatternCB_t* CB = 0);

		bool ArePatternsScanned();

	private:

		void Scan();
		void ScanPatternsForModule(PatternVec_t& Patterns);

	private:

		PatternVec_t m_Patterns;
		std::shared_ptr<Module> m_CurrentModule;

		std::size_t m_TotalPatterns; /*this will have the count of every m_Patterns and it won't be nulled*/
	};
}
