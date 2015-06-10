#pragma once

#include "Reflex.h"

namespace scone
{
	namespace cs
	{
		class CS_API DofReflex : public Reflex
		{
		public:
			DofReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Actuator& target, sim::ChannelSensor& source );
			virtual ~DofReflex();

			virtual void ComputeControls( double timestamp );

			// Reflex parameters
			Real target_pos;
			Real target_vel;
			Real pos_gain;
			Real vel_gain;

		private:
			sim::SensorDelayAdapter& m_DelayedRoot; // used for world coordinates, TODO: neater
			bool m_bUseRoot;
		};
	}
}
