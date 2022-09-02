#pragma once
#include <elang_builder.h>
#include <gl/glew.h>
#include <QtCore/qglobal.h>
#include <QApplication>
#include <QMainWindow>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QWidget>
#include <QOpenGlContext>
#include <QOpenGlWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QCloseEvent>
#include <QAction>
#include <QActiongroup>
#include <QComboBox>
#include <QBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QButtonGroup>


#ifndef BUILD_STATIC
# if defined(_ELANGQT_DLL)
#  define _ELANGQT_EXPORT Q_DECL_EXPORT
# else
#  define _ELANGQT_EXPORT Q_DECL_IMPORT
# endif
#else
# define _ELANGQT_EXPORT
#endif
