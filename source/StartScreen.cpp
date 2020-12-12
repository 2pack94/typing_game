#include "StartScreen.h"

// Constructor
StartScreen::StartScreen(GameSettings& game_settings)
	// member initializer list. Initialize the Buttons
	: start_btn("START >", game_settings.getFont(), 40), exit_btn("< EXIT", game_settings.getFont(), 40), options_btn("OPTIONS", game_settings.getFont(), 40)
{
	settings = &game_settings;	// save the Address of game_settings in a pointer

	// set up Game title text
	game_title.setString("Typing Game");
	game_title.setFont(settings->getFont());
	game_title.setCharacterSize(40);
	game_title.setStyle(sf::Text::Bold);
	game_title.setLetterSpacing(1.5);
	game_title.setPosition(sf::Vector2f((settings->get_window_size().x / 2) - (game_title.getGlobalBounds().width / 2), 80.f));

	// set up the position of the buttons
	float button_spacing = 150;		// in pixel. vertical spacing between every Button (from middle to middle)
	start_btn.setPosition(sf::Vector2f((settings->get_window_size().x / 2) - (start_btn.getSize().x / 2),
		(settings->get_window_size().y / 2) - (button_spacing + (start_btn.getSize().y / 2))));
	options_btn.setPosition(sf::Vector2f((settings->get_window_size().x / 2) - (options_btn.getSize().x / 2),
		(settings->get_window_size().y / 2) - (options_btn.getSize().y / 2)));
	exit_btn.setPosition(sf::Vector2f((settings->get_window_size().x / 2) - (exit_btn.getSize().x / 2),
		(settings->get_window_size().y / 2) + button_spacing - (exit_btn.getSize().y / 2)));

	// set up the status message
	status_msg.setFont(settings->getFont());
	status_msg.setCharacterSize(30);
	status_msg.setStyle(sf::Text::Bold);
	status_msg.setLetterSpacing(1.5);
	switch (settings->file_state)
	{
	case GameSettings::CREATE_NEW:
		status_msg.setString("Creating new settings file.");
		settings->file_state = GameSettings::GOOD;			// set the file state to good after the query
		break;
	case GameSettings::CREATE_NEW_FAIL:
		status_msg.setString("No settings file can be created, saving not possible!");
		break;
	default:
		status_msg.setString("");
		break;
	}	
	// set Origin of Text to be on the bottom left of the word
	status_msg.setOrigin(status_msg.getPosition().x, status_msg.getPosition().y + status_msg.getGlobalBounds().height + (status_msg.getGlobalBounds().top - status_msg.getPosition().y));
	status_msg.setPosition(0, settings->get_window_size().y);	// move the bottom left of the word to the bottom of the window
}

inline StartScreen::~StartScreen() {}	// virtual destructor

inline void StartScreen::update() {}

inline void StartScreen::update_physics() {}

// draw every Start Screen Element on the window
inline void StartScreen::draw_on_window(sf::RenderWindow& window)
{
	window.draw(status_msg);
	window.draw(game_title);
	start_btn.draw_on_window(window);
	options_btn.draw_on_window(window);
	exit_btn.draw_on_window(window);
}

inline void StartScreen::key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt) {}

// implement the functionality of the Buttons
inline void StartScreen::mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt)
{
	start_btn.mouse_clicked_processor(pressed_mouse_evnt);
	if (start_btn.is_button_pressed())
	{
		settings->game_state = GameSettings::PLAY_SCREEN;
		start_btn.button_pressed_reset();
	}
	options_btn.mouse_clicked_processor(pressed_mouse_evnt);
	if (options_btn.is_button_pressed())
	{
		settings->game_state = GameSettings::OPTIONS_SCREEN;
		options_btn.button_pressed_reset();
	}
	exit_btn.mouse_clicked_processor(pressed_mouse_evnt);
	if (exit_btn.is_button_pressed())
	{
		settings->game_state = GameSettings::EXIT;
		exit_btn.button_pressed_reset();
	}
}
