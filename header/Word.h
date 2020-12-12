#ifndef _WORD_HPP_
#define _WORD_HPP_

#include "Entity.h"


// Defines a Word that moves across the Screen, has health that is depleting with a timer and it can be typed to mark the word as finished
// class Word is derived from class Text and Entity
class Word : public sf::Text, public Entity
{
public:
	typedef enum word_state
	{
		ALIVE,			// the word can still be typed
		TYPED,			// all letters of the word were typed
		DEAD,			// the word died, because it has no more health
	} word_state_t;

	float velocity;				// movement speed of the word. in pixel per second
	unsigned int writing_index;	// indicates the next index/ letter of the word text that shall be typed
	float health;				// indicates how much health is still left. The health takes 1 damage per second

	Word(const sf::String& string = "_default_", const sf::Font& font = sf::Font(), float velo = 0, float max_hp = 0);
	virtual ~Word();

	double get_angle();
	void set_angle(double angle_in);
	word_state_t get_state();
	sf::Vector2f get_velocity_vector();

	virtual void update();
	virtual void update_physics();
	virtual void draw_on_window(sf::RenderWindow& window);
	virtual void key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt);
	virtual void mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt);

private:
	double angle;				// direction of moving word. measured clockwise from the x-axis (because coordinate origin is in the top left corner). in rad
	float max_health;			// maximum number of seconds that the word exists on the field
	word_state_t state;			// state of the word
	sf::Clock physics_clock;	// used for the word movement. Clock starts automatically after being constructed
	sf::Clock health_clock;		// used for the health depletion. Clock starts automatically after being constructed
};

#endif // _WORD_HPP_
