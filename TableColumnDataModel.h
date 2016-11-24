#ifndef TABLECOLUMNDATAMODEL_H
#define TABLECOLUMNDATAMODEL_H

#include <QtSql>

class TableColumnDataModel
{
public:
    TableColumnDataModel();

    TableColumnDataModel(QString columnName, QString columnTitleName, QString columnType, QString columnTypeForCreate,
                         bool isId, bool isParentId, bool isLink, bool hasDefaultValue, int positionInInterface);

    QString columnName() const;

    QString columnTitleName() const;

    QString columnType() const;

    QString columnTypeForCreate() const;

    bool isId() const;

    bool isParentId() const;

    bool isLink() const;

    bool hasDefaultValue() const;

    int positionInInterface() const;

private:
    friend QDebug operator<<(QDebug debug, const TableColumnDataModel &column);

    QString _columnName;
    QString _columnTitleName;
    QString _columnType;
    QString _columnTypeForCreate;
    bool _isId;
    bool _isParentId;
    bool _isLink;
    bool _hasDefaultValue;
    int _positionInInterface;

};


#endif // TABLECOLUMNDATAMODEL_H
