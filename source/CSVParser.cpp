#include <sstream>
#include "CSVParser.h"

using namespace std;

// default Constructor. opens the file and calculates the number of Elements in the file
CSVParser::CSVParser(const string& csv_filename, char csv_delimiter)
	// member initializer list. fin: open file for input. in the destructor of fin the file gets closed. the destructor is called automatically when this class is destroyed
	: fin(csv_filename)
{
	filename = csv_filename;
	delimiter = csv_delimiter;
	num_elem = 0;

	if (!fin.good())	// check error state
		return;

	string row, value;

	// get number of values (that are not empty) in the file
	// getline: Extracts characters from fin and stores them into row until the newline character is found. returns fin, which is in an error state when eof is reached
	while (getline(fin, row))
	{
		stringstream ss(row);	// convert row into a stream object
		while (getline(ss, value, delimiter))		// getline: Extracts characters from ss and stores them into value until the delimiter is found
		{
			if (value.size() != 0)	// if the value is not empty
				num_elem++;
		}
	}
}

// copy assignment operator. is called when an already initialized object is assigned a new value from another existing object
// csvparser_orig: input. right of the '='. Class object which shall be copied to the new object
// return: the new CSVParser object left of the '='
CSVParser& CSVParser::operator = (const CSVParser& csvparser_orig)
{
	filename = csvparser_orig.filename;
	delimiter = csvparser_orig.delimiter;
	num_elem = csvparser_orig.num_elem;
	// don't copy the filestream object, because its not trivial. manually open the file again instead
	if (!fin.is_open())
		fin.open(filename);

	return *this;	// after assigning every member of the object left from the operator, return this object
}

// copy constructor. is called when a new object is created from an existing object, as a copy of the existing object
// csvparser_orig: input. right of the '='. Class object which shall be copied to the new object
CSVParser::CSVParser(const CSVParser& csvparser_orig)
{
	*this = csvparser_orig;		// use the already defined copy assignment operator
}

// return a random value in the file as a string or "_default_" if failed
string CSVParser::get_random_elem()
{
	unsigned int rand_val;	// use a random number as the index of the returned value
	string row, value;

	if (num_elem == 0 || num_elem > RAND_MAX)	// if file does not exist (or no content) or too big
	{
		return "_default_";
	}

	rand_val = rand() % num_elem;		// note that the distribution of the output number isn't equal anymore when using modulo

	fin.clear();			// reset the error bits of the streamobject (to reset the eofbit which is set when EOF was reached)
	fin.seekg(0, fin.beg);	// set filepointer to the beginning of the file

	if (!fin.good())		// check error state
		return "_default_";

	// traverse the file until rand_val (in the same way as when counting the number of values in the file)
	for (unsigned int i = 0; i <= rand_val; )
	{
		getline(fin, row);		// Extracts characters from fin and stores them into row until the newline character is found
		stringstream ss(row);	// convert row into a stream object
		while (getline(ss, value, delimiter))		// getline: Extracts characters from ss and stores them into value until the delimiter is found
		{
			if (value.size() != 0)	// if the value is not empty
			{
				if (i == rand_val)
				{
					i++;
					break;	// leave the while (and also the for loop, because i is now out of range)
				}
				i++;
			}
		}
	}

	return value;
}
