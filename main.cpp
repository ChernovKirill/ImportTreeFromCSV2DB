#include <QCoreApplication>
#include <QtSql>

#include "logger.h"
#include "TableRowDataModel.h"

using namespace Logger;
int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler); //переопределяем обработчик
    qSetMessagePattern("[%{time process} - %{if-debug}Debug: %{endif}%{if-info}Info: %{endif}%{if-warning}Warning: %{endif}%{if-critical}Critical: %{endif}%{if-fatal}Fatal!%{endif}%{if-category}%{category}%{endif}  ] %{function}:%{line} - %{message}");
    initMessageHandler();
    qAddPostRoutine(exitFunction); //не срабатывает

    QCoreApplication a(argc, argv);

    qCDebug(cInputCmd) << "argc = " << (argc-1);
    for(int i = 1; i < argc; i++) qCDebug(cInputCmd) << "argv[" << i << "] = " << argv[i];
    if(argc!=3) qFatal("Wrong number of the command-line parameters!\n Needs 2: tree data file name and db file name!");

// tree.txt
    QFile inputFile(argv[1]);
    if(!inputFile.open(QIODevice::ReadOnly | QFile::Text))
        qFatal("Cannot open input file '%s'!", QFile::encodeName(QFileInfo(inputFile).absoluteFilePath()).toStdString().c_str());

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
// db4tree
    db.setDatabaseName(getLogFileName()+".db"); //argv[2]);
    if(!db.open())
        qFatal("Cannot open '%s' database! Unable to establish a database connection: %s", db.databaseName(), db.lastError());
    else
        qCDebug(cDB) << "The database connection is opened successfully: " << db.databaseName();

    QTextStream inputFileTextStream(&inputFile);
    inputFileTextStream.setCodec("UTF-8");
    // Read headline
    qlonglong lineCount = 1;
    QString line = inputFileTextStream.readLine();
    qCDebug(cInputFile) << lineCount << ": " << line;
    QStringList lineWordsList = line.split("\t");
    if(lineWordsList.size()==1&&lineWordsList[0].length()==line.length())
        qCWarning(cInputFile) << "Wrong parsing: " << line;
    else
        qCDebug(cInputFile) << lineWordsList.length() << ": " << lineWordsList;

    QString tableName = lineWordsList[0].trimmed();
    qCDebug(cDB) << "Table for import name: " << tableName;

    QList<TableColumnDataModel> columns;
    columns << TableColumnDataModel("id", "id", "integer", "integer PRIMARY KEY AUTOINCREMENT", true, false, false, true, 4);
    columns << TableColumnDataModel("parent", "pid", "integer", "integer", false, true, false, false, 5);
    columns << TableColumnDataModel("name", "Title", "text", "text", false, false, false, false, 1);
    columns << TableColumnDataModel("status", "Status", "text", "text DEFAULT 'active'", false, false, false, true, 6);
    columns << TableColumnDataModel("priority", "Pr", "integer", "integer DEFAULT 0", false, false, false, true, 7);
    columns << TableColumnDataModel("creation_time", "Created", "datetime", "datetime DEFAULT (datetime('now','localtime'))", false, false, false, true, 2);
    columns << TableColumnDataModel("check_time", "Check", "datetime", "datetime DEFAULT (datetime('now','localtime'))", false, false, false, true, 3);
    columns << TableColumnDataModel("description", "Description", "text", "text", false, false, false, false, 8);
    columns << TableColumnDataModel("link", "Link", "text", "text", false, false, true, false, 9);
    qCDebug(cDB) << "Columns: " << columns;

    TableRowDataModel *table = new TableRowDataModel(tableName, columns);

    QMap<QString, int> columnNameToFilePositionMap({{"name",1},{"description",2},{"link",3}});
    qCDebug(cDBInsert) << "columnNameToFilePositionMap: " << columnNameToFilePositionMap;

    QString createTableQueryTemplate("CREATE TABLE ");
    createTableQueryTemplate.append(tableName);
    createTableQueryTemplate.append(" (");
    foreach(const TableColumnDataModel column, columns)
        createTableQueryTemplate.append(column.columnName() + " " + column.columnTypeForCreate() + ", ");
    createTableQueryTemplate.truncate(createTableQueryTemplate.count()-2);
    createTableQueryTemplate.append(");");

    qCDebug(cDBCreate) << "Create query: " << createTableQueryTemplate;

    QString insertQueryTemplate("INSERT INTO ");
    insertQueryTemplate.append(tableName);
    insertQueryTemplate.append(" (");
    foreach(const TableColumnDataModel column, columns)
        if(!column.hasDefaultValue()) {
            insertQueryTemplate.append(column.columnName());
            insertQueryTemplate.append(", ");
        }
    insertQueryTemplate.truncate(insertQueryTemplate.count()-2);
    insertQueryTemplate.append(") VALUES (");
    foreach(const TableColumnDataModel column, columns)
        if(!column.hasDefaultValue()) {
            insertQueryTemplate.append(":" + column.columnName());
            insertQueryTemplate.append(", ");
        }
    insertQueryTemplate.truncate(insertQueryTemplate.count()-2);
    insertQueryTemplate.append(");");
    qCDebug(cDBInsert) << "Insert query: " << insertQueryTemplate;

    QSqlQuery query;
    if(query.exec(createTableQueryTemplate))
        qCDebug(cDBCreate) << "The table " << tableName << " created successfully!";
    else
        qCWarning(cDBCreate) << "Cannot create table " << tableName << ": " << query.lastError();

    query.prepare(insertQueryTemplate);

    int level = -1;
    QStack<qlonglong> stack;
    qlonglong count = 0;
    stack.push(count++);
    QVariant lastInsertId;
    while(!inputFileTextStream.atEnd()) {
        lineCount++;
        line = inputFileTextStream.readLine();
        int currentLevel = line.indexOf(QRegularExpression("\\t\\S"));
        if(currentLevel==-1) { // Пропускаем не подходящую по формату строку (проверяется только префикс)
            qCDebug(cInputCmd) << lineCount << ": it's non-proper line! : " << line;
            continue;
        } else {
            lineWordsList = line.split("\t");
        // Создаём новые сущности:
            if(currentLevel<=level) { // Если мы не спускаемся на очередной подуровень (а значит мы либо остаёмся на том же уровне, либо возвращаемся на один из предыдущих),
                for(;currentLevel<=level;level--) stack.pop(); //...нужно удалить всех родителей, которые нам точно больше не понадобятся (вплоть до, но исключая, родителя текущего элемента)
            }
            level++;
            query.bindValue(":parent", stack.top());

            foreach(const TableColumnDataModel column, columns)
                if(!column.hasDefaultValue() && !column.isId() && !column.isParentId()) {
                    QString string(lineWordsList.value(currentLevel + columnNameToFilePositionMap[column.columnName()], ""));
                    // Удаляем внешние кавычки ...
                    if(string.size() > 0 && string.at(0) == "\"" && string.at(string.size()-1) == "\"") {
                        string.chop(1); // ... закрывающую
                        string.remove(0,1); // ... и открывающую
                    }
                    if(column.isLink())
                        string = QUrl(string).toString();
                    query.bindValue(":" + column.columnName(), string);
                }

            if(!query.exec())
                qCWarning(cDBInsert) << "Can't insert record in " << tableName << ": " << query.lastError();

            lastInsertId = query.lastInsertId();
            if(!lastInsertId.isValid()) {
                QLoggingCategory cDBInsert("DB.Insert");
                qFatal("%s : Cannot get lastInsertId! %s", cDBInsert.categoryName(), query.lastError());
            }
            qCDebug(cDBInsert) << lineCount << ":" << count << ": (id,parent,name,description,link) = (" << lastInsertId.toLongLong() << ";" <<  stack.top()<< ";" << lineWordsList.value(currentLevel+1, "") << ";" << lineWordsList.value(currentLevel+2, "") << ";" << lineWordsList.value(currentLevel+3, "") << ")";
            stack.push(lastInsertId.toLongLong()); // запоминаем текущего кандидата в родители
            count++;
        }
    }

    qCDebug(cDBInsert) << "Was totally inserted " << (count-1) << " new records. Last insert ID is " << lastInsertId.toLongLong();
    qCDebug(cDBInsert) << "The database created/updated successfully!";

    table->printTable();

    qCDebug(cDBInsert) << "The end of main!";

    QTimer::singleShot(0, &a, SLOT(quit())); //Quit immediately
    return a.exec();
}

