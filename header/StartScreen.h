#ifndef _STARTSCREEN_HPP_
#define _STARTSCREEN_HPP_

#include "Entity.h"
#include "Button.h"
#include "GameSettings.h"


// The Start Screen gets shown on the start of the Program. There is the name of the game, a start button, an options button and an exit button
// The class StartScreen inherits from Entity
class StartScreen : public Entity
{
public:
	StartScreen(GameSettings& game_settings);
	virtual ~StartScreen();

	virtual void update();
	virtual void update_physics();
	virtual void draw_on_window(sf::RenderWindow& window);
	virtual void key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt);
	virtual void mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt);

private:
	Button start_btn, exit_btn, options_btn;	// Buttons to navigate to different Screens or exit
	sf::Text status_msg;						// used to display a information on the screen (currently only used for displaying the settings file state)
	sf::Text game_title;						// title of the game
	GameSettings* settings;						// pointer to the game settings
};

#endif // _STARTSCREEN_HPP_