#include "py_algorithms.hh"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>

#include "../Algorithm.hh"
#include "../NEvaluator.hh"
#include "../NDSolver.hh"
#include "../NIntegrator.hh"
#include "../NTensor.hh"
#include "../NInterpolatingFunction.hh"

#include "../algorithms/canonicalise.hh"
#include "../algorithms/collect_components.hh"
#include "../algorithms/collect_factors.hh"
#include "../algorithms/collect_terms.hh"
#include "../algorithms/combine.hh"
#include "../algorithms/complete.hh"
#include "../algorithms/decompose.hh"
#include "../algorithms/decompose_product.hh"
#include "../algorithms/distribute.hh"
#include "../algorithms/drop_weight.hh"
#include "../algorithms/einsteinify.hh"
#include "../algorithms/eliminate_kronecker.hh"
#include "../algorithms/eliminate_metric.hh"
#include "../algorithms/eliminate_vielbein.hh"
#include "../algorithms/epsilon_to_delta.hh"
#include "../algorithms/evaluate.hh"
#include "../algorithms/expand.hh"
#include "../algorithms/expand_delta.hh"
#include "../algorithms/expand_diracbar.hh"
#include "../algorithms/expand_dummies.hh"
#include "../algorithms/expand_power.hh"
#include "../algorithms/explicit_indices.hh"
#include "../algorithms/factor_in.hh"
#include "../algorithms/factor_out.hh"
#include "../algorithms/fierz.hh"
#include "../algorithms/first_order_form.hh"
#include "../algorithms/flatten_sum.hh"
#include "../algorithms/indexsort.hh"
#include "../algorithms/integrate_by_parts.hh"
#include "../algorithms/join_gamma.hh"
#include "../algorithms/keep_terms.hh"
#include "../algorithms/lower_free_indices.hh"
#include "../algorithms/lr_tensor.hh"
#ifdef MATHEMATICA_FOUND
#include "../algorithms/map_mma.hh"
#endif
#include "../algorithms/map_sympy.hh"
#include "../algorithms/meld.hh"
#include "../algorithms/nevaluate.hh"
#include "../algorithms/nval.hh"
#include "../algorithms/order.hh"
#include "../algorithms/product_rule.hh"
#include "../algorithms/reduce_delta.hh"
#include "../algorithms/rename_dummies.hh"
#include "../algorithms/replace_match.hh"
#include "../algorithms/rewrite_indices.hh"
#include "../algorithms/simplify.hh"
#include "../algorithms/sort_product.hh"
#include "../algorithms/sort_spinors.hh"
#include "../algorithms/sort_sum.hh"
#include "../algorithms/split_gamma.hh"
#include "../algorithms/split_index.hh"
#include "../algorithms/substitute.hh"
#include "../algorithms/sym.hh"
#include "../algorithms/tab_dimension.hh"
#include "../algorithms/take_match.hh"
#include "../algorithms/unwrap.hh"
#include "../algorithms/unzoom.hh"
#include "../algorithms/untrace.hh"
#include "../algorithms/vary.hh"
#include "../algorithms/young_project.hh"
#include "../algorithms/young_project_product.hh"
#include "../algorithms/young_project_tensor.hh"
#include "../algorithms/zoom.hh"

#include "../properties/ImaginaryI.hh"

namespace cadabra {
	namespace py = pybind11;

	void                                                                set_variables(NEvaluator& evaluator, py::dict d);
	std::tuple<Ex, NTensor, std::complex<double>, std::complex<double>> extract_var_range(py::dict d);

