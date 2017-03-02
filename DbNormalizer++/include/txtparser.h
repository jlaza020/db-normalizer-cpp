#pragma once

#include <string>

#include "database.h"
#include "iparser.h"

namespace DbNormalizerCpp {

	// =========================================================================
	// TxtParser class whose use-case is to parse a DbNormalizer++ .txt file.
	// After parsing the file, a Database will be returned initialized with 
	// a name, a universal set of attributes, and a set of functional 
	// dependencies.
	// =========================================================================
	class TxtParser : public IParser {

	public:

	// =========================================================================
	// Constructors and Destructors
	// =========================================================================
		
		TxtParser() {};
		~TxtParser() {};

	// =========================================================================
	// Member functions
	// =========================================================================

		// =====================================================================
		// Parses .txt file and creates a Database object.
		//
		// Returns a Database initialized with name, 
		// universal set of attributes, and set of functional dependencies.
		// =====================================================================
		Database Parse() {

			enum LineType { DbName, AttrSet, FuncDep };

			LineType line_type = LineType::DbName;
			Database db;
			std::string line;

			while (std::getline(fileStream, line)) {


				if (line == "" || line[0] == '#') // Empty line or comment.
					continue;

				if (line_type == LineType::DbName) {
					ProcessDbName(db, line);
					line_type = LineType::AttrSet;
				}
				else if (line_type == LineType::AttrSet) {
					ProcessAttrSet(db, line);
					line_type = LineType::FuncDep;
				}
				else
					ProcessFuncDep(db, line);

			}

			return db;

		}

	private:

	// =========================================================================
	// Member functions
	// =========================================================================

		void AddAttributeNameToFuncDep(SimpleFuncDep & func_dep, const std::string & attr_name, bool processing_left);
		void ProcessDbName(Database & db, const std::string & line);
		void ProcessAttrSet(Database & db, const std::string & line);
		void ProcessFuncDep(Database & db, const std::string & line);

	};

}