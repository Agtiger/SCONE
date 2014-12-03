#pragma once

#include "simbody.h"
#include "..\Joint.h"

namespace scone
{
	namespace sim
	{
		class SIM_SIMBODY_API Joint_Simbody : public Joint
		{
		public:
			Joint_Simbody() { };
			virtual ~Joint_Simbody() { };
			
			virtual class Body& GetParentLink() override;
			virtual class Body& GetChildLink() override;

			virtual class Model& GetModel() override;
		
		};
	}
}
