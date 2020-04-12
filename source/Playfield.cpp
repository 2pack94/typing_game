#define _USE_MATH_DEFINES
#include <math.h>
#include <mutex>
#include "Playfield.h"

using namespace std;

extern mutex mutex_glob;	// defined in main.cpp

// Constructor 
template <typename T>
Playfield<T>::Playfield(GameSettings& game_settings) : back_btn("<", game_settings.getFont(), 47), restart_btn("RESTART", game_settings.getFont(), 40), word_list_csv(game_settings.wordlist_csv_filename, game_settings.csv_delimiter)	// member initializer list. Initialize the Buttons and the CSVParser
{
	// if a non supported template type for playfield would be used, the program wouldn't compile

	if (!side_panel_texture.loadFromFile("resources/textures/Sidepanel.png"))
		throw - 1;

	settings = &game_settings;	// save the Address of game_settings in a pointer

	// define the boundary of the playfield
	boundary_size = 800;
	init_boundary(boundary);

	init_stats();

	// initialize the text in the side panel. set the position to fit the text inside its intended spot
	for (unsigned int i = 0; i < NUM_TEXTS; i++)
	{
		playfield_text[i].setFont(settings->getFont());
		playfield_text[i].setCharacterSize(30);
		playfield_text[i].setStyle(sf::Text::Bold);
		playfield_text[i].setLetterSpacing(1.5);
	}
	playfield_text[PLAYTIME].setPosition(35.f, 48.f);
	playfield_text[TYPED_WORDS].setPosition(35.f, 138.f);
	playfield_text[MISSED_WORDS].setPosition(35.f, 228.f);
	playfield_text[NEW_HI_SCORE].setPosition(35.f, 318.f);
	playfield_text[SCORE].setPosition(35.f, 362.f);

	// set the position of the Buttons
	back_btn.setPosition(sf::Vector2f(50.f, 730.f - back_btn.getSize().y / 2));
	restart_btn.setPosition(sf::Vector2f(120.f, 730.f - restart_btn.getSize().y / 2));
}

// virtual Destructor. delete all allocated memory
template <typename T>
inline Playfield<T>::~Playfield()
{
	for (list<Word*>::iterator word_list_it = word_list.begin(); word_list_it != word_list.end(); word_list_it++)	// iterator is used to point at the Elements of the list
	{
		delete* word_list_it;	// free memory that is pointed to by the list-Element (dereference of iterator gives the list Element)
	}
}

// follow the rule of three: when one of the following is manually defined, define all of them manually: destructor, copy constructor and copy assignment operator
// manually define copy constructor and copy assignment operator. the automatically created default copy constructor and copy assignment operator do a member-wise copy between objects. So when a pointer member is copied, only a shallow copy is made (pointer points on the same memory location instead of creating its own memory for the new object)

// copy assignment operator. is called when an already initialized object is assigned a new value from another existing object
// playfield_orig: input. right of the '='. Class object which shall be copied to the new object
// return: the new Playfield object left of the '='
template <typename T>
Playfield<T>& Playfield<T>::operator = (const Playfield<T>& playfield_orig)
{
	settings = playfield_orig.settings;		// make a shallow copy of the pointer
	playtime = playfield_orig.playtime;
	max_playtime = playfield_orig.max_playtime;
	typed_words = playfield_orig.typed_words;
	missed_words = playfield_orig.missed_words;
	score = playfield_orig.score;
	boundary_size = playfield_orig.boundary_size;
	game_running = playfield_orig.game_running;
	clock = playfield_orig.clock;
	word_list_csv = playfield_orig.word_list_csv;
	back_btn = playfield_orig.back_btn;
	restart_btn = playfield_orig.restart_btn;
	side_panel_texture = playfield_orig.side_panel_texture;
	boundary = playfield_orig.boundary;
	collision_cnt = playfield_orig.collision_cnt;

	// no simple assignment is possible for the following members
	for (unsigned int i = 0; i < NUM_TEXTS; i++)
		playfield_text[i] = playfield_orig.playfield_text[i];

	// create a new word_list with its own allocated memory for the new Playfield object
	for (auto word_list_it = playfield_orig.word_list.begin(); word_list_it != playfield_orig.word_list.end(); word_list_it++)	// iterator is used to point at the Elements of the list
	{
		Word* new_word = new Word;		
		*new_word = **word_list_it;		// copy the content of the word object
		word_list.push_back(new_word);	// insert in the word_list of the new object
	}

	return *this;	// after assigning every member of the object left from the operator, return this object
}

