#include <assert.h>
#include <iostream>

#include "attribute.h"
#include "database.h"
#include "relation.h"

// =============================================================================
// TODO:
//	*	Replace func_dep_table with closure?
// =============================================================================

namespace DbNormalizerCpp {

	const std::string Database::GLOBAL_RELATION_NAME = "global_relation";

	// =========================================================================
	// Constructs a Database.
	// =========================================================================
	Database::Database() {
	
		normal_form = NormalForm::One;
		relation_num = 1;
		table_index = 0;
	
	}

	// =========================================================================
	// Destroys a Database.
	// =========================================================================
	Database::~Database() {}
	
	// =========================================================================
	// Gets all attribute names in func_dep, looks up the index for each
	// attribute and adds them to attr_indexes.
	//
	// "func_dep":
	//		Functional dependency to insert "attribute_set" into.
	//
	// "attribute_set":
	//		Attribute set to insert into "func_dep".
	//
	// "lhs":
	//		True if "attribute_set" will be inserted into the lhs of "func_dep".
	//		False, if it will be inserted into the rhs.
	// =========================================================================
	void Database::AppendToFuncDep(SimpleFuncDep & func_dep, 
		AttributeSet & attribute_set, bool lhs = true) {

		std::vector<std::string> &side = func_dep.first;

		if (lhs == false)
			side = func_dep.second;

		for (std::string attr_name : side) {

			AttributeTblIndex index = LookUpAttributeTblIndex(attr_name);
			attribute_set.insert(index);

		}


	}

	// =========================================================================
	// Assigns a primary key to relation based on its candidate keys.
	//
	// "relation":
	//		Relation to assign primary key to.
	//
	// Precondition: 
	//		"relation"'s candidate keys have been computed.
	// =========================================================================
	void Database::AssignPrimaryKey(Relation & relation) {

		if (relation.candidate_keys.empty()) {

			for (AttributeTblIndex i = 0; i < attribute_table.size(); i++)
				relation.primary_key.insert(i);

		}
		else {

			CandidateKeyList::iterator smallest = relation.candidate_keys.begin();

			for (CandidateKeyList::iterator it = relation.candidate_keys.begin();
				it != relation.candidate_keys.end(); it++) {

				if (it->size() < smallest->size())
					smallest = it;

			}

			relation.primary_key = *smallest;

		}


	}

	// =========================================================================
	// Computes the AttributeSetClosure for a non-global Relation given a 
	// FuncDepTblIndex.
	//
	// "fd_tbl_index":
	//		Index into the functional dependency table that represents the lhs
	//		of an AttributeSetClosure.
	//
	// "relation":
	//		Non-global Relation for which the AttributeSetClosure will be
	//		computed.
	// =========================================================================
	void Database::ComputeAttributeSetClosure(FuncDepTblIndex fd_tbl_index,
		Relation & relation) {
	}

	// =========================================================================
	// Computes the FuncDepSetClosure for a Relation (can be global or 
	// non-global).
	//
	// "relation":
	//		Global or non-global Relation for which the FuncDepSetClosure will
	//		be computed.
	// =========================================================================
	void Database::ComputeClosure(Relation & relation) {

		if (relation.attributes.size() == attribute_table.size())
			ComputeGblFuncDepSetClosure(relation);
		else {
			ComputeFuncDepSetClosure(relation);
		}

	}

	// =========================================================================
	// Computes the FuncDepSetClosure for a non-global Relation.
	//
	// "relation":
	//		Non-global Relation for which the FuncDepSetClosure will be 
	//		computed.
	// =========================================================================
	void Database::ComputeFuncDepSetClosure(Relation & relation) {



	}

