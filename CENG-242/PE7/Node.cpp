#include "Node.h"

// // // THE FRIEND METHODS ARE ALREADY IMPLEMENTED. // // //
// // // // // // // DO NOT CHANGE THEM! // // // // // // //

Node::Node(int id) {
	// TODO
}

Node::~Node() {
	// TODO
}

Node::Node(const Node& node) {
	// TODO
}

int Node::getId() const {
	// TODO
}

char Node::getData() const {
	// TODO
}

vector<Node*>& Node::getChildren() {
	// TODO
}

void Node::operator+=(Node& childNode) {
	// TODO
}

Node* Node::operator&(const Node& node) const {
	// TODO
}

// This is already implemented for you, do NOT change it!
ostream& operator<<(ostream& os, const Node& node) {
	try {
		node.getData();
		os << *(DataNode*)&node;
	}
	catch (InvalidRequest e) {
		os << "[" << node.id;
		for (int i = 0; i < node.children.size(); i++)
			os << ", " << *node.children[i];
		os << "]";
	}
	return os;
}

/*************** DataNode *****************/

DataNode::DataNode(int id, char data) {
	// TODO
}

DataNode::~DataNode() {
	// TODO
}

DataNode::DataNode(const DataNode& dataNode) {
	// TODO
}

DataNode::DataNode(const Node& node, char data) {
	// TODO
}

char DataNode::getData() const {
	// TODO
}

// This is already implemented for you, do NOT change it!
ostream& operator<<(ostream& os, const DataNode& dataNode) {
	os << "[" << "(" << dataNode.id << ", \'" << dataNode.data << "\')";
	for (int i = 0; i < dataNode.children.size(); i++)
		os << ", " << *dataNode.children[i];
	os << "]";
	return os;
}

