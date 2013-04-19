#include "useitemmenu.h"

UseItemMenu::UseItemMenu(int itemID) {
	this->itemID = itemID;
}

UseItemMenu::~UseItemMenu() {
}

void UseItemMenu::update() {
	const int CURSEL = cursor.getSelection();
	int newCurSel = CURSEL;

	// exit menu or cancel option
	if (input->getCancel()) {
		input->resetCancel();	// reset cancel input
		menuState->popMenu();	// exit useitemmenu
		return;					// return immediately
	}

	// character selected
	if (input->getConfirm()) {
		input->resetConfirm();
		// TODO use item

		// remove one of the items from inventory
		party->addItem(itemID, -1);
		if (!party->getItemCount(itemID)) {
			// out of item, return to item menu
			menuState->popMenu();
			return;
		}
	}

	// cursor movement
	if (input->downPressed()) {
		newCurSel++;
	} else if (input->upPressed()) {
		newCurSel--;
	}

	// check for wrap wround
	if (newCurSel > Party::FOURTH) {
		newCurSel = Party::FIRST;
	} else if (newCurSel < Party::FIRST) {
		newCurSel = Party::FOURTH;
	}

	// update cursor selection
	cursor.setSelection(newCurSel);

	// input handled
	input->resetAll();
}

void UseItemMenu::render() {
	// x pos of character sprites
	const int SPRITEX = -windowWidth / 1.25;

	// y positions of the four different characters
	// static_cast to get rid of compiler warnings about double to float
	const float SPRITEY[Party::SIZE] = {static_cast<float>(windowHeight * 0.69),
										static_cast<float>(windowHeight * 0.23),
										static_cast<float>(-windowHeight * 0.23),
										static_cast<float>(-windowHeight * 0.69)};

	// blue background and border entire window
	blueBackground(windowWidth, windowHeight);
	border(-windowWidth, windowWidth, windowHeight, -windowHeight);

	// borders around submenus
	subBorders();

	// text in submenus
	subText();

	// render names, sprites, lv, job, hp, mp
	for (int i = Party::FIRST; i < Party::SIZE; i++) {
		Party::Characters c = static_cast<Party::Characters>(i);

		// character name and sprite
		twenty.drawText(SPRITEX - 35, SPRITEY[i] + twenty.getLineSkip() * 0.75, 
				party->getName(c).c_str());
		party->render(c, SPRITEX, SPRITEY[i] + 50);

		// draw character details
		std::string level = "Lv. " + 
				std::to_string(party->getAttribute(c, Character::LEVEL)) + "  ";
		std::string details = level + party->getJob(c) + "\nHP\nMP";
		twenty.drawText(-windowWidth / 1.75, SPRITEY[i], details.c_str());

		// get hp/mp fraction strings
		std::string hpFraction = "\n" + party->getHPFraction(c);
		std::string mpFraction = "\n\n" + party->getMPFraction(c);

		// hp/mp is right aligned
		const int RIGHTEDGE = -windowWidth / 1.75 + 390;
		SDL_Rect r = {0, 0, 0, 0};

		// draw the hp fraction right aligned
		twenty.textSize(hpFraction.c_str(), &r);
		twenty.drawText(RIGHTEDGE - r.w, SPRITEY[i], hpFraction.c_str());

		// draw the mp fraction right aligned
		twenty.textSize(mpFraction.c_str(), &r);
		twenty.drawText(RIGHTEDGE - r.w, SPRITEY[i], mpFraction.c_str());
	}

	// render the cursor(s)
	cursorRender(SPRITEX, SPRITEY);
}

void UseItemMenu::subBorders() {
	const int LEFTX = windowWidth - 550;
	border(LEFTX, windowWidth, windowHeight, windowHeight - 300);
	border(LEFTX, windowWidth, windowHeight - 300, -windowHeight + 200);
	border(LEFTX, windowWidth, -windowHeight + 200, -windowHeight);
}

void UseItemMenu::subText() {
	const int LINEHEIGHT = twenty.getLineSkip();

	const int LEFTX = windowWidth - 480;	// left edge for text
	const int ITEMY = windowHeight - 120;	// y for item name
	const int DESCY	= windowHeight - 400;	// y for item description
	const int CONTY	= -windowHeight + 100;	// y for controls
	const int DESCWIDTH = 410;				// width available for desriptions

	// draw item name and total left
	std::string itemName = items.getName(itemID);
   	std::string itemTotal = "Total          " + 
							std::to_string(party->getItemCount(itemID));
	twenty.drawText(LEFTX, ITEMY, itemName.c_str());
	twenty.drawText(LEFTX, ITEMY - LINEHEIGHT * 1.5, itemTotal.c_str());

	// draw description in description area
	// add line breaks in between words when need to fit
	SDL_Rect r;	// store the rectangle text fits in

	// get item description, and number of characters
	std::string desc = items.getDescription(itemID);
	int descSize = desc.size();

	// substr that fits on current line so far
	std::string curLine;

	// loop through string
	int i, j;
	for (i = 0, j = 0; i <= descSize; i++) {
		// if end of string, space or newline found
		if (!desc[i] || desc[i] == ' ' || desc[i] == '\n') {
			// add the next word to curLine
			curLine += desc.substr(j, i - j);;

			// check if curLine still fits within description area
			twenty.textSize(curLine.c_str(), &r);
			if (r.w > DESCWIDTH) {
				// doesnt fit, insert a newline before the word
				desc[j] = '\n';

				// move to new line
				curLine.clear();
			}
			j = i;
		}
	}

	// draw description that will now fit
	twenty.drawText(LEFTX, DESCY, desc.c_str());

	// draw confirm and cancel
	std::string controls = "A    Confirm\nB    Back";
	twenty.drawText(LEFTX, CONTY, controls.c_str());
}

void UseItemMenu::cursorRender(const int SPRITEX, const float *SPRITEY) {
	// cursor is pointing at character to use item on
	int curY = SPRITEY[cursor.getSelection()];
	cursor.render(SPRITEX - 20, curY);
}