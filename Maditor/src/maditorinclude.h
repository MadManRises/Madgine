#pragma once

#include <libinclude.h>

#include "Model/editorforward.h"

#include "Util/UtilMethods.h"
#include "Util/Profiler.h"

#include "Input/InputHandler.h"

#include "App/Application.h"
#include "App/appsettings.h"

#include "GUI\GUIEvents.h"
#include "GUI\GUISystem.h"

#include "Scene/scenemanager.h"
#include "Scene\Entity\entity.h"
#include "Scene/scenecomponent.h"

#include "Scripting\Parsing\scriptparser.h"

#include "UI\UIManager.h"

#include <mutex>
#include <unordered_map>
#include <functional>
#include <set>
#include <queue>

#include <Qsci\qsciabstractapis.h>
#include <Qsci/qsciscintilla.h>

#include <QMenu>
#include <QStringList>
#include <QApplication>
#include <qdom.h>
#include <QAbstractItemModel>
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
