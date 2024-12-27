
#pragma once

#include "Algorithm.hh"

namespace cadabra {

	/// \ingroup algorithms
	///
	/// Distribute factors over a sum, that is,
	/// \f$ (A+B) C \rightarrow A C + B C \f$


	class distribute : public Algorithm {
		public:
			distribute(const Kernel&, Ex&);

			virtual bool     can_apply(iterator);
			virtual result_t apply(iterator&);

			// virtual Ex::queued_iterator build_queued_iterator();
			virtual Ex_Nodemap::node_sets_t get_mapped_nodes();
			virtual bool is_mapped();
		private:
			std::vector<Ex> patterns_;

		};

	}