	void init_algorithms(py::module& m)
		{
		pybind11::enum_<Algorithm::result_t>(m, "result_t")
		.value("checkpointed", Algorithm::result_t::l_checkpointed)
		.value("changed", Algorithm::result_t::l_applied)
		.value("unchanged", Algorithm::result_t::l_no_action)
		.value("error", Algorithm::result_t::l_error)
		.export_values();

		def_algo<canonicalise>(m, "canonicalise", true, false, 0);
		def_algo<collect_components>(m, "collect_components", true, false, 0);
		def_algo<collect_factors>(m, "collect_factors", true, false, 0);
		def_algo<collect_terms>(m, "collect_terms", true, false, 0);
		def_algo<decompose_product>(m, "decompose_product", true, false, 0);
		def_algo<distribute>(m, "distribute", true, false, 0);
		def_algo<eliminate_kronecker>(m, "eliminate_kronecker", true, false, 0);
		def_algo<expand>(m, "expand", true, false, 0);
		def_algo<expand_delta>(m, "expand_delta", true, false, 0);
		def_algo<expand_diracbar>(m, "expand_diracbar", true, false, 0);
		def_algo<expand_dummies, const Ex*, bool>(m, "expand_dummies", true, false, 0, py::arg("components") = nullptr, py::arg("zero_missing_components") = true);
		def_algo<expand_power>(m, "expand_power", true, false, 0);
		def_algo<explicit_indices>(m, "explicit_indices", true, false, 0);
		def_algo<first_order_form, Ex>(m, "first_order_form", true, false, 0, py::arg("functions") = Ex{});
		def_algo<flatten_sum>(m, "flatten_sum", true, false, 0);
		def_algo<indexsort>(m, "indexsort", true, false, 0);
		def_algo<lr_tensor>(m, "lr_tensor", true, false, 0);
		def_algo<product_rule>(m, "product_rule", true, false, 0);
		def_algo<reduce_delta>(m, "reduce_delta", true, false, 0);
		def_algo<sort_product>(m, "sort_product", true, false, 0);
		def_algo<sort_spinors>(m, "sort_spinors", true, false, 0);
		def_algo<sort_sum>(m, "sort_sum", true, false, 0);
		def_algo<tabdimension>(m, "tab_dimension", true, false, 0);
		def_algo<young_project_product>(m, "young_project_product", true, false, 0);
		def_algo<combine, Ex>(m, "combine", true, false, 0, py::arg("trace_op") = Ex{});
		def_algo<complete, Ex>(m, "complete", false, false, 0, py::arg("add"));
		def_algo<decompose, Ex>(m, "decompose", false, false, 0, py::arg("basis"));
		def_algo<drop_weight, Ex>(m, "drop_weight", false, false, 0, py::arg("condition") = Ex{});
		def_algo<eliminate_metric, Ex,bool>(m, "eliminate_metric", true, false, 0, py::arg("preferred") = Ex{}, py::arg("redundant") = false);
		def_algo<eliminate_vielbein, Ex,bool>(m, "eliminate_vielbein", true, false, 0, py::arg("preferred") = Ex{},py::arg("redundant")= false);
		def_algo<keep_weight, Ex>(m, "keep_weight", false, false, 0, py::arg("condition"));
		def_algo<lower_free_indices, bool>(m, "lower_free_indices", true, false, 0, py::arg("lower") = true);
		def_algo<lower_free_indices, bool>(m, "raise_free_indices", true, false, 0, py::arg("lower") = false);
		def_algo<integrate_by_parts, Ex>(m, "integrate_by_parts", true, false, 0, py::arg("away_from"));
		def_algo<young_project_tensor, bool>(m, "young_project_tensor", true, false, 0, py::arg("modulo_monoterm") = false);
		def_algo<join_gamma, bool, bool>(m, "join_gamma", true, false, 0, py::arg("expand") = true, py::arg("use_gendelta") = false);
		def_algo<einsteinify, Ex>(m, "einsteinify", true, false, 0, py::arg("metric") = Ex{});
		def_algo<evaluate, Ex, bool, bool>(m, "evaluate", false, false, 0, py::arg("components") = Ex{}, py::arg("rhsonly") = false, py::arg("simplify") = true);
		def_algo<keep_terms, std::vector<int>>(m, "keep_terms", true, false, 0, py::arg("terms"));
		def_algo<young_project, std::vector<int>, std::vector<int>>(m, "young_project", true, false, 0, py::arg("shape"), py::arg("indices"));
		def_algo<simplify>(m, "simplify", false, false, 0);
		def_algo<order, Ex, bool>(m, "order", true, false, 0, py::arg("factors"), py::arg("anticommuting") = false);
		def_algo<epsilon_to_delta, bool>(m, "epsilon_to_delta", true, false, 0, py::arg("reduce") = true);
		def_algo<rename_dummies, std::string, std::string>(m, "rename_dummies", true, false, 0, py::arg("set") = "", py::arg("to") = "");
		def_algo<sym, Ex, bool>(m, "sym", true, false, 0, py::arg("items"), py::arg("antisymmetric") = false);
		def_algo<sym, Ex, bool>(m, "asym", true, false, 0, py::arg("items"), py::arg("antisymmetric") = true);
		def_algo<sym, std::vector<unsigned int>, bool>(m, "slot_sym", true, false, 0, py::arg("items"), py::arg("antisymmetric") = false);
		def_algo<sym, std::vector<unsigned int>, bool>(m, "slot_asym", true, false, 0, py::arg("items"), py::arg("antisymmetric") = true);
		def_algo<factor_in, Ex>(m, "factor_in", true, false, 0, py::arg("factors"));
		def_algo<factor_out, Ex, bool>(m, "factor_out", true, false, 0, py::arg("factors"), py::arg("right") = false);
		def_algo<fierz, Ex>(m, "fierz", true, false, 0, py::arg("spinors"));
		def_algo<substitute, Ex&, bool>(m, "substitute", true, false, 0, py::arg("rules"), py::arg("partial") = true);
		def_algo<take_match, Ex&>(m, "take_match", true, false, 0, py::arg("rules"));
		def_algo<replace_match>(m, "replace_match", false, false, 0);
		def_algo<zoom, Ex, bool>(m, "zoom", true, false, 0, py::arg("rules"), py::arg("partial") = true);
		def_algo_preorder<unzoom>(m, "unzoom", true, false, 0);
		def_algo<untrace>(m, "untrace", true, false, 0);
		def_algo<rewrite_indices, Ex, Ex>(m, "rewrite_indices", true, false, 0, py::arg("preferred"), py::arg("converters"));
		def_algo_preorder<vary, Ex&>(m, "vary", false, false, 0, py::arg("rules"));
		def_algo<split_gamma, bool>(m, "split_gamma", true, false, 0, py::arg("on_back"));
		def_algo<split_index, Ex>(m, "split_index", true, false, 0, py::arg("rules"));
		def_algo<unwrap, Ex>(m, "unwrap", true, false, 0, py::arg("wrapper") = Ex{});
//		def_algo_preorder<young_reduce, const Ex*>(m, "young_reduce", true, false, 0, py::arg("pattern") = nullptr);
//		def_algo_preorder<young_reduce_trace>(m, "young_reduce_trace", true, false, 0);
		def_algo_preorder<meld, bool>(m, "meld", true, false, 0, py::arg("project_as_sum") = false);

//		def_algo<nevaluate>(m, "nevaluate", true, false, 0);

		m.def("nrange", 
				[](py::dict d) {
				auto ret = extract_var_range(d);
				return std::make_pair(std::get<0>(ret), std::get<1>(ret));
				}
				);

		m.def("nval",
				// We cannot use `def_algo` because we first need to
				// convert the Python dict into something that `nevaluate`
				// can handle (you cannot do that with pybind11
				// automagic).
				[](Ex_ptr ex, py::dict d) {
				std::vector<std::pair<Ex, NTensor>> values;
				NEvaluator ev;
				set_variables(ev, d);
				return apply_algo_preorder<nval>(ex, ev, true, false, 0);
				},
				py::arg("ex"),
				py::arg("values")=py::dict(),
				pybind11::doc(read_manual(m, "algorithms", "nval").c_str())
				);
		
		m.def("nevaluate",
				// We cannot use `def_algo` because we first need to
				// convert the Python dict into something that `nevaluate`
				// can handle (you cannot do that with pybind11
				// automagic).
				[](Ex_ptr ex, py::dict d) {
				std::vector<std::pair<Ex, NTensor>> values;
				NEvaluator ev((*ex).begin()); // FIXME: we do not know the entry point, it does not have to be top-level.
				set_variables(ev, d);
				return ev.evaluate();
//				return apply_algo<nevaluate>(ex, ev, false, false, 0);
				}
				);
		
		m.def("ndsolve",
				[](Ex_ptr ex, py::dict initial, py::tuple range, Ex_ptr stop) {
				NDSolver nds(*ex);
				// initial values
				for(const auto& iv: initial)
					nds.set_initial_value( py::cast<Ex>(iv.first), py::cast<double>(iv.second) );

				if(range.size()!=3)
					throw ConsistencyException("ndsolve: integration range must have exactly three elements.");

				nds.set_range( py::cast<Ex>(range[0]), py::cast<double>(range[1]), py::cast<double>(range[2]));
				nds.set_stop(*stop);
					
				auto res = nds.integrate();
				auto rexv = std::make_shared<Ex>("\\comma");

				for(size_t n=1; n<res.size(); ++n) {
					Ex rex("\\equals");

					// Create interpolating function.
					auto rif = std::make_shared<NInterpolatingFunction>();
					rif->var = py::cast<Ex>(range[0]);
					rif->var_values = res[0];
					rif->fun_values = res[n];

					// Fill equality lhs and rhs;
					rex.append_child(rex.begin(), nds.functions()[n-1].begin());
					auto rf = rex.append_child(rex.begin(), Ex("1").begin());
					rf->content = rif;

					// Add to list.
					rexv->move_in_below(rexv->begin(), rex);
					}
				
				return rexv;
				},
				py::arg("ex"),
				py::arg("initial"),
				py::arg("range"),
				py::arg("stop") = Ex{}
				);

		m.def("nintegrate",
				[](Ex_ptr ex, py::dict ranges) {
				NIntegrator ni((*ex).begin());

				// Set all variables from the given dict entries.
				set_variables(ni.evaluator, ranges);

				// Set the integration range.
				auto var_range = extract_var_range(ranges);
				ni.set_range( std::get<0>(var_range), std::get<2>(var_range).real(), std::get<3>(var_range).real() );
				
				auto res = ni.integrate();
				// FIXME: we need to have a better test for real integrands
				if(std::abs(res.imag()) > 5.0*std::numeric_limits<double>::epsilon())
					return py::cast(res);
				else
					return py::cast(res.real());
				}
				);

		m.def("Re",
				[](Ex_ptr ex) {
				Ex ret(ex->begin());
				ret.wrap(ret.begin(), str_node("\\Re"));
				return ret;
				}
				);
		
		m.def("Im",
				[](Ex_ptr ex) {
				Ex ret(ex->begin());
				ret.wrap(ret.begin(), str_node("\\Im"));
				return ret;
				}
				);
		
		m.def("get_num_rules", []() {
			return substitute::cache_size();
			});
		}

