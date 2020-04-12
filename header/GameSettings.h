#ifndef _GAMESETTINGS_HPP_
#define _GAMESETTINGS_HPP_

#include <fstream>
#include "Entity.h"


// reads and writes the game settings in a .bin file. Options and Hi-Score are saved in the file. uses a checksum to validate the integrity of the data.
// its also possible to play without a settings file (because it can't be created for some reason). But then no settings or Hi-Scores are saved.
class SettingsFileParser
{
public:
	typedef enum f_state
	{
		GOOD,
		CREATE_NEW,
		CREATE_NEW_FAIL
	} f_state_t;
	f_state_t file_state;

	enum p_bound	// enum to select the playfield boundary
	{
		RECT = 0,
		CIRC,
		NUM_BOUNDS
	};

	enum fonts		// enum to select the available fonts
	{
		ARIAL = 0,
		TIMES,
		CONSOLAS,
		OLDENGL,
		NUM_FONTS
	};

	enum num_words_range	// minimum and maximum amount of words on the playfield
	{
		MIN_NUM_WORDS = 1,
		MAX_NUM_WORDS = 10
	};

	SettingsFileParser(const std::string& settings_filename);

	int calculate_checksum();
	void init_file_content();
	void updateChecksum();
	int check_settings_file();
	void create_settings_file();
	unsigned int getHiScore();
	void setSaveHiScore(unsigned int score);
	void setBoundaryID(int boundary);
	void saveBoundaryID();
	int getBoundaryID(std::string* bound_descr = NULL);
	void setFontID(int font_identifier);
	void saveFontID();
	int getFontID(std::string* font_name = NULL);
	void setNumWordsSpawn(unsigned int max_n_words);
	void saveNumWordsSpawn();
	unsigned int getNumWordsSpawn();

private:
	std::fstream fp;			// File pointer. Open file for reading and writing
	std::string filename;	// path of the opened file

	struct filecontent	// every information that is saved in the file (the Order of the Elements of the struct is the same as they are written in the file)
	{
		unsigned int hi_score;
		char boundary_id;
		char font_id;
		unsigned int num_words_spawn;
		int checksum;
	} file_content;

	unsigned int expected_file_length;	// size of the file (and the sum of the Elements in struct filecontent). in bytes
};


// contains different information used by other classes. inherits from SettingsFileParser.
// most of the members can be set and get. every class that needs access to the members should store a pointer to the same GameSettings object, so the same information is shared across every class.
class GameSettings : public SettingsFileParser
{
public:
	typedef enum g_state	// current state of the game. The game state gives information about which objects shall be created and displayed on the screen.
	{
		START_SCREEN,
		OPTIONS_SCREEN,
		PLAY_SCREEN,
		EXIT
	} game_state_t;
	game_state_t game_state;

	std::string wordlist_csv_filename;	// path to the .csv file that contains the word list. used to supply the information to the CSVParser class
	char csv_delimiter;				// delimiter for the csv file

	GameSettings();

	const sf::Font& getFont();
	void setFont(int font_identifier);
	sf::Vector2f& get_window_size();

private:
	sf::Font font;					// font which is used by every text object of this program
	sf::Vector2f window_size;		// window size of the game
};

#endif // _GAMESETTINGS_HPP_