
#pragma once

#include "Props.hh"

namespace cadabra {

	class Atomic : virtual public property {
		public:
            Atomic();
			~Atomic();
			virtual std::string name() const;
        
        private:
            size_t identifier = 0;
		};

	}