	void set_variables(NEvaluator& ev, py::dict d)
		{
		// For every variable in the dict, we can take either a numpy.array (or something that
		// auto-converts to it), a tuple (interpreted as a range) or a single complex/double
		// value. Figure out what it is, and convert to a vector of doubles.
		
		for(const auto& dv: d) {
			py::object type_obj = py::type::of(dv.second);
			std::string type_name = py::str(type_obj.attr("__name__"));
			if(type_name=="float") {
				std::vector<double> vec;
				vec.push_back(py::cast<double>(dv.second));
				ev.set_variable(py::cast<Ex>(dv.first), NTensor(vec));
				}
			else if(type_name=="int") {
				std::vector<double> vec;
				vec.push_back(py::cast<int>(dv.second));
				ev.set_variable(py::cast<Ex>(dv.first), NTensor(vec));
				}
			else if(type_name=="complex") {
				std::vector<std::complex<double>> vec;
				vec.push_back(py::cast<std::complex<double>>(dv.second));
				ev.set_variable(py::cast<Ex>(dv.first), NTensor(vec));
				}
			else if(type_name=="tuple") {
				std::vector<double> rvec = py::cast<std::vector<double>>(dv.second);
				if(rvec.size()!=2)
					throw ArgumentException("Range: value tuples must have exactly two elements (start, end); found "+std::to_string(rvec.size())+".");
				auto range = NTensor::linspace(rvec[0], rvec[1], 100);
				ev.set_variable(py::cast<Ex>(dv.first), range);
				}
			else {
				auto nt = NTensor(py::cast<std::vector<double>>(dv.second));
				if(nt.shape.size()==0 || nt.shape[0]==0)
					throw ArgumentException("Range: value cannot be converted to a numerical range.");
				
				ev.set_variable(py::cast<Ex>(dv.first), nt);
				}
			}
		ev.set_lookup_function([](const Ex& var) {
			Kernel *kernel = get_kernel_from_scope();
			auto imaginaryI = kernel->properties.get<ImaginaryI>(var.begin());
			if(imaginaryI) return std::complex<double>(0, 1.0);
			
			throw std::logic_error("No value.");
			});
		}