	// =========================================================================
	// Computes the AttributeSetClosure for a GlobalRelation.
	//
	// "fd_tbl_index":
	//		Index into the functional dependency table that represents the lhs
	//		of an AttributeSetClosure.
	// =========================================================================
	void Database::ComputeGblAttributeSetClosure(FuncDepTblIndex fd_tbl_index,
		GlobalRelation & gbl_relation) {

		Rhs closure_rhs;

		// Include the lhs of the current functional dependecy 
		// as part of the closure.
		closure_rhs.insert(func_dep_table[fd_tbl_index].first.begin(),
			func_dep_table[fd_tbl_index].first.end());

		// Include the rhs of the current functional dependecy 
		// as part of the closure.
		closure_rhs.insert(func_dep_table[fd_tbl_index].second.begin(),
			func_dep_table[fd_tbl_index].second.end());

		assert(closure_rhs.size() <= attribute_table.size());

		if (closure_rhs.size() == attribute_table.size()) {

			// The lhs of the current functional dependency functionally 
			// determines all attributes in the relation, so it is a
			// candidate key for relation.

			gbl_relation.candidate_keys.push_back(func_dep_table[fd_tbl_index].first);

		}
		else {

			// The lhs of the current functional dependency does not
			// functionally determine all attributes in the relation,
			// i.e., is not a candidate key.

			// State for do-while loop.
			bool is_lhs_candidate_key = false;
			bool new_insertion;
			std::unordered_set<int> included_func_deps;
			included_func_deps.insert(fd_tbl_index);

			// While a new insertion has been made into closure_rhs,
			// iterate over func_deps. If a functional dependency's
			// lhs is a subset of closure_rhs, include the
			// functional dependency's lhs into closure_rhs.
			do {

				new_insertion = false;

				for (int j = (fd_tbl_index + 1) % func_dep_table.size();
					j != fd_tbl_index;
					j = (j + 1) % func_dep_table.size()) {

					if (included_func_deps.find(j) == included_func_deps.end()
						&& IsSubsetOf(func_dep_table[j].first, closure_rhs)) {

						// This current functional dependency's rhs
						// has not already been included in
						// closure_rhs and its lhs is a subset of
						// closure_rhs.

						// Include the current functional
						// dependency's rhs in closure_rhs.
						closure_rhs.insert(func_dep_table[j].second.begin(),
							func_dep_table[j].second.end());

						assert(closure_rhs.size() <= attribute_table.size());

						// Mark this functional dependency as 
						// "included".
						included_func_deps.insert(j);
						new_insertion = true;

						if (closure_rhs.size() == attribute_table.size()) {

							// Lhs is a candidate key, so break for loop
							// and break do-while loop.

							gbl_relation.candidate_keys.push_back(func_dep_table[fd_tbl_index].first);
							is_lhs_candidate_key = true;
							break;

						}

					}

				}

			} while (!is_lhs_candidate_key && new_insertion);

		}

		// Add closure to relation.
		gbl_relation.closure.push_back(std::make_pair(fd_tbl_index, closure_rhs));


	}

	// =========================================================================
	// Helper function called by GenerateGlobalRelation() that initializes a 
	// GlobalRelation's closure using an existing Database.
	//
	// "gbl_relation":
	//		GlobalRelation for which the FuncDepSetClosure will be computed.
	// =========================================================================
	void Database::ComputeGblFuncDepSetClosure(GlobalRelation & gbl_relation) {

		for (FuncDepTblIndex i = 0; i < func_dep_table.size(); i++) {

			// Iterate over func_deps.

			if (IsSubsetOf(func_dep_table[i].first, gbl_relation.attributes)) {

				// The current functional dependency is relevant to
				// relation's attributes, so find the closure of the
				// set of attributes on the lhs of the current 
				// functional dependency.

				ComputeGblAttributeSetClosure(i, gbl_relation);

			}

		}

	}

	// =========================================================================
	// Helper function called by NormalizeTo2nf() to create the global 
	// (first) Relation. The global Relation's closure is also computed.
	//
	// Returns the generated global Relation.
	// =========================================================================
	GlobalRelation Database::GenerateGlobalRelation() {

		GlobalRelation gbl_relation;

		gbl_relation.name = GLOBAL_RELATION_NAME;

		for (AttributeTblIndex i = 0; i < attribute_table.size(); i++)
			gbl_relation.attributes.insert(i);

		ComputeGblFuncDepSetClosure(gbl_relation);
		AssignPrimaryKey(gbl_relation);

		return gbl_relation;

	}

