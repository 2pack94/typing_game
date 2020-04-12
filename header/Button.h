#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include "Entity.h"


// The Button class inherits from Entity
// The button conists of a Text and a Rectangle Shape. It reacts on mouse clicks.
// The button gets resized through the Character size.
// The functionality of the button when its clicked must be defined externally.
class Button : public Entity
{
public:
	Button(const sf::String& string = "", const sf::Font& font = sf::Font(), unsigned int characterSize = 30);
	virtual ~Button();

	void setPosition(const sf::Vector2f& position);
	const sf::Vector2f& getPosition();
	const sf::Vector2f& getSize();
	void setString(const std::string& string_in);
	void setCharacterSize(unsigned int char_size);
	void setFont(const sf::Font& font);
	bool is_mouse_on_button(const sf::Vector2f mouse_pos);
	bool is_button_pressed();
	void button_pressed_reset();

	virtual void update();
	virtual void update_physics();
	virtual void key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt);
	virtual void mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt);
	virtual void draw_on_window(sf::RenderWindow& window);

private:
	float margin;					// margin between the text and the outline of the button on every side. in pixels
	bool button_pressed;			// flag that gets set to true when the button is clicked, has to be reset manually afterwards
	sf::Color idle_color;			// color of the rectange and text when the button is idle
	sf::Color active_color;			// color of the rectange and text when the mouse hovers over the button
	sf::RectangleShape btn_shape;	// rectangle shape of the button
	sf::Text btn_text;				// text of the button
};

#endif // _BUTTON_HPP_