#pragma once

// System Headers
#include <optional>
#include <cstring>
#include <filesystem>
#include <utility>
#ifdef _WIN32
#include <memory>
#include <Windows.h>
#include <shlobj_core.h>
#else
#include <cstdlib>
#endif
#include <chrono>

// Qt Headers
#include <QMainWindow>
#include <QApplication>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QProcess>
#include <QScrollArea>
#include <QSettings>
#include <QStyle>
#include <QStyleHints>
#include <QVBoxLayout>
#include <QList>
#include <QString>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QToolButton>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QProgressDialog>
#include <QStandardPaths>
#include <QPlainTextEdit>

// Configuration File
#include "Config.h"

// Custom Widget Buttons
#include "LaunchButton.h"

// UI Headers
#include "GameConfig.h"
#include "NewModDialog.h"
#include "NewAddonDialog.h"
#include "Window.h"