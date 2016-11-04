#pragma once

#include "opt_config.h"
#include "opt_fwd.h"
#include "Objective.h"
#include "scone/core/HasSignature.h"
#include "scone/core/memory_tools.h"
#include "ParamSet.h"

namespace scone
{
	namespace opt
	{
		class SCONE_API Optimizer : public HasSignature
		{
		public:
			Optimizer( const PropNode& props );
			virtual ~Optimizer();

			Objective& GetObjective() { SCONE_ASSERT( m_Objectives.size() > 0 ); return *m_Objectives[ 0 ]; }
			const Objective& GetObjective() const { SCONE_ASSERT( m_Objectives.size() > 0 ); return *m_Objectives[ 0 ]; }
			virtual void Run() = 0;

			/// get the results output folder (creates it if it doesn't exist)
			const String& AcquireOutputFolder();

			bool IsBetterThan( double v1, double v2 ) { return IsMinimizing() ? v1 < v2 : v1 > v2; }
			bool IsMinimizing() { return !maximize_objective; }

			std::vector< double > Evaluate( std::vector< ParamSet >& parsets );

			const ParamSet& GetBestParamSet() { return m_BestParams; }
			double GetBestFitness() { return m_BestFitness; }

			void SetConsoleOutput( bool output ) { console_output = output; }
			bool GetProgressOutput() { return console_output && !status_output; }
			bool GetStatusOutput() const { return status_output; }
			void SetStatusOutput( bool s ) { status_output = s; }
			template< typename T > void OutputStatus( const String& key, const T& value ) {
				if ( GetStatusOutput() )
					std::cout << std::endl << "*" << key << "=" << value << std::endl;
			}

		protected:
			void CreateObjectives( size_t count );
			const PropNode& m_ObjectiveProps;
			std::vector< ObjectiveUP > m_Objectives;
			void ManageFileOutput( double fitness, const std::vector< String >& files );
			virtual String GetClassSignature() const override;

			// current status
			ParamSet m_BestParams;
			double m_BestFitness;
			bool console_output;
			bool status_output;
			size_t m_LastFileOutputGen;

			// properties
			size_t max_threads;
			int thread_priority;
			bool maximize_objective;
			bool show_optimization_time;
			Real min_improvement_factor_for_file_output;
			size_t max_generations_without_file_output;
			String init_file;
			bool use_init_file;
			bool output_objective_result_files;

		private:
			std::vector< double > EvaluateSingleThreaded( std::vector< ParamSet >& parsets );
			std::vector< double > EvaluateMultiThreaded( std::vector< ParamSet >& parsets );
			static void EvaluateFunc( Objective* obj, ParamSet& par, double* fitness, int priority );
			void InitOutputFolder();
            static void SetThreadPriority( int priority );

			String m_Name;
			String m_OutputFolder;
			std::vector< std::pair< double, std::vector< String > > > m_OutputFiles;

		private: // non-copyable and non-assignable
			Optimizer( const Optimizer& );
			Optimizer& operator=( const Optimizer& );
		};
	}
}
