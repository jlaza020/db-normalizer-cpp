# DBNormalizer++ Design Document

## Database

`Database` is a central repository for all database-relation information. Name, 
attributes, and functional dependencies are all stored into this one class. This
class is the main one client applications will use. Operations like adding 
attributes and normalizing to 2NF-BCNF with multi-threading all takes place 
here.

### Example

	name			normal_form			table_index
	----------		---------------		-----------
	"emp_proj"		NormalForm::One		6

	attr_table		attr_index_map		func_dep_table			relation_table
	----------		--------------		----------------		--------------
	0: A1			"ssn" : 0			0: <{0}, {1}> 			R1
	1: A2			"name" : 1			1: <{2}, {3,4}>			R2
	2: A3			"pnumber" : 2		2: <{0, 2}, {5}>		R3
	3: A4			"pname" : 3
	4: A5			"ploc" : 4
	5: A6			"hrs" : 5

### Data Members
---
#### Name

`name` is just the name of the `Database` as an `std::string`. For example, 
`"emp_proj"`.

#### Normal Form

`normal_form` is just the current normalization of the `Database`. The value is
from an enumeration `NormalForm`. A 2NF `Database` would have a `normal_form` 
with a value of `NormalForm::Two`.

#### Attribute Table

`attribute_table` is a `std::vector<DbNormalizerCpp::Attribute>`. It is just a 
list of `Attribute` objects. It is the repository for all attribute information.

#### Next Attribute Table Index

`next_attribute_table_index` is the index at which the next attribute will be 
inserted into `attribute_table`.

#### Attribute Index Map

`attribute_index_map` is a `std::unordered_map<std::string, int>`. Basically, it
contains mappings between an attribute's name, e.g., `"ssn"`, and the index, 
e.g., `0`, for its corresponding `Attribute` object in the attribute table.

#### Functional Dependency Table

`func_dep_table` is a 
`std::vector<std::pair<std::unordered_set<int>, std::unordered_set<int>>>`. 
Wow, that's complicated! Basically, it is a table wherein lies functional
dependencies. Consider a table with just one functional dependency, 
`{ssn, pnumber} -> {hrs}`. This would be represented as `<{0, 2}, {5}>`. If the 
table had more than one functional dependency, there would be many like that one.

#### Relation Table

`relation_table` is a `std::vector<DbNormalizerCpp::Relation>`, which consists 
of decomposed relations created in the normalization functions.

### Member Functions
---
#### void SetName (const std::string &_name)

* Public member function.
* Sets name of database to `_name`.

#### void InsertAttribute (const std::string &attr_name)

* Public member function.
* Creates an `Attribute` object with name `attr_name`.
* Inserts the created `Attribute` into `relation_table`.

#### void InsertFuncDep (FuncDep &func_dep)

* Public member function.
* Constructs a functional dependency from `func_dep` (see `FuncDep` definition 
in source code - makes creation of functional dependencies easy) and inserts it
into `func_dep_table` (see type of `func_dep_table` in source code).

#### void QueuePreNormalizedRelations (std::queue<Relation> &normalization_queue)

* Private member function called by the `NormalizeToXnf()` member functions and
some helpers. * Removes all relations in `relation_table` and pushes them to 
`normalization_queue`.

#### void NormalizeTo2nf (unsigned int max_threads = 1) 

* Public member function.
* Calls `_NormalizeTo2nf(max_threads)`.

#### void _NormalizeTo2nf (unsigned int max_threads)

* Private member function called by `NormalizeTo2nf()`.
* Generates the global relation by calling `GenerateGlobalRelation()` and adding
it to `relation_table`.
* Pushes the global relation into a `normalization_queue` by calling 
`QueuePreNormalizedRelations(nromalization_queue)`.
* If `max_threads` is `1`, use the existing thread to normalize the `Database`
by calling `SingleThreaded2nf(normalization_queue)`. Otherwise, create 
`max_threads` threads and normalize by calling 
`MultiThreaded2nf(normalization_queue, max_threads)`.

#### std::vector\<Relation> SingleThreaded2nf (std::queue<Relation> &normalization_queue)

* Private member function called by `_NormalizeTo2nf()`.
* While `normalization_queue` is not empty:
	* Pop it and store the popped relation to `next`.
	* Normalize `next` to 2NF by calling `RelationTo2nf(next)`.
	* If `RelationTo2nf(next)` returns only 1 `Relation` (it is the same as 
	`next`, meaning `next` cannot be decomposed any further), add it to 
	`relation_table`. Otherwise, add the returned relations to 
	`normalization_queue`.

#### std::vector<Relation> RelationTo2nf (Relation &relation)

* 

## Relation 

A `Relation` is a struct that basically represents a table as in a relational
database. Each `Relation` has a collection of attributes (columns), a primary
key, candidate keys, and a closure. A `Relation` may be decomposed during a
normalization step into many `Relations`. The data kept in a `Relation` is
fully-dependent on the existence of a `Database` somewhere. Existence of a 
`Relation` does not make sense without the raw data existing in a central 
repository such as a `Database`. Since it is just a struct used to store data 
and is "dumb", it does not need any reference or pointer to such a repository. 
The repository will use this struct to store data into it itself. A client 
program should never have to create, modify, or delete a `Relation`.

### Example

R1:

	attributes						primary_key			candidate_keys
	------------------				-----------			--------------
	{0, 1, 2, 3, 4, 5}				{0, 2}				0: {0, 2}

	closure
	--------------------------
	0: <0, {0, 1}>					// {ssn}* = {ssn, name}
	1: <1, {2, 3, 4}>				// {pnumber}* = {pnumber, pname, ploc}
	2: <2, {0, 1, 2, 3, 4, 5}>		// {ssn, pnumber}* = {ssn, pnumber, ..., hrs}

R2 (one of R1's decomposed relations):

	attributes						primary_key			candidate_keys
	------------------				-----------			--------------
	{0, 1}							{0}					0: {0}

	closure
	--------------------------
	0: <0, {0, 1}>					// {ssn}* = {ssn, name}

### Data Members
---
#### Attributes

`attributes` is an `std::unordered_set<int>` that contains attribute table 
indexes for all attributes that this `Relation` has.

#### Primary Key

`primary_key` is an `std::unordered_set<int>` that contains attribute indexes 
for all attributes that are part of the primary key for this `Relation`.

#### Candidate Keys

`candidate_keys` is an `std::vector<std::unordered_set<int>>` that contains
candidate keys for this `Relation`. Each candidate key is a set of
attribute table indexes that are a part of the candidate key.

#### Closure

`closure` is an `std::vector<std::pair<int, std::unordered_set<int>>>` that 
contains closures for this `Relation`. The attributes on the left-hand side of
any element in `closure` must all appear in this relation's `attributes`. The
first of the `std::pair<int, std::unordered_set<int>>` is an index into the
functional dependency table. We use this index only to know what is the
left-hand side of functional dependency that we care about. 

The right-hand side is irrelevant, but we use the index to save memory since the
left-hand side, which is of importance, already exists in the table. The second
of the aforementioned pair are indexes of all attributes that are functionally
determined by the first in the pair. 
