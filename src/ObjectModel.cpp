#include "ObjectModel.h"

ObjectModel::~ObjectModel() {

}

void ObjectModel::draw() {
	
	glPushMatrix();

	glTranslatef(this->position.x, this->position.y, this->position.z);

	glRotatef(this->rotation.x, 1.0, 0.0, 0.0);
	glRotatef(this->rotation.y, 0.0, 1.0, 0.0);
	glRotatef(this->rotation.z, 0.0, 0.0, 1.0);

	//draw this object itself
	this->drawThis();

	//draw any children
	this->drawChildren();

	glPopMatrix();

}

void ObjectModel::drawChildren() {
	for (unsigned int i = 0; i < this->children.size(); i++)
		this->children[i]->draw();
}
