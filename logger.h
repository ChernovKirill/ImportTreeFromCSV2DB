#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QTextStream>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <QCoreApplication>

#include <iostream>

Q_LOGGING_CATEGORY(cSettings,"Settings")
Q_LOGGING_CATEGORY(cInputCmd,"Input.CMD")
Q_LOGGING_CATEGORY(cInputFile,"Input.File")
Q_LOGGING_CATEGORY(cDB,"DB")
Q_LOGGING_CATEGORY(cDBCreate,"DB.Create")
Q_LOGGING_CATEGORY(cDBInsert,"DB.Insert")
Q_LOGGING_CATEGORY(cDBUpdate,"DB.Update")
Q_LOGGING_CATEGORY(cDBDelete,"DB.Delete")

namespace Logger {
const bool isNeedTime = true;
const bool isTimeShift = false;
const bool isLog2File = true;
const bool isLog2Console = true;
const qint64 TimeShift = QDateTime::currentMSecsSinceEpoch();
#ifdef QT_GUI_LIB
const QTextCodec * codec = QTextCodec::codecForName("Windows 1251"); //HOWTO Вывод кириллицы в лог среды разработки
#else // no gui == console
const QTextCodec * codec = QTextCodec::codecForName("IBM 866"); //HOWTO Вывод кириллицы в консоль винды
#endif
const QString logFileNamePattern = "yyyy-MM-dd_hh-mm-ss";
const QString logFileName = QString("App_" + QDateTime::currentDateTime().toString(logFileNamePattern));
const QString logFileDescription = ".log";
QFile logFile(logFileName + logFileDescription);
QTextStream out(&logFile);

/*Получаем имя логфайла*/
const QString getLogFileName()
{
    return logFileName;
}

/*Получаем текущую дату и время в строке сразу*/
char * currentTime()
{
    return isNeedTime ? (isTimeShift ? QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch() - TimeShift).toString("mm 'min' ss 'sec' zzz 'ms'") : QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz")).toLatin1().data() : (char*)"" ;
}

/*Инициализация логгера*/
void initMessageHandler()
{
    if(!logFile.open(QIODevice::Append | QIODevice::Text)) {
        qInstallMessageHandler(0);
        qWarning() << "Cannot open logFile " << logFileName;
    } else {
        out.setCodec("UTF-8");
    }
}

/*Непосредственно сам перехватчик */
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Если надо выводить в консоль:
    if(isLog2Console) {
        std::cerr << codec->fromUnicode(qFormatLogMessage(type, context, msg)).toStdString() << std::endl;
    }
    // Если надо выводить в файл:
    if(isLog2File) {
        if(logFile.isOpen()) {
            out << qFormatLogMessage(type, context, msg) << endl;
            out.flush();
        }
    }
    return;
}

/*Функция, которая будет вызвана при выходе из приложения.*/
void exitFunction()
{
    // Если надо выводить в файл:
    if(isLog2File) {
        if(logFile.isOpen()) {
            out << qFormatLogMessage(QtMsgType::QtDebugMsg, QMessageLogContext(__FILE__, __LINE__, Q_FUNC_INFO, "App.Exit"), "Application was finished! It's working time was " + QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch() - TimeShift).toString("mm 'min' ss 'sec' zzz 'ms'") + ".");
            out.flush();
            logFile.close();
        }

    }
}

}

#endif // LOGGER_H
