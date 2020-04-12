#ifndef _CSVPARSER_HPP_
#define _CSVPARSER_HPP_

#include <fstream>


// reads from a csv file. Used to obtain random words for the Playfield
// the csv file parsing is not really accurate, because it doesn't handle quotation marks here
class CSVParser
{
public:
	char delimiter;				// delimiter which separates the values in the file
	unsigned int num_elem;		// number of Elements in the opened file

	CSVParser(const std::string& csv_filename = "", char csv_delimiter = ';');
	CSVParser& operator = (const CSVParser& csvparser_orig);
	CSVParser(const CSVParser& csvparser_orig);

	std::string get_random_elem();

private:
	std::string filename;	// path of the opened file
	std::ifstream fin;		// File pointer. open file for reading
};

#endif // _CSVPARSER_HPP_