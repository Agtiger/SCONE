#include "stdafx.h"

#include "MuscleReflex.h"
#include "../sim/Muscle.h"
#include "../sim/Area.h"
#include "Tools.h"
#include "../sim/Dof.h"

//#define DEBUG_MUSCLE "vasti_r"

namespace scone
{
	namespace cs
	{
		MuscleReflex::MuscleReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Reflex( props, par, model, area ),
		m_pForceSensor( nullptr ),
		m_pLengthSensor( nullptr ),
		m_pVelocitySensor( nullptr ),
		m_pConditionalDofPos( nullptr ),
		m_pConditionalDofVel( nullptr )
		{
			sim::Muscle& source = *FindByName( model.GetMuscles(), props.GetStr( "source", props.GetStr( "target" ) ) + GetSideName( area.side ) );

			// init names
			String reflexname = GetReflexName( m_Target.GetName(), source.GetName() );
			opt::ScopedParamSetPrefixer prefixer( par, reflexname + "." );

			INIT_PARAM_NAMED( props, par, length_gain, "KL", 0.0 );
			INIT_PARAM_NAMED( props, par, length_ofs, "L0", 1.0 );
			INIT_PARAM_NAMED( props, par, velocity_gain, "KV", 0.0 );
			INIT_PARAM_NAMED( props, par, force_gain, "KF", 0.0 );

			if ( props.HasKey( "Condition" ) )
			{
				const PropNode& cp = props.GetChild( "Condition" );
				sim::Dof& dof = *FindByName( model.GetDofs(), cp.GetStr( "dof" ) + GetSideName( area.side ) );
				m_pConditionalDofPos = &model.AcquireDelayedSensor< sim::DofPositionSensor >( dof );
				m_pConditionalDofVel = &model.AcquireDelayedSensor< sim::DofVelocitySensor >( dof );
				m_ConditionalPosRange = Range< Degree >( cp.GetChild( "pos_range" ) );
			}

			// create delayed sensors
			if ( force_gain != 0.0 )
				m_pForceSensor = &model.AcquireDelayedSensor< sim::MuscleForceSensor >( source );

			if ( length_gain != 0.0 )
				m_pLengthSensor = &model.AcquireDelayedSensor< sim::MuscleLengthSensor >( source );

			if ( velocity_gain != 0.0 )
				m_pVelocitySensor = &model.AcquireDelayedSensor< sim::MuscleVelocitySensor >( source );
		}

		MuscleReflex::~MuscleReflex()
		{
		}

		void MuscleReflex::ComputeControls( double timestamp )
		{
			if ( m_pConditionalDofPos )
			{
				// check the condition
				Degree dofpos = Radian( m_pConditionalDofPos->GetValue( delay ) );
				if ( !m_ConditionalPosRange.Test( dofpos ) )
				{
					Real violation = m_ConditionalPosRange.GetRangeViolation( dofpos );
					Real dofvel = m_pConditionalDofVel->GetValue( delay );

					if ( std::signbit( violation ) == std::signbit( dofvel ) )
					{
						//log::Trace( m_Target.GetName() + ": Ignoring, " + VARSTR( dofpos ) + VARSTR( dofvel ) );
						return; // TODO: something neater than just return
					}
				}
			}

			// add stretch reflex
			Real u_l = m_pLengthSensor ? length_gain * std::max( 0.0, m_pLengthSensor->GetValue( delay ) - length_ofs ) : 0;

			// add velocity reflex
			// TODO: should velocity gain be positive only?
			Real u_v = m_pVelocitySensor ? velocity_gain * std::max( 0.0, m_pVelocitySensor->GetValue( delay ) ) : 0;

			// add force reflex
			Real u_f = m_pForceSensor ? force_gain * m_pForceSensor->GetValue( delay ) : 0;

			m_Target.AddControlValue( u_l + u_v + u_f );

#ifdef DEBUG_MUSCLE
			if ( m_Target.GetName() == DEBUG_MUSCLE )
				log::TraceF( "u_l=%.3f u_v=%.3f u_f=%.3f", u_l, u_v, u_f );
#endif
		}
	}
}
