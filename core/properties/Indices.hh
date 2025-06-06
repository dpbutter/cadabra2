
#pragma once

#include "Props.hh"

namespace cadabra {

	class Indices : public list_property {
		public:
			Indices(); //const std::string& parent="");
			virtual bool parse(Kernel&, std::shared_ptr<Ex>, keyval_t&) override;
			virtual std::string name() const override;
			virtual std::string unnamed_argument() const override
				{
				return "name";
				};
			virtual match_t equals(const property *) const override;
			virtual void    validate(const Kernel&, const Ex&) const override;
			
			//		virtual void display(std::ostream&) const override;
			virtual void latex(std::ostream&) const override;

			std::string set_name, parent_name;
			enum position_t { free=0, fixed=1, independent=2 } position_type;

			/// List of possible values that indices of this type can
			/// take.  This gets collected from the `values` parameter,
			/// or determined from any `Integer` property. This can be
			/// empty if the values are not (yet) known so the caller
			/// needs to check that explicitly.
			const std::vector<Ex>& values(const Properties&, Ex::iterator) const;

		private:
			/// Given the right-hand side of a 'values={...}' node, generate
			/// a list of all index values in index_values.

			void collect_index_values(Ex::iterator ind_values);

			// Explicit store of index values;
			mutable std::vector<Ex> values_;
		};

	}
