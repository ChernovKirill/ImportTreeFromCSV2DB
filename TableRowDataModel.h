#ifndef TABLEROWDATAMODEL_H
#define TABLEROWDATAMODEL_H

#include <QList>
#include <QtSql>

#include "TableColumnDataModel.h"

class TableRowDataModel
{
public:

    TableRowDataModel(const QString &tableName, const QList<TableColumnDataModel> columns);

    QString tableName() const;

    int idIndex() const;

    int parentIndex() const;

    static QString sqlRecordToString(const QSqlQuery query);

    bool printTable();

    QList<TableColumnDataModel> columns() const;

    QMap<int, TableColumnDataModel> sortedColumns() const;

    bool isUnquotedField(int fieldIndex) const;
    bool isLinkField(int fieldIndex) const;

    QString isExistQueryTemplate() const;

    QString selectChildsQueryTemplate() const;

    QString selectParentIdQueryTemplate() const;

    QString insertQueryTemplate() const;

    QString deleteQueryTemplate() const;

    QString updateQueryTemplate() const;

private:
    QString _tableName;
    QList<TableColumnDataModel> _columns;
    QMap<int,TableColumnDataModel> _sortedColumns;
    int _idIndex;
    int _parentIndex;
    QSet<int> _unquotedFields;
    QSet<int> _linkFields;

    QString _isExistQueryTemplate;
    QString _selectChildsQueryTemplate;
    QString _selectParentIdQueryTemplate;
    QString _insertQueryTemplate;
    QString _deleteQueryTemplate;
    QString _updateQueryTemplate;

};
#endif // TABLEROWDATAMODEL_H
