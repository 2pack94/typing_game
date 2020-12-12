#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_

#include "SFML/Graphics.hpp"

/////////////////////////////////////////////////////////////////
/// class hierarchy:
///		derived classes inherit everything from its parent class ecxept constructors, the destructor and the allocation operator
///		the private part of a class can only be accessed by the class itself.
///		a derived class can not directly access the private part of its parent class. use 'protected' for access for derived classes
///		order of the constructor invocation: constructor of parent class -> constructors of the member initializer list of the derived list -> constructor of the derived class
///		If a constructor of a member object is not explicitly listed in the member initializer list, the default constructor of the member object is called implicitly
/// lists in C++:
///		a list 'list<T> a' is a container for objects. list Elements are bidirectional linked (pointer show the memory location of the predecessor and successor)
///		a list stores Elements at non contiguous memory locations. So a class internal iterator must be used to access the Elements.
/// Reference:
///		a Reference 'int& a' is an Alias for an object. It works like a constant pointer (can't change which object is referenced)
///		mostly used for call by reference Parameters in functions or as function return values
/// abstract classes:
///		they arise when a class has at least one pure virtual function
///		they can't have objects
/// virtual functions:
///		are used to define a consistent interface for all classes derived from these base classes
///		every derived class redefines the virtual functions (with the same return value/ parameters/ name) for themself
/// pure virtual functions:
///		every pure virtual function must be overwritten (with the same return value/ parameters/ name) or the derived class will be also abstract!
/// polymorphism in C++:
///		every virtual function call is associated with its corresponding object
///		pointer to objects of derived classes can be stored in a vector / array / list of the parent class type
/////////////////////////////////////////////////////////////////


// Every game entity which needs a functionality that can be described with the following virtual functions shall inherit from this class.
// Every Class that inherits from this, can be put in the list of all game entities that are processed in the main game loop
// If a derived class doesn't need need a functionality that is intended for these virtual functions, the function definition can be left empty
class Entity
{
public:
	// virtual destructor. every class that inherits from this class should also manually define its virtual Destructor.
	// So the correct destructor/ sequence of destructors of the class belonging to the object can be called when using delete or going out of scope (see polymorphism).
	virtual ~Entity() {}
	// pure virtual functions:
	virtual void update() = 0;																			// for classes that need to be periodically updated
	virtual void update_physics() = 0;																	// for classes that have a physic
	virtual void draw_on_window(sf::RenderWindow& window) = 0;											// for classes that can be drawn to a window
	virtual void key_pressed_processor(const sf::Event::KeyEvent& pressed_key_evnt) = 0;				// for classes that need to react to a pressed key
	virtual void mouse_clicked_processor(const sf::Event::MouseButtonEvent& pressed_mouse_evnt) = 0;	// for classes that need to react to a pressed mouse button
};

#endif // _ENTITY_HPP_