	// =========================================================================
	// Inserts an attribute into this database.
	//
	// "attr_name":
	//		The name of the attribute to insert.
	//
	// Side effects:
	//		An attribute is inserted into private data member 
	//		"attribute_table", increasing its size by 1.
	// =========================================================================
	void Database::InsertAttribute(const std::string & attr_name) {

		Attribute attr;
		attr.name = attr_name;
		attribute_table.push_back(attr);
		attribute_index_map.insert(std::make_pair(attr_name, table_index));
		table_index++;

	}
	
	// =========================================================================
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
	// =========================================================================
	void Database::InsertFuncDep(SimpleFuncDep & func_dep) {

		Lhs left;
		Rhs right;

		AppendToFuncDep(func_dep, left);
		AppendToFuncDep(func_dep, right, false);

		func_dep_table.push_back(std::make_pair(left, right));

	}

	// =========================================================================
	// Determines whether an attribute set is a proper subset of a Relation's
	// primary key.
	//
	// "attributes":
	//		Set of attributes that is a potential partial primary key of
	//		"relation".
	//
	// "relation":
	//		The relation of which the primary key is used to determine if 
	//		"attributes" is a partial primary key.
	//
	// Returns true if "attributes" is a partial primary key, and false
	// otherwise.
	// =========================================================================
	bool Database::IsPartialPrimaryKey(AttributeSet & attributes,
		Relation & relation) {

		return IsSubsetOf(attributes, relation.primary_key)
			&& attributes.size() < relation.primary_key.size();

	}

	// =========================================================================
	// Helper function that determines whether every attribute in a
	// exists in b.
	// 
	// "a":
	//		Set of attributes. 
	//
	// "b":
	//		Set of attributes.
	//
	// Returns true if "a" is a subset of "b".
	// =========================================================================
	bool Database::IsSubsetOf(AttributeSet & a, AttributeSet & b) {

		for (AttributeSet::const_iterator a_it = a.begin();
			a_it != a.end(); a_it++) {

			if (b.find(*a_it) == b.end())
				return false;

		}

		return true;

	}

	// =========================================================================
	// Uses attribute_index_map to find index of an attribute by using its name.
	//
	// "attr_name":
	//		Name of attribute to find in attribute table.
	//
	// Returns the index into the attribute table at which the attribute is 
	// located.
	// =========================================================================
	AttributeTblIndex Database::LookUpAttributeTblIndex(const std::string &
		attr_name) {

		auto it = attribute_index_map.find(attr_name);

		if (it != attribute_index_map.end())
			return it->second;
		return -1;

	}

	// =========================================================================
	// Marks all prime attributes in attribute_table.
	//
	// Precondition: 
	//		The global relation is generated and at index 0
	//		in private member "relation_table".
	//
	// Side effects:
	//		Attributes in private member "attribute_table" are modified by
	//		setting "prime" to true.
	// =========================================================================
	void Database::MarkPrimeAttributes() {

		for (AttributeTblIndex i = 0; i < attribute_table.size(); i++) {

			if (relation_table.front().primary_key.find(i)
				!= relation_table.front().primary_key.end()) {

				// The current attribute exists in the global relation's
				// primary key.

				attribute_table[i].prime = true; // Mark as prime attribute.

			}

		}

	}

	// =========================================================================
	// Multi-threaded 2NF
	// =========================================================================
	RelationTable Database::MultiThreaded2nf(NormalizationQueue & 
		normalization_queue, unsigned int max_threads) {

		unsigned int num_threads = 0;

		while (!normalization_queue.empty() || num_threads > 0) {

			if (num_threads == max_threads) {
				// wait until a thread finishes
				// process all done threads
				// decrement num_threads by done threads
			}

			Relation &next = normalization_queue.front();
			normalization_queue.pop();
			RelationTo2nf(next); // Generates a new thread to normalize relation.					

		}

		return RelationTable();

	}

