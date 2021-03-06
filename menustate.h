#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "state.h"

#include "menu.h"
class Menu;

class MenuState : public State {
	public:
		MenuState();
		~MenuState();

		// push, pop or change menus
		void pushMenu(Menu *menu);
		void popMenu();
		void changeMenu(Menu *menu);

		// get menu count
		int getSize() { return size; }

		// ugly hack to copy battle state input to menu state :(
		void setInput(Input i) { input = i; }

		// exit all menus, startGame flag for
		// starting the game from party creation menu
		void exitMenus(bool startGame = 0);
	private:
		// operations
		void update();
		void render();

		// pause/unpause menu
		void pause();
		void unpause();

		// size of menu stack
		int size;

		// head and tail of linked list of menus
		// the linked list is treated like a stack
		Menu *head, *tail;
};

#endif