// copy constructor. is called when a new object is created from an existing object, as a copy of the existing object
// playfield_orig: input. right of the '='. Class object which shall be copied to the new object
template <typename T>
Playfield<T>::Playfield(const Playfield& playfield_orig)
{
	*this = playfield_orig;		// use the already defined copy assignment operator.
}

// set up the RectangleShape boundary for the playfield
// bound: output. Reference to boundary to set up
template <typename T>
inline void Playfield<T>::init_boundary(sf::RectangleShape& bound)
{
	bound.setSize(sf::Vector2f(boundary_size, boundary_size));
	bound.setPosition((settings->get_window_size().x / 2) - (bound.getSize().x / 2) + (side_panel_texture.getSize().x / 2), (settings->get_window_size().y / 2) - (bound.getSize().y / 2));	// move to the center
	bound.setOutlineColor(sf::Color::White);
	bound.setFillColor(sf::Color::Transparent);
	bound.setOutlineThickness(2.f);
}

// set up the CircleShape boundary for the playfield
// bound: output. Reference to boundary to set up
template <typename T>
inline void Playfield<T>::init_boundary(sf::CircleShape& bound)
{
	bound.setRadius(boundary_size / 2);
	bound.setOrigin(bound.getRadius(), bound.getRadius());	// set origin to the center of the circle
	bound.setPosition(settings->get_window_size().x / 2 + (side_panel_texture.getSize().x / 2), settings->get_window_size().y / 2);	// set the center of the circle to be in the center of the playfield
	bound.setOutlineColor(sf::Color::White);
	bound.setFillColor(sf::Color::Transparent);
	bound.setOutlineThickness(2.f);
}

// set game statistics to their starting values. update the text on the side panel accordingly
template <typename T>
inline void Playfield<T>::init_stats()
{
	max_playtime = 90;
	playtime = max_playtime;
	typed_words = 0;
	missed_words = 0;
	score = 0;
	game_running = true;

	playfield_text[TYPED_WORDS].setString(to_string(typed_words));
	playfield_text[MISSED_WORDS].setString(to_string(missed_words));
	playfield_text[SCORE].setString(to_string(score));
	playfield_text[NEW_HI_SCORE].setString("");
}

// set the Word to a random position inside the RectangleShape boundary and set a random word angle
// word: input/ output. Word object to set the position
// bound: input. boundary in which the word spawns
template <typename T>
void Playfield<T>::spawn_word(Word& word, const sf::RectangleShape& bound)
{
	sf::FloatRect word_rect = word.getGlobalBounds();
	// the position of word.getGlobalBounds() and the word itself is not the same! the position of the word itself includes a spacing on top of the word (to fit all possible characters), whereas the global boundary adjusts to the current string of the word
	float word_pos_diff = word_rect.top - word.getPosition().y;

	// get the area in which the word (in fact the word boundary) can spawn
	sf::Vector2f spawn_range_xy[2];
	// get top left point of spawn area
	spawn_range_xy[0].x = bound.getPosition().x + 1;	 // plus 1 pixel additional margin
	spawn_range_xy[0].y = bound.getPosition().y + 1;
	// get bottom right point of spawn area. The margin here needs to be the dimensions of the word (plus 1 pixel additional margin)
	spawn_range_xy[1].x = bound.getPosition().x + bound.getSize().x - word_rect.width - 1;
	spawn_range_xy[1].y = bound.getPosition().y + bound.getSize().y - word_rect.height - 1;

	if (spawn_range_xy[0].x >= spawn_range_xy[1].x || spawn_range_xy[0].y >= spawn_range_xy[1].y)	// if boundary too small for the word
		return;

	// set the position of the word
	sf::Vector2f word_pos;
	// first get the position of the up left point of spawn area
	word_pos.x = spawn_range_xy[0].x;
	word_pos.y = spawn_range_xy[0].y - word_pos_diff;	// account for the offset between word and word-boundary position
	// add a random amount inside the spawn range to the word position
	word_pos.x += rand() % (int)(spawn_range_xy[1].x - spawn_range_xy[0].x);
	word_pos.y += rand() % (int)(spawn_range_xy[1].y - spawn_range_xy[0].y);
	word.setPosition(word_pos);

	// set the starting angle of the Word
	double word_angle = (double)(rand() % (int)(2 * M_PI * 100)) / 100;	// get a random number between 0 and 2*pi with 2 decimals
	word.set_angle(word_angle);
}