	// =========================================================================
	// Multi-threaded 3NF
	// =========================================================================
	RelationTable Database::MultiThreaded3nf(NormalizationQueue &
		normalization_queue, unsigned int max_threads) {

		return RelationTable();

	}

	// =========================================================================
	// Normalizes database to 2NF.
	// =========================================================================
	void Database::NormalizeTo2nf(unsigned int max_threads) { 
		
		if (normal_form >= NormalForm::Two)
			return;

		int num_threads = 0;
		NormalizationQueue normalization_queue;

		// Generate global relation and add it to relation_table.
		relation_table.push_back(GenerateGlobalRelation());
		MarkPrimeAttributes();

		return;

		// Queue global relation for normalization.
		QueuePreNormalizedRelations(normalization_queue);

		if (max_threads == 1) {
			// Normalize using this existing thread.
			SingleThreaded2nf(normalization_queue);
		}
		else {
			// Normalize using additional threads.
			MultiThreaded2nf(normalization_queue, max_threads);

		}

		return;

	}

	// =========================================================================
	// Normalizes database to 3NF.
	// =========================================================================
	void Database::NormalizeTo3nf(unsigned int max_threads) { 
			
	}

	// =========================================================================
	// Prints database.
	// =========================================================================
	void Database::Print() {

		std::cout << "Database:\n\nName: ";
		PrintName();

		std::cout << "\n\nAttributes:\n";
		PrintAttrTable();

		std::cout << "\n\nFunctional Dependencies:\n";
		PrintFuncDepTable();

		std::cout << "\nDecomposed Relations:\n\n";
		PrintRelationTable();

	}

	// =========================================================================
	// Prints an Attribute.
	// 
	// "attribute":
	//		The attribute to print.
	//
	// "verbose":
	//		If true, prints extra information about "attribute", e.g, whether
	//		the attribute is prime or not.
	// =========================================================================
	void Database::PrintAttribute(const Attribute & attribute, 
		bool verbose = false) {

		std::cout << attribute.name;

		if (verbose) {
			std::string prime_str = attribute.prime == true ? "*prime*" : "";
			std::cout << " : " << prime_str;
		}

	}

	// =========================================================================
	// Prints an Attribute From an AttributeTblIndex.
	//
	// "index":
	//		Index of attribute to print.
	//
	// "verbose":
	//		If true, prints extra information about "attribute", e.g, whether it
	//		is prime or not.
	// =========================================================================
	void Database::PrintAttributeFromIndex(AttributeTblIndex index, 
		bool verbose = false) {

		PrintAttribute(attribute_table[index], verbose);

	}

	// =========================================================================
	// Prints an AttributeSet.
	//
	// "attribute_set":
	//		Set of attributes to print.
	// =========================================================================
	void Database::PrintAttrSet(const AttributeSet & attribute_set) {

		std::cout << "{ ";

		for (AttributeSet::const_iterator it = attribute_set.begin(); it != attribute_set.end(); it++) {

			PrintAttributeFromIndex(*it);
			std::string comma = std::next(it, 1) != attribute_set.end() ? ", " : "";
			std::cout << comma;

		}

		std::cout << " }";

	}

	// =========================================================================
	// Prints an AttributeSetClosure.
	//
	// "attribute_set_closure":
	//		Closure of an attribute set to print.
	// =========================================================================
	void Database::PrintAttributeSetClosure(const AttributeSetClosure & 
		attribute_set_closure) {

		PrintAttrSet(func_dep_table[attribute_set_closure.first].first);
		std::cout << " -> ";
		PrintAttrSet(attribute_set_closure.second);

	}

	// =========================================================================
	// Prints this database's attribute table.
	// =========================================================================
	void Database::PrintAttrTable() {

		std::cout << "{ ";

		for (AttributeTable::const_iterator it = attribute_table.begin();
			it != attribute_table.end(); it++) {

			PrintAttribute(*it);
			std::string comma = std::next(it, 1) != attribute_table.end() ? ", " : "";
			std::cout << comma;

		}

		std::cout << " }";

	}

