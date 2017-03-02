#pragma once

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// =============================================================================
// This file consists of program-defined types used by DbNormalizer++.
// =============================================================================

namespace DbNormalizerCpp {

	// =========================================================================
	// Forward declarations
	// =========================================================================

	struct Attribute;
	struct Relation;

	// =========================================================================
	// Table indexes
	// =========================================================================

	using AttributeTblIndex = unsigned int;
	using FuncDepTblIndex = unsigned int;
	using RelationTblIndex = unsigned int;

	// =========================================================================
	// Core types
	// =========================================================================

	using AttributeSet = std::unordered_set<AttributeTblIndex>;
	using Lhs = AttributeSet;
	using Rhs = AttributeSet;
	using FuncDep = std::pair<Lhs, Rhs>;
	using GlobalRelation = Relation;

	// =========================================================================
	// Tables
	//
	// All other tables are in their respective core type files, i.e.,
	// AttributeTable is defined in attribute.h, RelationTable is defined in
	// relation.h, etc.
	// =========================================================================

	using FuncDepTable = std::vector<FuncDep>;

	// =========================================================================
	// Maps
	// =========================================================================

	using AttributeIndexMap = std::unordered_map<std::string, AttributeTblIndex>;

	// =========================================================================
	// Keys
	// =========================================================================

	using PrimaryKey = AttributeSet;
	using CandidateKey = AttributeSet;
	using CandidateKeyList = std::vector<CandidateKey>;

	// =========================================================================
	// Closure
	// =========================================================================

	using AttributeSetClosure = std::pair<FuncDepTblIndex, Rhs>;
	using FuncDepSetClosure = std::vector<AttributeSetClosure>;
	
	using AttributeSetClosureIndex = unsigned int;
	using FuncDepSetClosureIndex = unsigned int;

	// =========================================================================
	// Queues
	// =========================================================================

	using NormalizationQueue = std::queue<Relation>;

	// =========================================================================
	// Used as an easy interface for insertion.
	// =========================================================================

	using SimpleFuncDep = std::pair<std::vector<std::string>, std::vector<std::string>> ;

}