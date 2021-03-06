/*
** StateController.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/controllers/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"

namespace scone
{
	class StateController : public Controller
	{
	public:
		StateController( const PropNode& props, Params& par, Model& model, const Location& loc );
		StateController( const StateController& other ) = delete;
		StateController& operator=( const StateController& other ) = delete;

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		typedef size_t StateIndex;
		virtual size_t GetStateCount() const = 0;
		virtual const String& GetStateName( StateIndex i ) const = 0;
		virtual StateIndex GetCurrentState( Model& model, double timestamp ) = 0;

		virtual String GetClassSignature() const override;

		struct ConditionalControllerState
		{
			std::vector< bool > state_mask;
			bool is_active;
			TimeInSeconds is_active_since;
		};

		typedef std::pair< ConditionalControllerState, ControllerUP > ConditionalController;
		std::vector< ConditionalController > m_ConditionalControllers;

		void CreateConditionalControllers( const PropNode& props, Params& par, Model& model, const Location& loc );
		void UpdateCurrentState( Model& model, TimeInSeconds timestamp );

		StateIndex m_CurrentState;
	};
}