	// =========================================================================
	// Prints a CandidateKeyList.
	//
	// "ck_list":
	//		List of candidate keys to print.
	// =========================================================================
	void Database::PrintCandidateKeyList(const CandidateKeyList & ck_list) {

		for (CandidateKeyList::const_iterator it = ck_list.begin();
			it != ck_list.end(); it++) {

			PrintAttrSet(*it);
			std::cout << "\n";

		}

	}

	// =========================================================================
	// Prints a FuncDep.
	//
	// "func_dep":
	//		The functional dependency to print.
	// =========================================================================
	void Database::PrintFuncDep(const FuncDep & func_dep) {

		PrintAttrSet(func_dep.first);
		std::cout << " -> ";
		PrintAttrSet(func_dep.second);

	}

	// =========================================================================
	// Prints a FuncDepSetClosure.
	// 
	// "func_dep_set_closure":
	//		Closure of set of functional dependencies to print.
	// =========================================================================
	void Database::PrintFuncDepSetClosure(const FuncDepSetClosure &
		func_dep_set_closure) {

		for (FuncDepSetClosure::const_iterator it = func_dep_set_closure.begin();
			it != func_dep_set_closure.end(); it++) {

			PrintAttributeSetClosure(*it);
			std::cout << "\n";

		}

	}

	// =========================================================================
	// Prints this database's functional dependency table.
	// =========================================================================
	void Database::PrintFuncDepTable() {

		for (FuncDepTable::const_iterator it = func_dep_table.begin();
			it != func_dep_table.end(); it++) {

			PrintFuncDep(*it);
			std::cout << "\n";

		}

	}

	// =========================================================================
	// Prints this database's name.
	// =========================================================================
	void Database::PrintName() {

		std::cout << name;

	}

	// =========================================================================
	// Prints a Relation.
	// 
	//	"relation":
	//		The relation to print.
	// =========================================================================
	void Database::PrintRelation(const Relation & relation) {

		std::cout
			<< "========================================\n"
			<< relation.name << "\n"
			<< "========================================\n";

		std::cout << "\nAttributes:\n";
		PrintAttrSet(relation.attributes);
		std::cout << "\n\nPrimary Key:\n";
		PrintAttrSet(relation.primary_key);
		std::cout << "\n\nCandidate Keys:\n";
		PrintCandidateKeyList(relation.candidate_keys);
		std::cout << "\nClosure:\n";
		PrintFuncDepSetClosure(relation.closure);

	}

	// =========================================================================
	// Prints this database's relation table.
	// =========================================================================
	void Database::PrintRelationTable() {

		for (RelationTable::const_iterator it = relation_table.begin();
			it != relation_table.end(); it++) {

			PrintRelation(*it);
			std::cout << "\n";

		}

	}

	// =========================================================================
	// Removes all relations in relation_table and pushes them to 
	// normalization_queue for normalization step.
	//
	// "normalization_queue":
	//		Queue used to enqueue relations for normalization.
	//
	// Side effects:
	//		Clears private member "relation_table".
	// =========================================================================
	void Database::QueuePreNormalizedRelations(NormalizationQueue & 
		normalization_queue) {

		for (RelationTable::iterator it = relation_table.begin();
			it != relation_table.end(); it = relation_table.erase(it)) {

			normalization_queue.push(*it);

		}


	}

