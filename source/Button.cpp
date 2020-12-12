#include "Button.h"

using namespace std;

// default constructor
// string: text of the button
// font: a valid font has to be provided in order for the button text to show
// characterSize: character size of the text in pixels
Button::Button(const sf::String& string, const sf::Font& font, unsigned int characterSize)
{
	button_pressed = false;
	margin = (float)characterSize / 3;
	idle_color = sf::Color(150, 150, 150, 255);
	active_color = sf::Color(255, 255, 255, 255);

	// set up the Text
	btn_text.setString(string);
	btn_text.setFont(font);
	btn_text.setCharacterSize(characterSize);
	btn_text.setStyle(sf::Text::Bold);
	btn_text.setLetterSpacing(1.5);
	btn_text.setFillColor(idle_color);
	btn_text.setOutlineColor(idle_color);

	// set up the Rectangle
	btn_shape.setFillColor(sf::Color::Transparent);
	btn_shape.setOutlineColor(idle_color);
	btn_shape.setOutlineThickness(2);

	// set up the size of the rectangle and the position of the text
	update();
}

inline Button::~Button() {}	// virtual destructor

// checks of the if the mouse is inside the Rectangle shape of the button
// mouse_pos: input. coordinates of mouse position
// return: true if mouse_pos is inside the Rectangle shape of the button, false otherwise
inline bool Button::is_mouse_on_button(const sf::Vector2f mouse_pos)
{
	sf::FloatRect outline_rect(btn_shape.getPosition(), btn_shape.getSize());	// convert the Rectangle Shape to a Rect

	if (outline_rect.contains(mouse_pos))
		return true;

	return false;
}

// returns button_pressed (gets set in mouse_clicked_processor())
bool Button::is_button_pressed()
{
	return button_pressed;
}

// sets button_pressed to false (should be called after the function of the button was executed in an external function)
void Button::button_pressed_reset()
{
	button_pressed = false;
}

// set the position of the text and the rectangle outline (the text gets adjusted to the rectangle position)
// position: input. desired coordinates of the Button
void Button::setPosition(const sf::Vector2f& position)
{
	btn_shape.setPosition(position);
	update();
}

// returns the position of the Button (of the rectangle shape)
const sf::Vector2f& Button::getPosition()
{
	return btn_shape.getPosition();
}

// returns the dimensions of the rectangle shape of the Button
const sf::Vector2f& Button::getSize()
{
	return btn_shape.getSize();
}

// set the text of the Button and adjusts the Rectangle Shape to the new text
// string_in: input. new text of the Button
void Button::setString(const string& string_in)
{
	btn_text.setString(string_in);
	update();
}

// set the size of the Text (and of the Button itself)
// char_size: input. character size of the text in pixels
void Button::setCharacterSize(unsigned int char_size)
{
	btn_text.setCharacterSize(char_size);
	update();
}

// set the Font of the Text. Adjust the Rectangle Shape to the new text
// font: input. new font of the text (must be loaded correctly before)
void Button::setFont(const sf::Font& font)
{
	btn_text.setFont(font);
	update();
}

// set up the size of the rectangle according to the text and the position of the text according to the rectangle.
// only needs to be executed when some Attribute of the Button has changed. No periodic Update necessary
inline void Button::update()
{
	// set the dimensions of the button outline
	// the method getGlobalBounds() doesn't work if the font of the text is not set yet.
	sf::Vector2f rect_size = sf::Vector2f(btn_text.getGlobalBounds().width + 2 * margin, btn_text.getGlobalBounds().height + 2 * margin);
	btn_shape.setSize(rect_size);

	// set the text position
	// the position of text.getGlobalBounds() and the text itself is not the same!
	// the position of the text itself includes a spacing on top of the text (to fit all possible characters), whereas the global boundary adjusts to the current string of the text
	float text_pos_diff = btn_text.getGlobalBounds().top - btn_text.getPosition().y;
	btn_text.setPosition(btn_shape.getPosition().x + margin, btn_shape.getPosition().y + margin - text_pos_diff);	// set text position in the middle of the rectangle
}

inline void Button::update_physics() {}

inline void Button::key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt) {}

// set button_pressed to true if the Button was clicked with a left mouse click
inline void Button::mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt)
{
	sf::Vector2f mouse_pos((float)pressed_mouse_evnt.x, (float)pressed_mouse_evnt.y); // position of the mouse at the time of the mouse click

	if (pressed_mouse_evnt.button == sf::Mouse::Left && is_mouse_on_button(mouse_pos))
		button_pressed = true;	// needs to be reset manually after processing the functionality of the button
}

// draw the Button on the Window
inline void Button::draw_on_window(sf::RenderWindow& window)
{
	sf::Vector2f mouse_pos = (sf::Vector2f)sf::Mouse::getPosition(window);		// get the position of the mouse realtive to the Render window and use a typecast to a float vector
	if (is_mouse_on_button(mouse_pos))	// change color if the mouse is hovering over the Button
	{
		btn_shape.setOutlineColor(active_color);
		btn_text.setFillColor(active_color);
		btn_text.setOutlineColor(active_color);
	}
	else
	{
		btn_shape.setOutlineColor(idle_color);
		btn_text.setFillColor(idle_color);
		btn_text.setOutlineColor(idle_color);
	}

	window.draw(btn_text);
	window.draw(btn_shape);
}
