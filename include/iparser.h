#pragma once

#include <fstream>
#include <string>

namespace DbNormalizerCpp {

	// =========================================================================
	// IParser class. IParser is an interface used to define the minimal
	// requirements for parsing a DbNormalizer++ file. The derived class can
	// parse any type of file: .txt, .json, .xml, network socket, etc. 
	// =========================================================================
	class IParser {

	public:

	// =========================================================================
	// Member Functions
	// =========================================================================

		// =====================================================================
		// Opens a file.
		// 
		// "filename":
		//		Name of the file to open.
		// =====================================================================
		virtual void Open(std::string fileName) { fileStream.open(fileName); }


		// =====================================================================
		// Parses the file. 
		// 
		// Precondition: A file has been opened.
		// 
		// Returns a database that is initialized with a name, universal set of
		// attributes, and a set of functional dependencies.
		// =====================================================================
		virtual Database Parse() = 0;


		// =====================================================================
		// Closes the file.
		//
		// Precondition: A file has been opened.
		// =====================================================================
		virtual void Close() { fileStream.close(); }

	protected:

	// =========================================================================
	// Data members
	// =========================================================================

		std::ifstream fileStream;			// File stream that contains the 
											// contents of the data to be 
											// parsed.

	};

}