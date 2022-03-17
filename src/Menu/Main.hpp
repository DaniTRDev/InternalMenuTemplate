#pragma once

namespace change_me
{

	inline void MainMenu(UIManager * Ui)
	{
		Ui->AddSub("Main", [](SubMenu* Sub)
			{
				static bool Asd = false;
				Sub->AddOption("MyOpt", [](OptionParams* Params)
					{
						Params->AddWidget<BoolWidget>(&Asd);
					});
				static float ASd1 = 1.f;
				Sub->AddOption("MyOpt2", [](OptionParams* Params)
					{
						Params->AddWidget<NumberWidget<float>>(&ASd1, 100.f, 0.f, 1.f, 5);
					});
				Sub->AddOption("MyOpt3", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt4", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt5", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt6", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt7", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt8", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt9", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt10", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt11", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt12", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt13", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt14", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt15", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt16", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt17", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt18", [](OptionParams* Params)
					{
					});
				Sub->AddOption("MyOpt19", [](OptionParams* Params)
					{
					});
			});
	}
}