#include "SGE/SGE.h"

#include "Sandbox3D.h"
#include "FeatureTest3D.h"
#include "SGE/Core/EntryPoint.h"

class SelfishGene : public SGE::Application
{
public:
	SelfishGene()
	{
        PushLayer(new FeatureTest3D());
	}

	~SelfishGene() {}
};

SGE::Application* SGE::CreateApplication()
{
	return new SelfishGene();
}