	std::tuple<Ex, NTensor, std::complex<double>, std::complex<double>> extract_var_range(py::dict d)
		{
		// Given a dict of variable to value mappings, extract the
		// single variable for which the value is an explicit or
		// implicit range, and return a tuple consisting of the
		// variable and the range. Throws an exception if there is
		// more than one range, or none.
		NTensor range(0);
		std::complex<double> start, end;
		Ex      rangevar;
		bool found=false;
		for(const auto& dv: d) {
			py::object type_obj = py::type::of(dv.second);
			std::string type_name = py::str(type_obj.attr("__name__"));
			if(!(type_name=="float" || type_name=="complex" || type_name=="int")) {
				if(found)
					throw ArgumentException("nrange: found more than one variable with a value range.");
				
				rangevar = py::cast<Ex>(dv.first);
				if(type_name=="tuple") {
					std::vector<double> rvec = py::cast<std::vector<double>>(dv.second);
					if(rvec.size()!=2)
						throw ArgumentException("nrange: value tuples must have exactly two elements (start, end); found "+std::to_string(rvec.size())+".");
					range = NTensor::linspace(rvec[0], rvec[1], 100);
					found = true;
					start = rvec[0];
					end   = rvec[1];
					}
				else {
					auto manrange = py::cast<std::vector<double>>(dv.second);
					start = manrange[0];
					end   = manrange[manrange.size()-1];
					range = NTensor(manrange);
					found = true;
					}
				}
			}
		if(!found)
			throw ArgumentException("nrange: not found any variable with a value range.");

		return std::make_tuple(rangevar, range, start, end);
		}
}
