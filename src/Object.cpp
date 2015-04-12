#include "Object.h"

#include <tools/Logger.h>

DECLARE_STATIC_LOGGER(logger, "object");

namespace polygon4
{

Object::Object()
{
}

Object::~Object()
{
}

ObjectName Object::getInternalName() const
{
    return internalName;
}

ObjectName Object::getDisplayedName() const
{
    return displayedName;
}

bool Object::hasObject(const ObjectName &name)
{
    Objects &objects = getObjects();
    return objects.find(name) != objects.end();
}

Object *Object::getObject(const ObjectName &name)
{
    if (!hasObject(name))
        return 0;
    return getObjects().find(name)->second;
}

void Object::setInternalName(const ObjectName &name)
{
    internalName = name;
}

void Object::setDisplayedName(const ObjectName &name)
{
    displayedName = name;
}

bool Object::addObject()
{
    if (hasObject(getInternalName()))
        return false;
    getObjects()[getInternalName()] = this;
    return true;
}

bool Object::addObject(const ObjectName &name, Object *object)
{
    if (hasObject(name))
        return false;
    getObjects()[name] = object;
    return true;
}

bool Object::removeObject()
{
    if (!hasObject(getInternalName()))
        return false;
    getObjects().erase(getInternalName());
    return true;
}

bool Object::removeObject(const ObjectName &name)
{
    if (!hasObject(name))
        return false;
    getObjects().erase(name);
    return true;
}

Objects &Object::getObjects()
{
    static Objects objects;
    return objects;
}

}
