#pragma once

#include <string>
#include <vector>

#include "types.h"

namespace DbNormalizerCpp {
	
	// =========================================================================
	// A Relation struct that basically represents a table in a relational
	// database. Relations house data used by the Database class. 
	// =========================================================================
	struct Relation {
	
		std::string name;
		AttributeSet attributes;
		PrimaryKey primary_key;
		CandidateKeyList candidate_keys;
		FuncDepSetClosure closure; 

	};

	using RelationTable = std::vector<Relation>;

}