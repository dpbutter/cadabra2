
#pragma once

#include <string>
#include <set>
#include <pybind11/pybind11.h>

namespace cadabra {

	std::string escape_quotes(const std::string&);

	/// \ingroup files
	/// Convert a block of Cadabra notation into pure Python. Mimics
	/// the functionality in the python script 'cadabra2'
	/// If display is false, this will not make ';' characters 
	/// generate 'display' statements (used in the conversion of
	/// notebooks to python packages).

	std::string cdb2python(const std::string&, bool display);

	/// Helper function to insert a prefix.
	std::string insert_prefix(const std::string&, const std::string& prefix, size_t prefixloc);
	
	std::string cdb2python_string(const std::string&, bool display, std::string& error);	

	/// Test whether a block of Python code is complete or requires
	/// more input lines. This uses `codeop.compile_command`.
	///     1: complete
   ///     0: incomplete
   ///    -1: indentation error, need backtracking
	///    -2: overcomplete

	int         is_python_code_complete(const std::string&, std::string& error);
	
	/// Determine if the python code `code` contains a reference to
	/// the variable `variable` by parsing it into an AST.
	bool code_contains_variable(const std::string& code, const std::string& variable);

	/// Fill a set with all variables references in the given piece of Python code.
	/// This needs to be proper Python, not a mixture of Python and Cadabra input which
	/// still needs to be pre-processed.
	bool variables_in_code(const std::string& code, std::set<std::string>& variables);

	/// Fill a set with all variables references inside of Cadabra pull-in constructions,
	/// that is, constructions of the form "@(var)". This just does regex matching, no
	/// Python AST walking (we cannot, as this pull-in logic only happens when a
	/// string gets parsed into a Cadabra Ex, but that's all inside Ex, not visible
	/// to the server that runs the code.
	bool variables_to_pull_in(const std::string& code, std::set<std::string>& variables);

	/// Given a piece of Python code, remove all lines which assign a value to
	/// the given variable. Return the modified code.
	std::string remove_variable_assignments(const std::string& code, const std::string& variable);

	/// \ingroup files
	/// Object to store pre-parsing intermediate results. Necessary
	/// to keep things tidy but also in order to avoid the fact that
	/// we cannot pass strings by reference between C++ and Python.
	class ConvertData {
		public:
			ConvertData();
			ConvertData(const std::string&, const std::string&, const std::string&, const std::string&);
			
			std::string lhs, rhs, op, indent;
	};
	
	/// \ingroup files
   /// Detect Cadabra expression statements and rewrite to Python form.
   ///  
   /// Lines containing ':=' are interpreted as expression declarations.
   /// Lines containing '::' are interpreted as property declarations.
   /// 
   /// These need to end on '.', ':' or ';'. If not, keep track of the
   /// input so far and store that in self.convert_data.lhs, self.convert_data.op, self.convert_data.rhs, and
   /// then return an empty string.
   /// 
   /// TODO: make ';' at the end of '::' line result the print statement printing 
   /// property objects using their readable form; addresses one issue report).
	///
	/// Returns two strings, one which should be prefixed to the block so far, the other postfixed.

	std::pair<std::string, std::string> convert_line(const std::string&, ConvertData& cv, bool display);

	/// \ingroup files
	/// Convert a Cadabra notebook file to pure Python. This gets
	/// called on-the-fly when importing Cadabra notebooks written by
	/// users, and at install time for all system-supplied packages.
	/// If for_standalone is false, this will not make ';' characters 
	/// generate 'display' statements (used in the conversion of
	/// notebooks to python packages), and it will not convert
	/// any cells which have their `ignore_on_import` flag set.

	#ifndef CDBPYTHON_NO_NOTEBOOK

	std::string cnb2python(const std::string&, bool for_standalone);

	#endif
	}
