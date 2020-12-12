#include "OptionScreen.h"

using namespace std;

// Constructor. Sets up the content of the Option screen (consisting of Text and Buttons)
OptionScreen::OptionScreen(GameSettings& game_settings) : back_btn("< save and back", game_settings.getFont(), 40)	// member initializer list: back_btn
{
	settings = &game_settings;	// save the Address of game_settings in a pointer
	string optn_val;

	options_text_descr[HI_SCORE_TXT].setString("Hi-Score");
	options_text_val[HI_SCORE_TXT].setString(to_string(settings->getHiScore()));
	options_text_descr[BOUNDARY_TXT].setString("Playfield Boundary");
	settings->getBoundaryID(&optn_val);
	options_text_val[BOUNDARY_TXT].setString(optn_val);
	options_text_descr[FONT_TXT].setString("Font");
	settings->getFontID(&optn_val);
	options_text_val[FONT_TXT].setString(optn_val);
	options_text_descr[NUM_WORDS_TEXT].setString("Number of Words");
	options_text_val[NUM_WORDS_TEXT].setString(to_string(settings->getNumWordsSpawn()));

	for (unsigned int i = 0; i < NUM_TEXTS; i++)
	{
		options_text_descr[i].setFont(settings->getFont());
		options_text_descr[i].setCharacterSize(30);
		options_text_descr[i].setStyle(sf::Text::Bold);
		options_text_descr[i].setLetterSpacing(1.5);

		options_text_val[i].setFont(settings->getFont());
		options_text_val[i].setCharacterSize(30);
		options_text_val[i].setStyle(sf::Text::Bold);
		options_text_val[i].setLetterSpacing(1.5);
	}

	for (unsigned int i = 0; i < NUM_BUTTONS; i++)
	{
		options_btn_left[i].setFont(game_settings.getFont());
		options_btn_left[i].setString("<");
		options_btn_left[i].setCharacterSize(40);

		options_btn_right[i].setFont(game_settings.getFont());
		options_btn_right[i].setString(">");
		options_btn_right[i].setCharacterSize(40);
	}

	update_positions();
	back_btn.setPosition(sf::Vector2f(20.f, 20.f));
}

inline OptionScreen::~OptionScreen() {}	// virtual destructor

// set the position of every object of the Option Screen
void OptionScreen::update_positions()
{
	// the following variables define the layout of the Option Screen. in pixels
	float margin_vert = 80;
	float margin_hor = 30;
	float text_right_margin_hor = margin_hor + 60;
	float btn_right_margin_hor = margin_hor + 350;
	float vert_pos_start = 150;
	float vert_pos_cur = vert_pos_start;
	float text_pos_diff = 0;

	// set up the text
	for (unsigned int i = 0; i < NUM_TEXTS; i++)
	{
		text_pos_diff = options_text_descr[i].getGlobalBounds().top - options_text_descr[i].getPosition().y;
		options_text_descr[i].setPosition(settings->get_window_size().x / 2 - options_text_descr[i].getGlobalBounds().width - margin_hor,
			vert_pos_cur - (text_pos_diff + options_text_descr[i].getGlobalBounds().height / 2));

		text_pos_diff = options_text_val[i].getGlobalBounds().top - options_text_val[i].getPosition().y;
		options_text_val[i].setPosition(settings->get_window_size().x / 2 + text_right_margin_hor, vert_pos_cur - (text_pos_diff + options_text_val[i].getGlobalBounds().height / 2));

		vert_pos_cur += margin_vert;
	}

	// set up the Buttons
	vert_pos_cur = vert_pos_start + margin_vert;	// no Buttons needed for the first Option (Hi-Score)
	for (unsigned int i = 0; i < NUM_BUTTONS; i++)
	{
		options_btn_left[i].setPosition(sf::Vector2f(settings->get_window_size().x / 2 + margin_hor, vert_pos_cur - options_btn_left[i].getSize().y / 2));
		options_btn_right[i].setPosition(sf::Vector2f(settings->get_window_size().x / 2 + btn_right_margin_hor, vert_pos_cur - options_btn_right[i].getSize().y / 2));

		vert_pos_cur += margin_vert;
	}
}

inline void OptionScreen::update() {}

inline void OptionScreen::update_physics() {}

// draw every button and text on the screen
inline void OptionScreen::draw_on_window(sf::RenderWindow& window)
{
	back_btn.draw_on_window(window);
	for (unsigned int i = 0; i < NUM_TEXTS; i++)
	{
		window.draw(options_text_descr[i]);
		window.draw(options_text_val[i]);
	}
	for (unsigned int i = 0; i < NUM_BUTTONS; i++)
	{
		options_btn_left[i].draw_on_window(window);
		options_btn_right[i].draw_on_window(window);
	}
}

