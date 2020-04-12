#include "GameSettings.h"

using namespace std;

// Constructor. Open the file pointer for reading and writing in binary mode
// check if file has the correct format and if the checksum is correct. If not create a new default file
// settings_filename: input. path of the settings file
SettingsFileParser::SettingsFileParser(const string& settings_filename) : fp(settings_filename, ios::binary | ios::in | ios::out)	// member initializer list: fp
{
	filename = settings_filename;
	file_state = GOOD;
	// sizeof(struct filecontent) doesn't return the size of the sum of the Elements (because it isn't packed), so the size of every Element must be added individually.
	expected_file_length = sizeof(file_content.hi_score) + sizeof(file_content.boundary_id) + sizeof(file_content.font_id) + sizeof(file_content.num_words_spawn) + sizeof(file_content.checksum);

	init_file_content();

	if (check_settings_file() < 0)
	{
		create_settings_file();
	}
}

// read out the file content byte-wise and add all bytes together to calculate the checksum
// return: calculated check sum
inline int SettingsFileParser::calculate_checksum()
{
	char byte = 0;
	int check_sum = 0;

	fp.seekg(0, fp.beg);	// set file pointer to the beginning of the file
	for (unsigned int i = 0; i < expected_file_length - sizeof(file_content.checksum); i++)
	{
		fp.read(&byte, sizeof(byte));
		check_sum += byte;
	}
	return check_sum;
}

// set all the Elements of struct filecontent to their defaults
inline void SettingsFileParser::init_file_content()
{
	file_content.hi_score = 0;
	file_content.boundary_id = RECT;
	file_content.font_id = ARIAL;
	file_content.num_words_spawn = MIN_NUM_WORDS;
	file_content.checksum = 0;
}

// calculate the check sum of the file and write it to the end of the file. update also the corresponding struct filecontent Element
// the checksum should always be updated after writing new content to the file
inline void SettingsFileParser::updateChecksum()
{
	file_content.checksum = calculate_checksum();
	fp.seekp((-1) * (int)sizeof(file_content.checksum), fp.end);	// use a negative offset from the end of the file to jump to the checksum position
	fp.write((char*)& file_content.checksum, sizeof(file_content.checksum));
	fp.flush();		// flush the output buffer, to send the buffer to the file
}

// check if all contents of the file are like they should
// read out every information in the file and save it into the struct filecontent
// return: -1 if file is not ok. 0 if no error
int SettingsFileParser::check_settings_file()
{
	if (!fp.good())	// check error state
		return -1;

	// check the size of the file
	fp.seekg(0, fp.end);
	streamoff file_size = fp.tellg();
	if (file_size != expected_file_length)
		return -1;

	// read out the file content
	fp.seekg(0, fp.beg);
	fp.read((char*)& file_content.hi_score, sizeof(file_content.hi_score));
	fp.read((char*)& file_content.boundary_id, sizeof(file_content.boundary_id));
	fp.read((char*)& file_content.font_id, sizeof(file_content.font_id));
	fp.read((char*)& file_content.num_words_spawn, sizeof(file_content.num_words_spawn));
	fp.read((char*)& file_content.checksum, sizeof(file_content.checksum));

	// check if every Element in the file is correct and inside its range
	if (calculate_checksum() != file_content.checksum)
		return -1;
	if (file_content.boundary_id >= NUM_BOUNDS || file_content.boundary_id < 0)
		return -1;
	if (file_content.font_id < 0 || file_content.font_id >= NUM_FONTS)
		return -1;
	if (file_content.num_words_spawn < MIN_NUM_WORDS || file_content.num_words_spawn > MAX_NUM_WORDS)
		return -1;

	file_state = GOOD;

	return 0;
}

// create a new settings file with default values. used when the file doesn't exist yet, or if the current file is corrupted.
void SettingsFileParser::create_settings_file()
{
	file_state = CREATE_NEW;

	fp.close();
	fp.open(filename, ios::binary | ios::out | ios::in | ios::trunc);	// open the file again in truncate mode to erase everything and create a new file if not already existing
	if (!fp.good())	// check error state
	{
		file_state = CREATE_NEW_FAIL;
		return;
	}

	// write the default file content to the file
	init_file_content();
	fp.write((char*)& file_content.hi_score, sizeof(file_content.hi_score));
	fp.write((char*)& file_content.boundary_id, sizeof(file_content.boundary_id));
	fp.write((char*)& file_content.font_id, sizeof(file_content.font_id));
	fp.write((char*)& file_content.num_words_spawn, sizeof(file_content.num_words_spawn));
	file_content.checksum = calculate_checksum();
	fp.write((char*)& file_content.checksum, sizeof(file_content.checksum));
	fp.flush();		// flush the output buffer, to send the buffer to the file
}

// returns the hi-score
unsigned int SettingsFileParser::getHiScore()
{
	return file_content.hi_score;
}

