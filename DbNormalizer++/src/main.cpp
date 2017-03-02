#include <iostream>
#include <string>

#include "database.h"
#include "txtparser.h"

#define EMP_PROJ_PATH "C:\\vs_projects\\DbNormalizer++\\DbNormalizer++\\emp_proj.txt"

// ============================================================================= 
// TODO:
//	*	Add line numbers to TxtParser.
//	*	Expose function in IParser or Database for subclasses to use to easily 
//		parse/insert.
//	*	Functional dependencies (convert from 'A,B->C' to FuncDep?)
//	*	Consider if an attribute appears on the rhs of a functional dependency
//		more than once.
//	*	Consider artificial keys if no natural PK exists. in AssignPrimaryKey(),
//		must add the necessary functional depedency?
//	*	Add types to input files.
//	*	Add AttributeType to Attribute struct.
// =============================================================================

int main()
{
	using namespace DbNormalizerCpp;

	Database db;

	{

		TxtParser parser;
		parser.Open(EMP_PROJ_PATH);
		db = parser.Parse(); 

	}	// Discard parser; it is not needed anymore.

	db.NormalizeTo2nf();
	db.Print();

	return 0;
}