inline void OptionScreen::key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt) {}

// implement the functionality of every Button
inline void OptionScreen::mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt)
{
	// back Button: save every setting in the settings file and go back to the Start Screen
	back_btn.mouse_clicked_processor(pressed_mouse_evnt);
	if (back_btn.is_button_pressed())
	{
		settings->saveFontID();
		settings->saveBoundaryID();
		settings->saveNumWordsSpawn();
		settings->game_state = GameSettings::START_SCREEN;
		back_btn.button_pressed_reset();
	}

	options_btn_left[BOUNDARY_BTN].mouse_clicked_processor(pressed_mouse_evnt);
	options_btn_right[BOUNDARY_BTN].mouse_clicked_processor(pressed_mouse_evnt);
	if (options_btn_left[BOUNDARY_BTN].is_button_pressed() || options_btn_right[BOUNDARY_BTN].is_button_pressed())
	{
		int new_bound = settings->getBoundaryID();
		if (options_btn_left[BOUNDARY_BTN].is_button_pressed())
			new_bound--;
		else if (options_btn_right[BOUNDARY_BTN].is_button_pressed())
			new_bound++;

		if (new_bound >= GameSettings::NUM_BOUNDS)
			new_bound = 0;
		else if (new_bound < 0)
			new_bound = settings->NUM_BOUNDS - 1;

		string bound_type;
		settings->setBoundaryID(new_bound);
		settings->getBoundaryID(&bound_type);
		options_text_val[BOUNDARY_TXT].setString(bound_type);

		options_btn_left[BOUNDARY_BTN].button_pressed_reset();
		options_btn_right[BOUNDARY_BTN].button_pressed_reset();
	}

	options_btn_left[FONT_BTN].mouse_clicked_processor(pressed_mouse_evnt);
	options_btn_right[FONT_BTN].mouse_clicked_processor(pressed_mouse_evnt);
	if (options_btn_left[FONT_BTN].is_button_pressed() || options_btn_right[FONT_BTN].is_button_pressed())
	{
		int new_font = settings->getFontID();
		if (options_btn_left[FONT_BTN].is_button_pressed())
			new_font--;
		else if (options_btn_right[FONT_BTN].is_button_pressed())
			new_font++;

		if (new_font >= GameSettings::NUM_FONTS)
			new_font = 0;
		else if (new_font < 0)
			new_font = settings->NUM_FONTS - 1;

		string font_type;
		settings->setFont(new_font);
		settings->getFontID(&font_type);
		options_text_val[FONT_TXT].setString(font_type);
		// update all Buttons to adjust to the new font
		back_btn.update();
		for (unsigned int i = 0; i < NUM_BUTTONS; i++)
		{
			options_btn_left[i].update();
			options_btn_right[i].update();
		}
		// update the Position of all Elements on the Screen to adjust to the new font
		update_positions();

		options_btn_left[FONT_BTN].button_pressed_reset();
		options_btn_right[FONT_BTN].button_pressed_reset();
	}

	options_btn_left[NUM_WORDS_BTN].mouse_clicked_processor(pressed_mouse_evnt);
	options_btn_right[NUM_WORDS_BTN].mouse_clicked_processor(pressed_mouse_evnt);
	if (options_btn_left[NUM_WORDS_BTN].is_button_pressed() || options_btn_right[NUM_WORDS_BTN].is_button_pressed())
	{
		int new_num_words_spawn = settings->getNumWordsSpawn();
		if (options_btn_left[NUM_WORDS_BTN].is_button_pressed())
			new_num_words_spawn--;
		else if (options_btn_right[NUM_WORDS_BTN].is_button_pressed())
			new_num_words_spawn++;

		if (new_num_words_spawn > GameSettings::MAX_NUM_WORDS)
			new_num_words_spawn = GameSettings::MIN_NUM_WORDS;
		else if (new_num_words_spawn < GameSettings::MIN_NUM_WORDS)
			new_num_words_spawn = GameSettings::MAX_NUM_WORDS;

		settings->setNumWordsSpawn(new_num_words_spawn);
		options_text_val[NUM_WORDS_TEXT].setString(to_string(new_num_words_spawn));

		options_btn_left[NUM_WORDS_BTN].button_pressed_reset();
		options_btn_right[NUM_WORDS_BTN].button_pressed_reset();
	}
}
