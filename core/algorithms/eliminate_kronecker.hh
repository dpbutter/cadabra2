
#pragma once

#include "Algorithm.hh"

namespace cadabra {

	class eliminate_kronecker : public Algorithm {
		public:
			eliminate_kronecker(const Kernel&, Ex&);

			virtual bool     can_apply(iterator) override;
			virtual result_t apply(iterator&) override;

			virtual bool is_mapped();
			virtual Ex_Nodemap::node_sets_t get_mapped_nodes();
		};

	}
