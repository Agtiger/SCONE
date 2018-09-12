#pragma once

#include "Function.h"
#include "scone/core/string_tools.h"
#include "PropNode.h"

namespace scone
{
	/// Parameterizable piece-wise constant function.
	class SCONE_API PieceWiseConstantFunction : public Function
	{
	public:
		PieceWiseConstantFunction( const PropNode& props, Params& par );
		virtual ~PieceWiseConstantFunction();

		/// Number of control points in this function.
		size_t control_points;

		/// Parameter for the y value of each control point.
		const PropNode& control_point_y;

		/// Parameter for the dt value of each control point.
		const PropNode& control_point_dt;

		virtual Real GetValue( Real x ) override;

		// a signature describing the function
		virtual String GetSignature() override;

	private:
		struct Impl;
		u_ptr< Impl > m_pImpl;
	};
}
