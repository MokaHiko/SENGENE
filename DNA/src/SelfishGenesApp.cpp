#include "SGE/SGE.h"

#include "EditorLayer.h"
#include "SGE/Core/EntryPoint.h"

class SelfishGene : public SGE::Application
{
public:
	SelfishGene()
	{
        PushLayer(new EditorLayer());
	}

	~SelfishGene() {}
};

SGE::Application* SGE::CreateApplication()
{
	return new SelfishGene();
}