#include "DebugConsolePanel.h"

namespace SGE {
	DebugConsolePanel::DebugConsolePanel()
	{

	}

	DebugConsolePanel::~DebugConsolePanel()
	{

	}

	void DebugConsolePanel::OnImGuiRender()
	{
		static bool isOpen = true;
		ShowExampleAppConsole(&isOpen);
	}

    void DebugConsolePanel::SetContext(const Ref<SGE::Scene> scene)
	{

	}
}