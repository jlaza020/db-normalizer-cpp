#include "txtparser.h"

namespace DbNormalizerCpp {

	// =========================================================================
	// Adds Attribute name to a FuncDep.
	//
	// "func_dep":
	//		Functional dependency to add the attribute to.
	//
	// "attr_name":
	//		Name of attribute to add.
	//
	// "processing_left":
	//		True if adding the attribute to the lhs of "func_dep". Otherwise,
	//		false.
	// =========================================================================
	void TxtParser::AddAttributeNameToFuncDep(SimpleFuncDep &func_dep, 
		const std::string &attr_name, bool processing_left = true) {

		if (processing_left)
			func_dep.first.push_back(attr_name);
		else
			func_dep.second.push_back(attr_name);

	}
	
	// =========================================================================
	// Processes a Database name.
	//
	// "db":
	//		Database to process the name for.
	//
	// "line":
	//		Raw string that contains the name to be set for "db".
	// =========================================================================
	void TxtParser::ProcessDbName(Database &db, const std::string &line) {
		db.SetName(line);
	}
	
	// =========================================================================
	// Processes an AttrSet.
	//
	// "db":
	//		Database to process the attribute set for.
	//
	// "line":
	//		Raw string that contains the attribute set to be set for "db".
	// =========================================================================
	void TxtParser::ProcessAttrSet(Database &db, const std::string &line) {

		std::string attr_name;

		for (char token : line) {

			if (token == ',') {
				db.InsertAttribute(attr_name);
				attr_name.clear();
			}
			else {
				attr_name.push_back(token);
			}
		}

		try {
			db.InsertAttribute(attr_name);
		}
		catch (std::exception) {
			throw std::runtime_error(std::string("Could not add last attribute in universal attribute set!"));
		}


	}
	
	// =========================================================================
	// Processes a FuncDep.
	//
	// "db":
	//		Database to process the functional dependency for.
	//
	// "line":
	//		Raw string that contains the functional dependency to be set for 
	//		"db".
	// =========================================================================
	void TxtParser::ProcessFuncDep(Database &db, const std::string &line) {

		bool processing_left = true;
		SimpleFuncDep func_dep;
		std::string attr_name;

		for (std::string::const_iterator it = line.begin(); it != line.end(); it++) {

			if (*it == ',') {
				// Add attribute to appropriate side of func_dep.
				AddAttributeNameToFuncDep(func_dep, attr_name, processing_left);
				attr_name.clear();
			}
			else if (*it == '-') {
				char next_token;
				it++;

				try {
					next_token = *it;
				}
				catch (std::exception) {
					throw std::runtime_error("Expected '>' but reached end of line!");
				}

				if (next_token == '>') {
					// Add attribute to left side of func_dep.
					// Start parsing right side of func_dep.
					AddAttributeNameToFuncDep(func_dep, attr_name);
					attr_name.clear();
					processing_left = false;
				}
				else {
					throw std::runtime_error(std::string("Expected '>' but got '") + next_token + "'!");
				}
			}
			else {
				attr_name.push_back(*it);
			}
		}

		try {
			// Add last attribute to right side of functional dependency.
			AddAttributeNameToFuncDep(func_dep, attr_name, false);
		}
		catch (std::exception) {
			throw std::runtime_error(std::string("Could not add last attribute to functional dependency!"));
		}

		db.InsertFuncDep(func_dep);

	}

}