#include "Word.h"

// Default constructor
Word::Word(const sf::String& string, const sf::Font& font, float velo, float max_hp)
{
	state = ALIVE;
	writing_index = 0;
	max_health = max_hp;
	health = max_health;
	velocity = velo;
	angle = 0;				// set spawning angle (and position) when the word is created by the playfield outside of this constructor

	setCharacterSize(25);			// set the character size of the text. in pixels
	setFillColor(sf::Color::White);	// set the color of the text
	setString(string);
	setFont(font);					// select the font
}

inline Word::~Word() {}	// virtual destructor

// returns the angle of the word. in rad
double Word::get_angle()
{
	return angle;
}

// set the angle of the word. in rad
void Word::set_angle(double angle_in)
{
	angle = angle_in;
}

// returns the state of the word
Word::word_state_t Word::get_state()
{
	return state;
}

// calculate and return the velocity vector of the word
sf::Vector2f Word::get_velocity_vector()
{
	// the member angle of the word is the angle between the x-Axis and the velocity vector
	// the member velocity is the amount of the vector
	// the math is like the Transformation between Polar coordinates (radius, angle) and cartesian coordinates (x,y), by using the angular relationship in a right triangle
	return sf::Vector2f((float)(velocity * cos(angle)), (float)(velocity * sin(angle)));
}

// update the health of the word and set the state of the word to DEAD if all health is depleted
inline void Word::update()
{
	float damage = 1;		// in health per second
	if (max_health == 0)	// if max_health is 0 the word takes no damage
		return;
	health -= damage * health_clock.restart().asSeconds();		// decrease the health according to the time that has passed since the last update 
	if (health <= 0)
		state = DEAD;
}

// compute the Movement of the word
inline void Word::update_physics()
{
	sf::Int64 elapsed = physics_clock.restart().asMicroseconds();		// restart returns the time after the last restart. convert sf::Time object to microseconds
	sf::Vector2f vel_vec = get_velocity_vector();
	sf::Vector2f delta_d_vec;							// travel distance since the last update
	delta_d_vec.x = (vel_vec.x * elapsed) / 1000000;	// distance = velocity * time
	delta_d_vec.y = (vel_vec.y * elapsed) / 1000000;

	move(delta_d_vec);		// moves the word starting from its current position (works also when arguments are < 1)
}

// draw the word, its health bar and display the already typed letters of the word in red color
inline void Word::draw_on_window(sf::RenderWindow& window)
{
	// make health bar
	if (max_health > 0)
	{
		sf::FloatRect word_bounds = getGlobalBounds();
		sf::RectangleShape health_bar;
		sf::Vector2f health_bar_size(word_bounds.width * (health / max_health), 2);		// the y-size is the thickness of the health bar
		sf::Vector2f health_bar_pos(word_bounds.left, word_bounds.top - 4);				// add some pixel margin at the top
		health_bar.setSize(health_bar_size);
		health_bar.setPosition(health_bar_pos);
		health_bar.setFillColor(sf::Color::Red);
		health_bar.setOutlineColor(sf::Color(255, 50, 50));
		health_bar.setOutlineThickness(1);
		window.draw(health_bar);
	}

	// make a Text object, consisting of the portion of the Word that is already typed
	sf::Text progress_str = *this;		// copy the properties of the Text. The "this" pointer points on the object from which this function is called
	progress_str.setString(progress_str.getString().substring(0, writing_index));	// get the portion of the Word that is already typed
	progress_str.setFillColor(sf::Color::Red);

	window.draw(*this);			// draw the full word
	window.draw(progress_str);	// draw over the word
}

// processes all key presses according to a german keyboard and update the writing index
void Word::key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt)
{
	if (state != ALIVE)
		return;

	unsigned char pressed_key = pressed_key_evnt.code;

	// convert pressed key to char
	if (pressed_key_evnt.code >= sf::Keyboard::Key::A && pressed_key_evnt.code <= sf::Keyboard::Key::Z && pressed_key_evnt.alt == false && pressed_key_evnt.control == false)	// if letter in the alphabet
	{
		pressed_key -= sf::Keyboard::Key::A;
		if (pressed_key_evnt.shift == true)
			pressed_key += 'A';
		else
			pressed_key += 'a';
	}
	else if (pressed_key_evnt.code >= sf::Keyboard::Key::Num0 && pressed_key_evnt.code <= sf::Keyboard::Key::Num9 && pressed_key_evnt.shift == false && pressed_key_evnt.alt == false && pressed_key_evnt.control == false)	// if number
	{
		pressed_key -= sf::Keyboard::Key::Num0;
		pressed_key += '0';
	}
	else if (pressed_key_evnt.code >= sf::Keyboard::Key::Numpad0 && pressed_key_evnt.code <= sf::Keyboard::Key::Numpad9 && pressed_key_evnt.shift == false && pressed_key_evnt.alt == false && pressed_key_evnt.control == false)	// if number (on numpad)
	{
		pressed_key -= sf::Keyboard::Key::Numpad0;
		pressed_key += '0';
	}
	else if (pressed_key_evnt.code == sf::Keyboard::Key::Space && pressed_key_evnt.alt == false && pressed_key_evnt.control == false)
	{
		pressed_key = ' ';
	}
	else if (pressed_key_evnt.code == sf::Keyboard::Key::Enter && pressed_key_evnt.alt == false && pressed_key_evnt.control == false)
	{
		pressed_key = '\r';
	}
	else if (pressed_key_evnt.code == sf::Keyboard::Key::Slash && pressed_key_evnt.alt == false && pressed_key_evnt.control == false)
	{
		// the slash key is the Quote/ Hashtag Key on a german Keyboard
		if (pressed_key_evnt.shift == true)
			pressed_key = '\'';	 // escape sequence needed
		else
			pressed_key = '#';
	}
	else if (pressed_key_evnt.code == sf::Keyboard::Key::Comma && pressed_key_evnt.alt == false && pressed_key_evnt.control == false)
	{
		if (pressed_key_evnt.shift == true)
			pressed_key = ';';	// the semicolon key on a german keyboard is the same as the comma key
		else
			pressed_key = ',';
	}
	else if (pressed_key_evnt.code == sf::Keyboard::Key::Hyphen && pressed_key_evnt.alt == false && pressed_key_evnt.control == false)
	{
		if (pressed_key_evnt.shift == true)
			pressed_key = '_';
		else
			pressed_key = '-';
	}
	else if (pressed_key_evnt.code == sf::Keyboard::Key::Period && pressed_key_evnt.alt == false && pressed_key_evnt.control == false)
	{
		if (pressed_key_evnt.shift == true)
			pressed_key = ':';	// the colon key on a german keyboard is the same as the period key
		else
			pressed_key = '.';
	}
	else if (pressed_key_evnt.code >= sf::Keyboard::Key::Escape && pressed_key_evnt.code <= sf::Keyboard::Key::Menu)
	{
		return;
	}
	else	// unhandled key
	{
		pressed_key = 0;
	}

	// if every character of the word was typed, the space or enter key must be hit in order to finish the word
	if (writing_index >= getString().getSize())
	{
		if (pressed_key == '\r' || pressed_key == ' ')
			state = TYPED;
		else
			writing_index = 0;
		return;
	}

	// check if pressed string is the next character in the word
	if (pressed_key == getString()[writing_index])
		writing_index++;
	else
		writing_index = 0;
}

inline void Word::mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt) {}