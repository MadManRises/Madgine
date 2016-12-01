#pragma once

#if Maditor_EXPORTS
#define MADITOR_EXPORT __declspec(dllexport)
#else
#define MADITOR_EXPORT __declspec(dllimport)
#endif


#include "Model/editorforward.h"

#include <QMenu>
#include <QStringList>
#include <QApplication>
#include <qdom.h>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QMessageBox>
#include <QEvent>
#include <QDebug>
#include <QWindow>
#include <QShortcut>
#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QSettings>
#include <QFont>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDirIterator>
#include <QCloseEvent>
#include <QFontDatabase>
#include <QHeaderView>
#include <QMainWindow>
#include <QTableView>
#include <QPushButton>
#include <QPainter>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QToolBar>
#include <QCheckBox>
#include <QFileSystemModel>

#include <mutex>
#include <unordered_map>
#include <functional>
#include <set>
#include <queue>
#include <list>

#include <Qsci\qsciabstractapis.h>
#include <Qsci/qsciscintilla.h>