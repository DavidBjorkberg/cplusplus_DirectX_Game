#include "Container.h"

std::unique_ptr<Container> Container::playerInventory;

Container::Container()
{
}

Container::~Container()
{
	for (int i = 0; i < ItemType::TYPES_SIZE; i++)
	{
		for (Container::Slot* slot : this->itemLists[i])
		{
			delete slot;
		}
	}
}

void Container::addItem(Item* item)
{
	this->itemLists[item->getType()].push_back(new Slot{this, item});
}

void Container::removeItem(Slot* slot)
{
	auto found = std::lower_bound(this->itemLists[slot->item->getType()].begin(), this->itemLists[slot->item->getType()].end(), slot);

	if (found != this->itemLists[slot->item->getType()].end())
	{
		delete *found;
		this->itemLists[slot->item->getType()].erase(found);
	}
}

bool Container::containsItem(Slot* slot)
{
	return this->containsItem(slot->item);
}

bool Container::containsItem(Item* item)
{
	for (int i = 0; i < ItemType::TYPES_SIZE; i++)
	{
		for (Container::Slot* slot : this->itemLists[i])
		{
			if (slot->item == item)
			{
				return true;
			}
		}
	}

	return false;
}

std::vector<Container::Slot*>* Container::getItemStack(ItemType type)
{
	return &this->itemLists[type];
}

Container::Slot* Container::getItemSlot(ItemType type, int index)
{
	return this->itemLists[type][index];
}