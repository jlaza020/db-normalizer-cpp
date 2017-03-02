#pragma once

#include <string>
#include <vector>

namespace DbNormalizerCpp {

	// =========================================================================
	// An AttributeType enumeration that represents the type of an attribute
	// in a database.
	// =========================================================================
	enum AttributeType { String, Float, Int };

	// =========================================================================
	// Attribute struct that represents an attribute like a column in a
	// relational database.
	// =========================================================================
	struct Attribute {

		std::string name;
		AttributeType type;
		bool prime = false;

	};

	using AttributeTable = std::vector<Attribute>;

}