// set the Word to a random position inside the CircleShape boundary and set a random word angle
// word: input/ output. Word object to set the position
// bound: input. boundary in which the word spawns
template <typename T>
void Playfield<T>::spawn_word(Word& word, const sf::CircleShape& bound)
{
	sf::FloatRect word_rect = word.getGlobalBounds();
	// the position of word.getGlobalBounds() and the word itself is not the same! the position of the word itself includes a spacing on top of the word (to fit all possible characters), whereas the global boundary adjusts to the current string of the word
	float word_pos_diff = word_rect.top - word.getPosition().y;

	// calculate the diagonal / 2 of word_rect (plus 1 pixel additional margin). the center of the word should be at least this far away from the boundary when spawning
	float margin = (sqrt(pow(word_rect.width, 2) + pow(word_rect.height, 2)) / 2) + 1;

	if (margin >= bound.getRadius())	// if boundary too small for the word
		return;

	// calulate a random spawning position in polar coordinates (seen from the center of the boundary)
	int spawn_distance = rand() % (int)(bound.getRadius() - margin);
	double spawn_angle = (double)(rand() % (int)(2 * M_PI * 100)) / 100;	// get a random number between 0 and 2*pi with 2 decimals

	// set the position of the word
	// convert into cartesian coordinates (seen from the global coordinate origin)
	sf::Vector2f word_pos;
	// get the position of the center of the boundary and place the middle of the word boundary on the same position
	word_pos.x = bound.getPosition().x - (word_rect.width / 2);
	word_pos.y = bound.getPosition().y - (word_rect.height / 2) - word_pos_diff;	// account for the offset between word and word-boundary position
	// add a random distance and angle
	word_pos.x += (float)(spawn_distance * cos(spawn_angle));
	word_pos.y += (float)(spawn_distance * sin(spawn_angle));
	word.setPosition(word_pos);

	// set the starting angle of the Word
	double word_angle = (double)(rand() % (int)(2 * M_PI * 100)) / 100;	// get a random number between 0 and 2*pi with 2 decimals
	word.set_angle(word_angle);
}

// check if the Word collides with the boundary and reflect the Word from the boundary if yes
// word: input/ output. Word object which can be reflected
// bound: input. boundary where the word shall be reflected to stay inside
template <typename T>
bool Playfield<T>::word_reflection(Word& word, const sf::RectangleShape& bound)
{
	sf::FloatRect word_rect = word.getGlobalBounds();
	sf::Vector2f corner_p[4];		// corner points of the Rectangle P0, P1, P2, P3
	// P0 P1
	// P2 P3
	unsigned int num_collisions = 0;							// number of boundary edges the word is colliding with
	bool is_collision_lrtb[4] = { false, false, false, false };	// is collision left right top bottom of the boundary
	double refl_angle = 0;										// the angle of the word after the collision
	double sounding_line_angle = 0;								// angle of the sounding line. the sounding line is orthogonal to the collision edge of the boundary and it points at the collision edge.

	// get corner points of the word rectangle
	corner_p[0].x = word_rect.left;
	corner_p[0].y = word_rect.top;
	corner_p[1].x = word_rect.left + word_rect.width;
	corner_p[1].y = word_rect.top;
	corner_p[2].x = word_rect.left;
	corner_p[2].y = word_rect.top + word_rect.height;
	corner_p[3].x = word_rect.left + word_rect.width;
	corner_p[3].y = word_rect.top + word_rect.height;

	// check which edges of the boundary the word is colliding with
	for (unsigned int i = 0; i < sizeof(corner_p) / sizeof(corner_p[0]); i++)
	{
		if (corner_p[i].x <= bound.getPosition().x)
			is_collision_lrtb[0] = true;
		if (corner_p[i].x >= bound.getPosition().x + bound.getSize().x)
			is_collision_lrtb[1] = true;
		if (corner_p[i].y <= bound.getPosition().y)
			is_collision_lrtb[2] = true;
		if (corner_p[i].y >= bound.getPosition().y + bound.getSize().y)
			is_collision_lrtb[3] = true;
	}

	// calculate the sounding line angle
	// if the word collides with two edges, the sounding line will be diagonally in between.
	if (is_collision_lrtb[0])
	{
		num_collisions++;
		sounding_line_angle += M_PI;
	}
	if (is_collision_lrtb[1])
	{
		num_collisions++;
		sounding_line_angle += 0;
	}
	if (is_collision_lrtb[2])
	{
		num_collisions++;
		sounding_line_angle += 1.5 * M_PI;
	}
	if (is_collision_lrtb[3])
	{
		num_collisions++;
		sounding_line_angle += 0.5 * M_PI;
	}
	if (!num_collisions)	// if no collision
		return false;

	sounding_line_angle = sounding_line_angle / num_collisions;	// get the angle in between, when there is a collision with two edges

	// get angle difference between the Word angle and the sounding line angle: this is the incidence angle which is the same as the reflection angle
	// alternative: calculate the angle between the velocity vector and the sounding line vector with the dot product
	refl_angle = sounding_line_angle - word.get_angle();

	// reflect the word. the new angle is the opposite angle plus the incidence angle (from the original velocity vector to the soundling line vector) plus the reflection angle (from the sounding line vector to the reflected velocity vector)
	word.set_angle(word.get_angle() + M_PI + 2 * refl_angle);

	// bounce some pixels from the boundary (along the new velocity vector). Because there is a chance that the word is still out of bounds after the reflection (then it would get stuck).
	word.move(word.get_velocity_vector() / (word.velocity * (float)0.75));

	return true;
}

