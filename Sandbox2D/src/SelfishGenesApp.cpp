#include "SGE/SGE.h"

#include "Sandbox2D.h"
#include "SGE/Core/EntryPoint.h"

class SelfishGene : public SGE::Application
{
public:
	SelfishGene()
	{
        PushLayer(new Sandbox2D());
	}

	~SelfishGene() {}
};

SGE::Application* SGE::CreateApplication()
{
	return new SelfishGene();
}