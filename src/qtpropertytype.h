#ifndef QTPROPERTYTYPE_H
#define QTPROPERTYTYPE_H

#include <QString>

namespace QtPropertyType
{
	typedef QString Type;
    extern const Type NONE;
    extern const Type BOOL;
    extern const Type INT;
    extern const Type FLOAT;
    extern const Type STRING;
    extern const Type GROUP;
    extern const Type LIST;
    extern const Type DICT;
    extern const Type ENUM;
    extern const Type FLAG;
    extern const Type COLOR;
    extern const Type FILE;
    extern const Type DYNAMIC_LIST;
    extern const Type DYNAMIC_ITEM;
    extern const Type ENUM_PAIR;
    extern const Type FLOAT_LIST;
}

#endif // QTPROPERTYTYPE_H