// check if the Word collides with the boundary and reflect the Word from the boundary if yes
// word: input/ output. Word object which can be reflected
// bound: input. boundary where the word shall be reflected to stay inside
template <typename T>
bool Playfield<T>::word_reflection(Word& word, const sf::CircleShape& bound)
{
	sf::FloatRect word_rect = word.getGlobalBounds();
	sf::Vector2f corner_p[4];				// corner points of the Rectangle P0, P1, P2, P3
	// P0 P1
	// P2 P3
	float distance = 0;						// distance between a corner point of the word and center of circle boundary
	unsigned int num_collision_points = 0;	// number of collision points (corners of the word_rect that are colliding)
	sf::Vector2f collision_center(0, 0);	// center of the collision points
	double refl_angle = 0;					// the angle of the word after the collision
	double sounding_line_angle = 0;			// sounding line angle. sounding line vector: from origin of the circle to the collision center/ orthogonal to the tangent of the circle in the collision point

	// get corner points of the word rectangle
	corner_p[0].x = word_rect.left;
	corner_p[0].y = word_rect.top;
	corner_p[1].x = word_rect.left + word_rect.width;
	corner_p[1].y = word_rect.top;
	corner_p[2].x = word_rect.left;
	corner_p[2].y = word_rect.top + word_rect.height;
	corner_p[3].x = word_rect.left + word_rect.width;
	corner_p[3].y = word_rect.top + word_rect.height;

	// get number of collisions and calculate the center of all collision points. used to reflect the word later. Better than deciding for one corner point in case of more than one collision.
	for (unsigned int i = 0; i < sizeof(corner_p) / sizeof(corner_p[0]); i++)
	{
		distance = sqrt(pow(corner_p[i].x - bound.getPosition().x, 2) + pow(corner_p[i].y - bound.getPosition().y, 2));	// d = sqrt((x2-x1)^2+(y2-y1)^2)
		// if the distance between a point of the rect and the center of the circle is more than the radius of the circle, the rect collided with the circle
		if (distance >= bound.getRadius())
		{
			num_collision_points++;
			collision_center.x += corner_p[i].x;
			collision_center.y += corner_p[i].y;
		}
	}

	if (!num_collision_points)	// if no collision
		return false;

	collision_center.x = collision_center.x / num_collision_points;
	collision_center.y = collision_center.y / num_collision_points;

	// get the sounding line angle
	// tan(alpha) = opposite cathetus / adjacent cathetus
	// the opposite cathetus and adjacent cathetus are also the x and y components of the sounding line vector
	// the function atan2 takes the y and x component separately for y/x, so it can do the case distinction internally. The output range therefore is -pi...pi.
	sounding_line_angle = atan2((collision_center.y - bound.getPosition().y), (collision_center.x - bound.getPosition().x));

	// get angle difference between the Word angle and the sounding line angle: this is the incidence angle which is the same as the reflection angle
	// alternative: calculate the angle between the velocity vector and the sounding line vector with the dot product (but the acos function needed for that has only a output value range of 0...pi)
	refl_angle = sounding_line_angle - word.get_angle();

	// reflect the word. the new angle is the opposite angle plus the incidence angle (from the original velocity vector to the sounding line vector) plus the reflection angle (from the sounding line vector to the reflected velocity vector)
	word.set_angle(word.get_angle() + M_PI + 2 * refl_angle);

	// bounce some pixels from the boundary (along the new velocity vector). Because there is a chance that the word is still out of bounds after the reflection (then it would get stuck).
	word.move(word.get_velocity_vector() / (word.velocity * (float)0.75));

	return true;
}

