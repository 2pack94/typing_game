#ifndef _PLAYFIELD_HPP_
#define _PLAYFIELD_HPP_

#include <list>
#include "Entity.h"
#include "GameSettings.h"
#include "CSVParser.h"
#include "Word.h"
#include "Button.h"


// The class Playfield inherits from Entity
// displays all the game statistics, a list of the Words on the field and the boundary. Has a restart and back button
// Template class. The type specifies the boundary of the playfield. Default type: sf::RectangleShape / supported types: sf::RectangleShape, sf::CircleShape
// because the supported types can't be handled in the same way and have different Member functions, there must be multiple functions defined of the same name that handle every supported type
// note: non-virtual Member Functions of e.g. sf::CircleShape can't be called by objects of the Template type T or of the parent type sf::Shape
template <typename T = sf::RectangleShape> class Playfield : public Entity
{
public:
	Playfield(GameSettings& game_settings);
	virtual ~Playfield();
	Playfield& operator = (const Playfield& playfield_orig);
	Playfield(const Playfield& playfield_orig);

	virtual void update();
	virtual void update_physics();
	virtual void draw_on_window(sf::RenderWindow& window);
	virtual void key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt);
	virtual void mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt);

private:
	enum Text_id	// defines an ID for every Text on the Screen
	{
		PLAYTIME = 0,
		TYPED_WORDS,
		SCORE,
		MISSED_WORDS,
		NEW_HI_SCORE,
		NUM_TEXTS
	};

	enum score_points		// defines the score points to get
	{
		POINTS_PER_MISS = -50,
		POINTS_PER_LETTER = 10,
	};

	GameSettings* settings;				// pointer to the game settings
	float playtime;						// in seconds. gets counted backwards from max_playtime as the game progresses
	float max_playtime;					// in seconds. defines the length of a game
	unsigned int typed_words;			// number of words typed in the playthrough. gets displayed on the screen
	unsigned int missed_words;			// number of missed words in the playthrough. gets displayed on the screen
	int score;							// current score points
	float boundary_size;				// in pixels. size of the boundary (diameter of circle or edge length of rectangle) where the Words are inside
	bool game_running;					// flag if the game is currently running (playtime not at zero)
	sf::Clock clock;					// The clock starts automatically after being constructed. used to count down playtime
	CSVParser word_list_csv;			// CSVParser object to get random words from a file
	Button back_btn, restart_btn;		// back and restart Button. the back button leads to the Start Screen. the restart Button resets the game statistics and restarts the game clock
	sf::Texture side_panel_texture;		// Texture on the left of the screen to hold the game statistics
	T boundary;							// boundary shape with template type
	sf::Text playfield_text[NUM_TEXTS];	// Game statistics on the left of the screen in Text form
	std::list<Word*> word_list;				// list to store pointer to Word objects that are registred on the Playfield. Typeparameter for the Template is a pointer on class Word
	std::list<unsigned int> collision_cnt;	// store the number of continous collisions for every word to detect if its out of bounds. This list shall follow the word list exactly

	void init_boundary(sf::RectangleShape& bound);
	void init_boundary(sf::CircleShape& bound);
	void init_stats();
	void spawn_word(Word& word, const sf::RectangleShape& bound);
	void spawn_word(Word& word, const sf::CircleShape& bound);
	bool word_reflection(Word& word, const sf::RectangleShape& bound);
	bool word_reflection(Word& word, const sf::CircleShape& bound);
};

// Define the supported types for the template
typedef Playfield<sf::RectangleShape> PlayfieldRect;
typedef Playfield<sf::CircleShape> PlayfieldCirlce;

#endif // _PLAYFIELD_HPP_