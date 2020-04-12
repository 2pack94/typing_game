#include <list>
#include <thread>
#include <mutex>
#include "Entity.h"
#include "GameSettings.h"
#include "StartScreen.h"
#include "OptionScreen.h"
#include "Playfield.h"

using namespace std;

// Explanation of some C++ Terminology: see Entity.h
// Tutorial for running SFML in Visual Studio: https://www.sfml-dev.org/tutorials/2.5/start-vc.php
// SFML Version used: 2.5.1

mutex mutex_glob;	// the mutex is used to prevent the manipulation of the same data in different threads at the same time.

// Template Function. deletes every Element of a list (considering that the memory of the Elements were allocated by 'new')
// list_in: input/ output. list with Elements of the template type T
template <typename T> void delete_list(list<T>& list_in)
{
	for (auto entity_it = list_in.begin(); entity_it != list_in.end(); )	// iterator is used to point at the Elements of the list. get the fitting type automatically with auto
	{
		T entity_tmp = *entity_it;				// store the current Element to still have a pointer on it after the erase from the list and to delete it afterwards
		entity_it = list_in.erase(entity_it);	// delete the Element from the list pointed by the iterator. return a new iterator with the updated list which points on the Element after the deleted one
		delete entity_tmp;						// after the element is erased from the list, delete must still be called. delete calls the destructor and frees up the memory that was allocated by new.
	}
}

// In this task all Entities that have a physic are getting updated.
// phys_entity: input. Reference to the list of the entities to update.
// running: input. this reference is used to signal the thread to terminate.
void physic_task(list<Entity*>& phys_entity, bool& running)
{
	while (1)
	{
		for (list<Entity*>::iterator entity_it = phys_entity.begin(); entity_it != phys_entity.end(); entity_it++)	// iterator is used to point at the Elements of the list.
		{
			(*entity_it)->update_physics();		// entity_it is a pointer to a list Element which is a pointer to an Entity. To get a Entity object, the iterator must be dereferenced 2 times.
		}

		this_thread::sleep_for(chrono::milliseconds(10));	// sleep some time before updating the physics again

		if (!running)
			break;
	}
}

// main game loop
int main()
{
	srand((unsigned int)time(0));	// use current time in seconds since January 1, 1970 as seed for rand() functions
	(void)rand();					// returns an integer between 0 and RAND_MAX. use rand one time to make the next call more random
	
	GameSettings settings;			// create a GameSettings object that is valid for the whole main thread
	list<Entity*> entities;			// list where Pointer to all Entities to process are stored

	// create the game window. window can be closed and has a titlebar but cannot be resized
	sf::RenderWindow window(sf::VideoMode((unsigned int)settings.get_window_size().x, (unsigned int)settings.get_window_size().y), "typing_game", sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);	// enable V-Sync

	// start a separate thread to compute the physics of all objects (not really needed in this case, just to demonstrate the concept)
	bool physic_thread_running = true;	// flag to signal the thread to terminate
	thread physic_thread(physic_task, ref(entities), ref(physic_thread_running));		// The first argument is the name of the function/ member function that shall be started in a new thread. if a reference needs to be passed to a thread, it must be wrapped in std::ref()

	GameSettings::game_state_t last_game_state = settings.game_state;		// always store the last game_state to detect a change in game_state
	
	while (window.isOpen())
	{
		// delete entity list
		mutex_glob.lock();		// lock the mutex if free or wait here and lock it when its free
		delete_list(entities);
		mutex_glob.unlock();	// release the mutex again

		// put Entities in the Entity list according to the game_state. Process these Entities in the main loop (invoke all functions that are declared in the Entity class)
		switch (settings.game_state)
		{
		case GameSettings::START_SCREEN:
			last_game_state = settings.game_state;
			entities.push_back(new StartScreen(settings));
			break;

		case GameSettings::PLAY_SCREEN:
			last_game_state = settings.game_state;
			switch (settings.getBoundaryID())
			{
			case GameSettings::RECT:
				entities.push_back(new PlayfieldRect(settings));
				break;
			case GameSettings::CIRC:
				entities.push_back(new PlayfieldCirlce(settings));
				break;
			}
			break;

		case GameSettings::OPTIONS_SCREEN:
			last_game_state = settings.game_state;
			entities.push_back(new OptionScreen(settings));
			break;

		case GameSettings::EXIT:		// if game window was closed or exit Button was pressed
			last_game_state = settings.game_state;
			window.close();
			break;
		}
		
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))		// process all SFML events that occured since the last poll
			{
				if (event.type == sf::Event::Closed)
					settings.game_state = GameSettings::EXIT;

				if (event.type == sf::Event::KeyPressed)
				{
					for (auto entity_it = entities.begin(); entity_it != entities.end(); entity_it++)
					{
						(*entity_it)->key_pressed_processor(event.key);
					}
				}

				if (event.type == sf::Event::MouseButtonPressed)
				{
					for (auto entity_it = entities.begin(); entity_it != entities.end(); entity_it++)
					{
						(*entity_it)->mouse_clicked_processor(event.mouseButton);
					}
				}
			}

			for (auto entity_it = entities.begin(); entity_it != entities.end(); entity_it++)
			{
				(*entity_it)->update();
			}

			window.clear();
			for (auto entity_it = entities.begin(); entity_it != entities.end(); entity_it++)
			{
				(*entity_it)->draw_on_window(window);
			}
			window.display();

			if (last_game_state != settings.game_state)	// if game state changed
				break;
		}
	}
	// end the physics thread before exiting the main.
	physic_thread_running = false;	// set flag to signal to the thread to end
	physic_thread.join();			// wait for thread to finish

	return 0;
}