// update, delete, create Words. manage the game time and stop the playthrough when the time is up
template <typename T>
inline void Playfield<T>::update()
{
	if (!game_running)
		return;

	// Update all words
	for (list<Word*>::iterator word_list_it = word_list.begin(); word_list_it != word_list.end(); word_list_it++)
	{
		(*word_list_it)->update();
	}

	// the following section needs to be protected by a mutex. Because the Elements of the word list could be used at the same time by another thread.
	mutex_glob.lock();	// lock the mutex if free or wait here and lock it when its free
	auto collision_cnt_it = collision_cnt.begin();	// get the fitting type automatically with auto

	// delete finished words
	for (list<Word*>::iterator word_list_it = word_list.begin(); word_list_it != word_list.end(); )	// iterator is used to point at the Elements of the list
	{
		// word_list_it is a pointer to a list Element which is a pointer to a word. To get a Word object, the iterator must be dereferenced 2 times.
		if ((*word_list_it)->get_state() != Word::word_state::ALIVE)
		{
			if ((*word_list_it)->get_state() == Word::word_state::TYPED)
			{
				typed_words++;
				score += (*word_list_it)->getString().getSize() * POINTS_PER_LETTER;	// get points for each letter of the typed word
			}
			else if ((*word_list_it)->get_state() == Word::word_state::DEAD)
			{
				missed_words++;
				score += POINTS_PER_MISS;	// subtract points from the score
				if (score < 0)				// don't get a negative total score
					score = 0;
			}

			playfield_text[TYPED_WORDS].setString(to_string(typed_words));
			playfield_text[SCORE].setString(to_string(score));
			playfield_text[MISSED_WORDS].setString(to_string(missed_words));

			Word* word_tmp = *word_list_it;					// store the current Element to still have a pointer on it after the erase from the list and to delete it afterwards
			word_list_it = word_list.erase(word_list_it);	// delete the Element from the list pointed by the iterator. return a new iterator with the updated list which points on the Element after the deleted one
			delete word_tmp;								// after the element is erased from the list, delete must still be called. delete calls the destructor and frees up the memory that was allocated by new
			collision_cnt_it = collision_cnt.erase(collision_cnt_it);
		}
		else
		{
			word_list_it++;			// if an Element is erased, the iterator is also incremented. So the incrementation can't be in the for-loop header.
			collision_cnt_it++;
		}
	}

	// create new words if there are less existing words than max_num_words
	unsigned int max_num_words = settings->getNumWordsSpawn();
	for (unsigned int i = word_list.size(); i < max_num_words; i++)	// fill the word list until the maximum number of words is reached
	{
		string word_string = word_list_csv.get_random_elem();
		float word_velo = 100;					// in pixel per second. velocity of the word moving across the screen
		// word_health = a * b^c * d + e. <d> is the number of letters of the word. with 1 word there is <a> health per letter. the health per letter gets multiplied <b>, but the more words are on the screen, the smaller the health increase per word gets (thats what the power of <c> is doing). <e> is additional health intended to give a reaction time
		double word_health = 0.5 * pow(max_num_words, 0.9) * word_string.size() + 1;
		Word* new_word = new Word(word_string, settings->getFont(), word_velo, (float)word_health);	// call the constructor and allocate memory. assign a pointer to the created object.

		// set random starting position (and angle) inside boundary
		spawn_word(*new_word, boundary);

		word_list.push_back(new_word);				// add the pointer new_word to the end of the list
		collision_cnt.push_back(0);
	}
	mutex_glob.unlock();	// release the mutex again

	playtime -= clock.restart().asSeconds();	// subtract the elapsed time from the playtime
	if (playtime <= 0)							// if game is over
	{
		if ((unsigned int)score > settings->getHiScore())
		{
			settings->setSaveHiScore((unsigned int)score);
			playfield_text[NEW_HI_SCORE].setString("a new hi-score!");
		}
		game_running = false;
	}
}

