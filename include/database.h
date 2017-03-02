#pragma once

#include <string>

#include "attribute.h"
#include "relation.h"
#include "types.h"

namespace DbNormalizerCpp {

	// =========================================================================
	// NormalForm enumeration whose values represent the current normalization
	// form of the database. In other words, NormalForm::One means 1NF, 
	// NormalForm::Bcnf means BCNF, etc.
	// =========================================================================
	enum NormalForm { One, Two, Three, Bcnf };

	// =========================================================================
	// Database class that represents a database as specified in the
	// DbNormalizer++ file. A Database has a name, a universal set of
	// attributes, and a set of functional dependencies. Operations are mostly
	// to normalize the database to 2NF, 3NF, BCNF, etc.
	// =========================================================================
	class Database {

	public:

	// =========================================================================
	// Constructors and Destructors
	// =========================================================================

		Database();
		~Database();

	// =========================================================================
	// Member functions
	// =========================================================================

		// =====================================================================
		// Inserts an attribute into this database.
		//
		// "attr_name":
		//		The name of the attribute to insert.
		//
		// Side effects:
		//		An attribute is inserted into private data member 
		//		"attribute_table", increasing its size by 1.
		// =====================================================================
		void InsertAttribute(const std::string & attr_name);

		// =====================================================================
		// Inserts functional dependency into this database.
		// 
		// "func_dep":
		//		The functional dependency to insert.
		//
		// Precondition: 
		//		The attribute table is populated with the attributes referenced
		//		in the func_dep.
		//
		// Side effects:
		//		A functional dependency is inserted into private data member
		//		"func_dep_table", increasing its size by 1.
		// =====================================================================
		void InsertFuncDep(SimpleFuncDep & func_dep);

		// =====================================================================
		// Normalizes database to 2NF.
		//
		// "max_threads": 
		//		Maximum number of threads to spawn to normalize to 2NF.
		// =====================================================================
		void NormalizeTo2nf(unsigned int max_threads = 1);
			
		// =====================================================================
		// Normalizes database to 3NF.
		//
		// "max_threads": 
		//		Maximum number of threads to spawn to normalize to 3NF.
		// =====================================================================
		void NormalizeTo3nf(unsigned int max_threads = 1);

		// =====================================================================
		// Prints database.
		// =====================================================================
		void Print();

		// =====================================================================
		// Sets name of database.
		//
		// "_name":
		//		Name to set this database's name to.
		// =====================================================================
		void SetName(const std::string & _name);

	private:
		
	// =========================================================================
	// Data members
	// =========================================================================

		AttributeIndexMap					// Map of attribute names to indexes
			attribute_index_map;			// of attribute objects in												
											// attribute_table.

		AttributeTable attribute_table;		// Collection of attribute objects.

		FuncDepTable func_dep_table;		// Collection of functional 
											// dependencies.
			
		static const std::string			// Name of global relation.
			GLOBAL_RELATION_NAME;					
			
		std::string name;					// Name of database.
		NormalForm normal_form;				// Current NF of database.
		unsigned int relation_num;			// Next decomposed relation number.
			
		RelationTable relation_table; 		// Collection of decomposed 
											// relations.
			
		AttributeTblIndex table_index;		// Next available index into 
											// attribute table.

	// =========================================================================
	// Member functions
	// =========================================================================

		void AppendToFuncDep(SimpleFuncDep & func_dep, AttributeSet & attribute_set, bool lhs);
		void AssignPrimaryKey(Relation & relation);
		void ComputeAttributeSetClosure(FuncDepTblIndex fd_tbl_index, Relation & relation);
		void ComputeClosure(Relation & relation);
		void ComputeFuncDepSetClosure(Relation & relation);
		void ComputeGblAttributeSetClosure(FuncDepTblIndex fd_tbl_index, GlobalRelation & gbl_relation);
		void ComputeGblFuncDepSetClosure(GlobalRelation & gbl_relation);
		GlobalRelation GenerateGlobalRelation();
		bool IsPartialPrimaryKey(AttributeSet & attributes, Relation & relation);
		bool IsSubsetOf(AttributeSet & a, AttributeSet & b);
		AttributeTblIndex LookUpAttributeTblIndex(const std::string & attr_name);
		void MarkPrimeAttributes();
		RelationTable MultiThreaded2nf(NormalizationQueue & normalization_queue, unsigned int max_threads);
		RelationTable MultiThreaded3nf(NormalizationQueue & normalization_queue, unsigned int max_threads);
		void PrintName();
		void PrintAttributeFromIndex(AttributeTblIndex index, bool verbose);
		void PrintAttribute(const Attribute & attribute, bool verbose);
		void PrintAttrSet(const AttributeSet & attribute_set);
		void PrintFuncDep(const FuncDep & func_dep);
		void PrintAttributeSetClosure(const AttributeSetClosure & attribute_set_closure);
		void PrintFuncDepSetClosure(const FuncDepSetClosure & func_dep_set_closure);
		void PrintAttrTable();
		void PrintCandidateKeyList(const CandidateKeyList & ck_list);
		void PrintRelation(const Relation & relation);
		void PrintFuncDepTable();
		void PrintRelationTable();
		void QueuePreNormalizedRelations(NormalizationQueue & normalization_queue);
		RelationTable RelationTo2nf(Relation & relation);
		RelationTable RelationTo3nf(Relation & relation);
		void SingleThreaded2nf(NormalizationQueue & normalization_queue);
		RelationTable SingleThreaded3nf(NormalizationQueue & normalization_queue);

};

}