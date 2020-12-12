#ifndef _OPTIONSCREEN_HPP_
#define _OPTIONSCREEN_HPP_

#include "Entity.h"
#include "Button.h"
#include "GameSettings.h"


// The OptionScreen inherits from Entity
// The Option Screen displays all Settings and the Hi-Score
class OptionScreen : public Entity
{
public:
	OptionScreen(GameSettings& game_settings);
	virtual ~OptionScreen();

	void update_positions();

	virtual void update();
	virtual void update_physics();
	virtual void key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt);
	virtual void mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt);
	virtual void draw_on_window(sf::RenderWindow& window);

private:
	enum Text_id	// defines an ID for every Text on the Screen
	{
		HI_SCORE_TXT = 0,
		BOUNDARY_TXT,
		FONT_TXT,
		NUM_WORDS_TEXT,
		NUM_TEXTS
	};

	enum Button_id	// defines an ID for every Button on the Screen
	{
		BOUNDARY_BTN = 0,
		FONT_BTN,
		NUM_WORDS_BTN,
		NUM_BUTTONS
	};

	// Buttons and text thats displayed on the Option Screen
	sf::Text options_text_descr[NUM_TEXTS];
	sf::Text options_text_val[NUM_TEXTS];
	Button back_btn;
	Button options_btn_left[NUM_BUTTONS];
	Button options_btn_right[NUM_BUTTONS];
	GameSettings* settings;	// pointer to the game settings
};

#endif // _OPTIONSCREEN_HPP_
