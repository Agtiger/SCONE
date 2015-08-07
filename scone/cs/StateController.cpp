#include "stdafx.h"

#include "StateController.h"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "../sim/Factories.h"

namespace scone
{
	namespace cs
	{
		StateController::StateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		sim::Controller( props, par, model, area )
		{
		}

		StateController::~StateController()
		{
		}

		void StateController::CreateConditionalControllers( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area )
		{
			// create instances for each controller
			const PropNode& ccProps = props.GetChild( "ConditionalControllers" );
			for ( PropNode::ConstChildIter ccIt = ccProps.Begin(); ccIt != ccProps.End(); ++ccIt )
			{
				// get state masks
				String state_masks = ccIt->second->GetStr( "states" );
				boost::char_separator< char > state_mask_seperator(";,");
				boost::tokenizer< boost::char_separator< char > > state_tokens( state_masks, state_mask_seperator );
				BOOST_FOREACH( const String& instance_states, state_tokens )
				{
					// create new conditional controller
					m_ConditionalControllers.push_back( ConditionalController() );
					ConditionalController& cc = m_ConditionalControllers.back();
					ConditionalControllerState& ccs = cc.first;
					ccs.is_active = false;
					ccs.is_active_since = 0;

					// initialize state_mask based on names in instance_states (TODO: use tokenizer?)
					String bit_string( GetStateCount(), '0' );
					ccs.state_mask.resize( GetStateCount(), false );
					bool has_any_state = false;
					for ( size_t i = 0; i < GetStateCount(); ++i )
					{
						if ( instance_states.find( GetStateName( i ) ) != String::npos )
						{
							ccs.state_mask[ i ] = has_any_state = true;
							bit_string[ GetStateCount() - 1 - i ] = '1';
						}
					}
					SCONE_THROW_IF( !has_any_state, "Conditional Controller has empty state mask" )

					// create controller
					const PropNode& cprops = ccIt->second->GetChild( "Controller" );
					opt::ScopedParamSetPrefixer prefixer( par, "S" + bit_string + "." );
					cc.second = sim::CreateController( cprops, par, model, area );
				}
			}
		}

		void StateController::UpdateConditionalControllerStates( StateIndex current_state, TimeInSeconds timestamp )
		{
			SCONE_ASSERT( current_state < GetStateCount() );

			// update controller states
			BOOST_FOREACH( ConditionalController& cc, m_ConditionalControllers )
			{
				ConditionalControllerState& ccs = cc.first;
				bool activate = ccs.state_mask[ current_state ];

				// activate or deactivate controller
				if ( activate != ccs.is_active )
				{
					ccs.is_active = activate; 
					ccs.is_active_since = timestamp;
				}
			}
		}

		StateController::UpdateResult StateController::UpdateControls( sim::Model& model, double timestamp )
		{
			BOOST_FOREACH( ConditionalController& cc, m_ConditionalControllers )
			{
				if ( cc.first.is_active )
					cc.second->UpdateControls( model, timestamp - cc.first.is_active_since );
			}

			return SuccessfulUpdate;
		}

		scone::String StateController::GetClassSignature() const 
		{
			String s = "S";

			std::map< String, int > controllers;
			BOOST_FOREACH( const ConditionalController& cc, m_ConditionalControllers )
				controllers[ cc.second->GetSignature() ] += 1;

			// output number of controllers per leg
			for ( auto it = controllers.begin(); it != controllers.end(); ++it )
				s += "_" + ToString( it->second ) + it->first;

			return s;
		}
	}
}