// compute the movement, collision and reflection of all Words on the Playfield
// runs in a seperate physics thread
template <typename T>
inline void Playfield<T>::update_physics()
{
	// the following section needs to be protected by a mutex. Because the Elements in the list could be altered by another thread at the same time
	mutex_glob.lock();	// lock the mutex if free or wait here and lock it when its free
	auto collision_cnt_it = collision_cnt.begin();
	bool collision_ret;

	for (list<Word*>::iterator word_list_it = word_list.begin(); word_list_it != word_list.end(); word_list_it++)
	{
		collision_ret = word_reflection(**word_list_it, boundary);
		if (!collision_ret)
			*collision_cnt_it = 0;		// set the collision number for this word to 0
		else
			*collision_cnt_it += (unsigned int)collision_ret;
		if (*collision_cnt_it >= 10)	// if one word has a certain amount of continous collisions, its out of bounds
		{
			spawn_word(**word_list_it, boundary);
		}

		(*word_list_it)->update_physics();

		collision_cnt_it++;
	}
	mutex_glob.unlock();	// release the mutex again
}

// draw every Element on the Screen
template <typename T>
inline void Playfield<T>::draw_on_window(sf::RenderWindow& window)
{
	// draw boundary
	window.draw(boundary);
	// draw words
	for (list<Word*>::iterator word_list_it = word_list.begin(); word_list_it != word_list.end(); word_list_it++)
		(*word_list_it)->draw_on_window(window);
	// draw buttons
	back_btn.draw_on_window(window);
	restart_btn.draw_on_window(window);
	// draw text
	playfield_text[PLAYTIME].setString(to_string((int)(playtime + 1)));	// display the int value + 1 of the playtime
	for (unsigned int i = 0; i < NUM_TEXTS; i++)
		window.draw(playfield_text[i]);
	// draw the side panel sprite, which is just the texture without any changes
	window.draw(sf::Sprite(side_panel_texture));
}

// call the key_pressed_processor for every word in the list. reset the writing index of all words that are not being typed
template <typename T>
inline void Playfield<T>::key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt)
{
	if (!game_running)
		return;

	unsigned int max_writing_index = 0;				// the maximum writing index of all words in the list

	for (list<Word*>::iterator word_list_it = word_list.begin(); word_list_it != word_list.end(); word_list_it++)	// iterator is used to point at the Elements of the list
	{
		(*word_list_it)->key_pressed_processor(pressed_key_evnt);

		// find out the maximum writing index
		if ((*word_list_it)->writing_index > max_writing_index)
			max_writing_index = (*word_list_it)->writing_index;
	}

	// set the writing index of all the words that don't have the maximum writing index to 0
	for (list<Word*>::iterator word_list_it = word_list.begin(); word_list_it != word_list.end(); word_list_it++)
	{
		if ((*word_list_it)->writing_index < max_writing_index)
			(*word_list_it)->writing_index = 0;
	}
}

// implement the functionality of the Buttons
template <typename T>
inline void Playfield<T>::mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt)
{
	back_btn.mouse_clicked_processor(pressed_mouse_evnt);
	if (back_btn.is_button_pressed())
	{
		settings->game_state = GameSettings::START_SCREEN;
		back_btn.button_pressed_reset();
	}

	restart_btn.mouse_clicked_processor(pressed_mouse_evnt);
	if (restart_btn.is_button_pressed())
	{
		mutex_glob.lock();	// lock the mutex if free or wait here and lock it when its free
		auto collision_cnt_it = collision_cnt.begin();
		// delete all words
		for (list<Word*>::iterator word_list_it = word_list.begin(); word_list_it != word_list.end(); )	// iterator is used to point at the Elements of the list
		{
			Word* word_tmp = *word_list_it;	// store the current Element to still have a pointer on it after the erase from the list and to delete it afterwards
			word_list_it = word_list.erase(word_list_it);	// delete the Element from the list pointed by the iterator. return a new iterator with the updated list which points on the Element after the deleted one
			delete word_tmp;	// after the element is erased from the list, delete must still be called. delete calls the destructor and frees up the memory that was allocated by new.
			collision_cnt_it = collision_cnt.erase(collision_cnt_it);
		}
		mutex_glob.unlock();	// release the mutex again

		init_stats();		// reset stats
		clock.restart();
	}
}

// explicitly instantiate all possible Playfield classes
// Necessary, if declaration and definition of a template class is not in the same file. Gives a linker Error otherwise.
// To create a class with a "filled in" template type from the template class (as it is used in main.ccp), the compiler needs to see both declaration and definition.
template class Playfield<sf::RectangleShape>;
template class Playfield<sf::CircleShape>;