#include "Action.h"
#include "Exception.h"

Action::Action() {
	// TODO
}

Action::~Action() {
	// TODO
}

/********************** CompleteAction **************************/

CompleteAction::CompleteAction(const Node& node) {
	// TODO
}

CompleteAction::~CompleteAction() {
	// TODO
}

Node* CompleteAction::act(const Node* node) const {
	// TODO
}

/************************* CutAction ***************************/

CutAction::CutAction(char character) {
	// TODO
}

CutAction::~CutAction() {
	// TODO
}

Node* CutAction::act(const Node* node) const {
	// TODO
}

/************************* CompositeAction ***************************/

CompositeAction::CompositeAction() {
	// TODO
}

CompositeAction::~CompositeAction() {
	// TODO
}

CompositeAction* CompositeAction::addAction(const Action* action) {
	// TODO
}

Node* CompositeAction::act(const Node* node) const {
	// TODO
}