	// =========================================================================
	// Helper function called by SingleThreaded2nf() and MultiThreaded2nf()
	// to normalize a specific relation to 2NF.
	//
	// Returns a RelationTable, i.e., a collection of decomposed relations 
	// as part of 2NF normalization.
	// =========================================================================
	RelationTable Database::RelationTo2nf(Relation & relation) {

		RelationTable decomposed_relations;

		decomposed_relations.push_back(relation);

		if (relation.primary_key.size() == 1)
			return decomposed_relations;

		// Iterate over relation's closures.
		for (auto relation_closure_it = relation.closure.begin();
			relation_closure_it != relation.closure.end();
			relation_closure_it++) {

			if (IsPartialPrimaryKey(func_dep_table[relation_closure_it->first].first,
				relation)) {

				// The lhs of the current closure is a partial
				// primary key.

				Relation decomposed_relation;
				bool decomposed = false;

				// Iterate over the rhs of the closure.
				for (Rhs::iterator closure_rhs_it = relation_closure_it->second.begin();
					closure_rhs_it != relation_closure_it->second.end();
					closure_rhs_it++) {

					// Determine if rhs of closure is in lhs of functional dependency.
					Lhs &func_dep_lhs = func_dep_table[relation_closure_it->first].first;

					bool is_in_lhs = func_dep_lhs.find(*closure_rhs_it)
						!= func_dep_lhs.end();

					if (!is_in_lhs && !attribute_table[*closure_rhs_it].prime) {

						// The rhs of the closure includes a
						// non-prime attribute, which is a violation
						// of 2NF. Decompose relation.

						if (!decomposed) {

							// Initialize decomposed relation.

							decomposed_relation.name = "r" + relation_num;
							relation_num++;

							// New relation's PK is the lhs of the closure.
							decomposed_relation.candidate_keys.push_back(
								CandidateKey(
									func_dep_table[relation_closure_it->first].first.begin(),
									func_dep_table[relation_closure_it->first].first.end()
								)
							);

							AssignPrimaryKey(decomposed_relation);

							// Add PK to new relation's attributes.
							decomposed_relation.attributes.insert(
								func_dep_table[relation_closure_it->first].first.begin(),
								func_dep_table[relation_closure_it->first].first.end());

							decomposed = true;

						}

						decomposed_relation.attributes.insert(*closure_rhs_it);
						decomposed_relations.front().attributes.erase(*closure_rhs_it); // [todo]

					}

				} // ... Iterate over the rhs of the closure.

				if (decomposed)
					decomposed_relations.push_back(decomposed_relation);

			} // ... if (IsPartialPrimaryKey) ...

		} // ... Iterate over relation's closures.

		return decomposed_relations;

	}

	// =========================================================================
	// Helper function called by NormalizeTo3nf() to normalize a specific
	// relation to 3NF.
	//	
	//	"relation":
	//		Relation to normalize to 3NF.
	//
	// Returns a RelationTable, i.e., a collection of decomposed relations 
	// as part of 3NF normalization.
	// =========================================================================
	RelationTable Database::RelationTo3nf(Relation & relation) {
	
	
		return RelationTable();
	
	
	}

	// =========================================================================
	// Sets name of database.
	//
	// "_name":
	//		String used to set database's name.
	//
	// Side effects: 
	//		Sets private data member "name" to "_name".
	// =========================================================================
	void Database::SetName(const std::string & _name) {
		name = _name;
	}

	// =========================================================================
	// Single-threaded 2NF.
	// =========================================================================
	void Database::SingleThreaded2nf(NormalizationQueue & 
		normalization_queue) {

		while (!normalization_queue.empty())
		{
			Relation &next = normalization_queue.front();
			normalization_queue.pop();
			RelationTable decomposed_relations = RelationTo2nf(next);

			if (decomposed_relations.size() < 1) {
				throw std::runtime_error("Internal error. 2NF should never return 0 decomposed relations.");
			}
			else if (decomposed_relations.size() == 1) {
				relation_table.push_back(decomposed_relations.front());
			}
			else {

				for (RelationTable::iterator it = decomposed_relations.begin();
					it != decomposed_relations.end(); it = decomposed_relations.erase(it)) {

					normalization_queue.push(*it);

				}
			}

		}

	}

	// =========================================================================
	// Single-threaded 3NF.
	// =========================================================================
	RelationTable Database::SingleThreaded3nf(NormalizationQueue &
		normalization_queue) {

		return RelationTable();

	}

}