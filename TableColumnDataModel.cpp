#include "TableColumnDataModel.h"

TableColumnDataModel::TableColumnDataModel() {}

TableColumnDataModel::TableColumnDataModel(QString columnName, QString columnTitleName, QString columnType, QString columnTypeForCreate, bool isId, bool isParentId, bool isLink, bool hasDefaultValue, int positionInInterface)
{
    _columnName = columnName;
    _columnTitleName = columnTitleName;
    _columnType = columnType;
    _columnTypeForCreate = columnTypeForCreate;
    _isId = isId;
    _isParentId = isParentId;
    _isLink = isLink;
    _hasDefaultValue = hasDefaultValue;
    _positionInInterface = positionInInterface;
}

QString TableColumnDataModel::columnName() const
{
    return _columnName;
}

QString TableColumnDataModel::columnTitleName() const
{
    return _columnTitleName;
}

QString TableColumnDataModel::columnType() const
{
    return _columnType;
}

QString TableColumnDataModel::columnTypeForCreate() const
{
    return _columnTypeForCreate;
}

bool TableColumnDataModel::isId() const
{
    return _isId;
}

bool TableColumnDataModel::isParentId() const
{
    return _isParentId;
}

bool TableColumnDataModel::isLink() const
{
    return _isLink;
}

bool TableColumnDataModel::hasDefaultValue() const
{
    return _hasDefaultValue;
}

int TableColumnDataModel::positionInInterface() const
{
    return _positionInInterface;
}

// For logging
QDebug operator<<(QDebug debug, const TableColumnDataModel& column)
{
    QDebugStateSaver saver(debug);
    debug.noquote() << "(" << column.columnName() << "," << column.columnType() << "," << column.columnTitleName() << ",";
    if(column.hasDefaultValue())
        debug.noquote() << column.columnTypeForCreate() << ",";
    if(column.isId())
        debug << "it's primary id,";
    if(column.isParentId())
        debug << "it's parent id,";
    if(column.isLink())
        debug << "it's link,";
    debug << column.positionInInterface() << ")";

    return debug;
}


