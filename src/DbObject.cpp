#include "DbObject.h"

#include <tools/Logger.h>

DECLARE_STATIC_LOGGER(logger, "db_object");

namespace polygon4
{
    
DbObject::DbObject()
{
}

DbObject::~DbObject()
{
}

ObjectName DbObject::getResourceName() const
{
    return resourceName;
}

void DbObject::setResourceName(const ObjectName &name)
{
    resourceName = name;
}

}