// set a new hi-score and save it into the file
// score: input. new hi-score to set
void SettingsFileParser::setSaveHiScore(unsigned int score)
{
	file_content.hi_score = score;

	if (!fp.good())	// check error state
		return;

	fp.seekp(0, fp.beg);		// set the position of the output stream filepointer. the hi-score is saved at the beginning of the file
	fp.write((char*)& file_content.hi_score, sizeof(file_content.hi_score));

	updateChecksum();
}

// set boundary_id in the struct filecontent
// boundary: input. boundary_id to set
void SettingsFileParser::setBoundaryID(int boundary)
{
	if (boundary < 0 || boundary >= NUM_BOUNDS)
		return;
	file_content.boundary_id = boundary;
}

// save boundary_id to the file
void SettingsFileParser::saveBoundaryID()
{
	if (!fp.good())	// check error state
		return;

	fp.seekp(sizeof(file_content.hi_score), fp.beg);		// set the position of the output stream filepointer. The first Parameter marks the offset from the second Parameter
	fp.write((char*)& file_content.boundary_id, sizeof(file_content.boundary_id));

	updateChecksum();
}

// returns boundary_id
// bound_descr: output. if not NULL, get a descriptive text to the returned boundary_id
int SettingsFileParser::getBoundaryID(string* bound_descr)
{
	if (bound_descr != NULL)
	{
		if (file_content.boundary_id == RECT)
			*bound_descr = "Rectangle";
		else if (file_content.boundary_id == CIRC)
			*bound_descr = "Circle";
		else
			*bound_descr = "";
	}

	return file_content.boundary_id;
}

// set font_id in the struct filecontent
// font_identifier: input. font_id to set
void SettingsFileParser::setFontID(int font_identifier)
{
	if (font_identifier < 0 || font_identifier >= NUM_FONTS)
		return;
	file_content.font_id = font_identifier;
}

// save font_id to the file
void SettingsFileParser::saveFontID()
{
	if (!fp.good())	// check error state
		return;

	fp.seekp(sizeof(file_content.hi_score) + sizeof(file_content.boundary_id), fp.beg);		// set the position of the output stream filepointer. The first Parameter marks the offset from the second Parameter
	fp.write((char*)& file_content.font_id, sizeof(file_content.font_id));

	updateChecksum();
}

// returns font_id
// font_name: output. if not NULL, get the name of the current font (which is the font filename)
int SettingsFileParser::getFontID(string* font_name)
{
	if (font_name != NULL)
	{
		if (file_content.font_id == ARIAL)
			*font_name = "arial";
		else if (file_content.font_id == TIMES)
			*font_name = "times";
		else if (file_content.font_id == CONSOLAS)
			*font_name = "consola";
		else if (file_content.font_id == OLDENGL)
			*font_name = "OLDENGL";
		else
			*font_name = "";
	}

	return file_content.font_id;
}

// set num_words_spawn in the struct filecontent
// max_n_words: input. num_words_spawn to set
void SettingsFileParser::setNumWordsSpawn(unsigned int max_n_words)
{
	if (max_n_words < MIN_NUM_WORDS || max_n_words > MAX_NUM_WORDS)
		return;
	file_content.num_words_spawn = max_n_words;
}

// save num_words_spawn to the file
void SettingsFileParser::saveNumWordsSpawn()
{
	if (!fp.good())	// check error state
		return;

	fp.seekp(sizeof(file_content.hi_score) + sizeof(file_content.boundary_id) + sizeof(file_content.font_id), fp.beg);		// set the position of the output stream filepointer. The first Parameter marks the offset from the second Parameter
	fp.write((char*)& file_content.num_words_spawn, sizeof(file_content.num_words_spawn));

	updateChecksum();
}

// returns num_words_spawn
unsigned int SettingsFileParser::getNumWordsSpawn()
{
	return file_content.num_words_spawn;
}


// Default constructor. Because the constructor of the parent class needs an Argument for its constructor (no default constructor), the constructor with its argument must be called here explicitly
GameSettings::GameSettings() : SettingsFileParser("resources/settings.bin")
{
	window_size.x = 1200;
	window_size.y = 800;
	setFont(getFontID());		// get the font id from the settings file and set the font
	game_state = START_SCREEN;	// set the game to its initial state
	wordlist_csv_filename = "resources/word_list.csv";
	csv_delimiter = ';';
}

// returns a constant reference to the font object of this class. This reference can be supplied to the text objects of this program. As long as a text object uses a reference to this font, the font object shall not be destroyed
const sf::Font& GameSettings::getFont()
{
	return font;
}

// load a new font according to its ID. set the new font also in the SettingsFileParser class (but not save it)
// when the font is loaded from the file, the changed font is applied to all objects that use the reference to this font object
void GameSettings::setFont(int font_identifier)
{
	setFontID(font_identifier);
	string font_path;
	getFontID(&font_path);
	font_path = "resources/fonts/" + font_path + ".ttf";	// insert the name of the font into the full path

	if (!font.loadFromFile(font_path))
		throw - 1;
}

// returns the window size of the program
sf::Vector2f& GameSettings::get_window_size()
{
	return window_size;
}