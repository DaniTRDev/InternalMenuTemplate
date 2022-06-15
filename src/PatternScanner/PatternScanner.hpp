#pragma once

namespace change_me
{
	using OnModuleCB_t = std::function<void()>;
	using OnFinishedScanCB_t = std::function<void()>;
	using OnScannedPatternCB_t = std::function<void(PointerMath& Ptr)>;
	using PatternVec_t = std::vector<std::pair<OnScannedPatternCB_t, Pattern>>;

	struct ModuleScan
	{
		std::vector<std::pair<OnScannedPatternCB_t, Pattern>> m_Patterns;
		bool m_Scanned;
	};

	using ScanQueue_t = std::map<std::shared_ptr<Module>, ModuleScan>;


	class PatternScanner : public Singleton<PatternScanner>
	{
	public:

		PatternScanner();

		bool Initialize();

		void OnModule(std::shared_ptr<Module> Module, OnModuleCB_t Callback);
		void Add(Pattern Patt, OnScannedPatternCB_t CB = 0);

		void Scan(OnFinishedScanCB_t Callback = 0);
		void ScanPatternsForModule(std::shared_ptr<Module> Mod, PatternVec_t& Patterns);

		ScanQueue_t& GetScanQueue();

	private:

		ScanQueue_t m_ScanQueue;
		std::shared_ptr<Module> m_CurrentModule;
	
	};
}
