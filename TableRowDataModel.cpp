#include "TableRowDataModel.h"

Q_DECLARE_LOGGING_CATEGORY(cInputCmd)
Q_DECLARE_LOGGING_CATEGORY(cInputFile)
Q_DECLARE_LOGGING_CATEGORY(cDB)
Q_DECLARE_LOGGING_CATEGORY(cDBCreate)
Q_DECLARE_LOGGING_CATEGORY(cDBInsert)
Q_DECLARE_LOGGING_CATEGORY(cDBUpdate)
Q_DECLARE_LOGGING_CATEGORY(cDBDelete)

TableRowDataModel::TableRowDataModel(const QString& tableName, const QList<TableColumnDataModel> columns)
{
    _tableName = tableName;
    _columns = columns;

    foreach(const TableColumnDataModel column, columns)
        _sortedColumns[column.positionInInterface()] = column;

    qDebug() << "_columns" << _columns;
    qDebug() << "_sortedColumns" << _sortedColumns;

    for(int i = 1; i <= _sortedColumns.size(); i++) {
        if(_sortedColumns[i].isId())
            _idIndex = i;
        if(_sortedColumns[i].isParentId())
            _parentIndex = i;
        if(_sortedColumns[i].isLink())
            _linkFields.insert(i);
        if(_sortedColumns[i].columnType() == "integer")
            _unquotedFields.insert(i);
    }
    qDebug() << "_idIndex" << _idIndex << ", _parentIndex" << _parentIndex << ", _linkFields" << _linkFields << ", _unquotedFields" << _unquotedFields;

    _isExistQueryTemplate = "SELECT count(id) FROM " + _tableName + " WHERE " + _sortedColumns[_idIndex].columnName() + " = %1;";
    qCDebug(cDBCreate) << "Check parent query template: " << _isExistQueryTemplate;

    _selectChildsQueryTemplate = "SELECT * FROM " + _tableName + " WHERE " + _sortedColumns[_parentIndex].columnName() + " = %1;";
    qCDebug(cDBCreate) << "Select childs query template: " << _selectChildsQueryTemplate;

    _selectParentIdQueryTemplate = "SELECT " + _sortedColumns[_parentIndex].columnName() + " FROM " + _tableName + " WHERE " + _sortedColumns[_idIndex].columnName() + " = %1;";
    qCDebug(cDBCreate) << "Select parent id query template: " << _selectParentIdQueryTemplate;

    _insertQueryTemplate = "INSERT INTO " + _tableName + " (" + _sortedColumns[_parentIndex].columnName() + ") VALUES (%1);";
    qCDebug(cDBCreate) << "Insert row query template: " << _insertQueryTemplate;

    _deleteQueryTemplate = "DELETE FROM " + _tableName + " WHERE " + _sortedColumns[_idIndex].columnName() + " = %1;";
    qCDebug(cDBCreate) << "Delete row query template: " << _deleteQueryTemplate;

    _updateQueryTemplate = "UPDATE " + _tableName + " SET %2 = %1%3%1 " + "WHERE " + _sortedColumns[_idIndex].columnName() + " = %4;";
    qCDebug(cDBCreate) << "Update query template: " << _updateQueryTemplate;

}

QString TableRowDataModel::tableName() const
{
    return _tableName;
}

int TableRowDataModel::idIndex() const
{
    return _idIndex;
}

int TableRowDataModel::parentIndex() const
{
    return _parentIndex;
}

// Печать текущей строки запроса
QString TableRowDataModel::sqlRecordToString(const QSqlQuery query)
{
    QSqlRecord rec = query.record();
    QString ts("(");
    for (int j = 0; j < rec.count(); j++) {
        ts.append(rec.fieldName(j) + "='" + query.value(j).toString() + "',");
    }
    ts.truncate(ts.count()-1);
    ts.append(")");
    return ts;
}

bool TableRowDataModel::printTable()
{
    qCDebug(cDB) << "Begin print table " << tableName();
    QString sQuery = "select * from " + tableName() + ";";
    QSqlQuery query;
    if (!query.exec(sQuery)) {
        qCWarning(cDB) << "Unable to execute query: " << query.lastError();
        return false;
    }
    qCDebug(cDB) << "Select query: " << sQuery;
    int i = 0;

    while (query.next()) {
        QString ts;
        ts.setNum(++i);
        ts.prepend("[");
        ts.prepend(tableName());
        ts.append("] ");
        ts.append(sqlRecordToString(query));
        qCDebug(cDB).noquote() << ts;
    }
    qCDebug(cDB) << "Total displayed records: " << i;
    qCDebug(cDB) << "End print table " << tableName();

    return true;
}

QList<TableColumnDataModel> TableRowDataModel::columns() const
{
    return _columns;
}

QMap<int, TableColumnDataModel> TableRowDataModel::sortedColumns() const
{
    return _sortedColumns;
}

bool TableRowDataModel::isUnquotedField(int fieldIndex) const
{
    return _unquotedFields.contains(fieldIndex);
}

bool TableRowDataModel::isLinkField(int fieldIndex) const
{
    return _linkFields.contains(fieldIndex);
}

QString TableRowDataModel::isExistQueryTemplate() const
{
    return _isExistQueryTemplate;
}

QString TableRowDataModel::selectChildsQueryTemplate() const
{
    return _selectChildsQueryTemplate;
}

QString TableRowDataModel::selectParentIdQueryTemplate() const
{
    return _selectParentIdQueryTemplate;
}

QString TableRowDataModel::insertQueryTemplate() const
{
    return _insertQueryTemplate;
}

QString TableRowDataModel::deleteQueryTemplate() const
{
    return _deleteQueryTemplate;
}

QString TableRowDataModel::updateQueryTemplate() const
{
    return _updateQueryTemplate;
}


