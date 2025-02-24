#include "precompiled.h"
#pragma hdrstop

namespace {

// NOLINTNEXTLINE(*-no-recursion)
void clearLayout(QLayout* layout, bool deleteWidgets = true) {
	while (QLayoutItem* item = layout->takeAt(0)) {
		if (deleteWidgets) {
			if (QWidget* widget = item->widget()) {
				widget->deleteLater();
			}
		}
		if (QLayout* childLayout = item->layout()) {
			clearLayout(childLayout, deleteWidgets);
		}
		delete item;
	}
}

#ifdef _WIN32
[[nodiscard]] QIcon getExecutableIcon(const QString& path) {
	HICON hIcon;
	if (SHDefExtractIconA(path.toLocal8Bit().constData(), 0, 0, &hIcon, nullptr, 16) != S_OK) {
		return QIcon{};
	}
	QIcon out{QPixmap::fromImage(QImage::fromHICON(hIcon))};
	DestroyIcon(hIcon);
	return out;
}
#endif

[[nodiscard]] QString getRootPath() {
	QString rootPath = QCoreApplication::applicationDirPath();

	if (auto cleanPath = QDir::cleanPath(rootPath); !cleanPath.isEmpty()) {
		return cleanPath;
	}
	return rootPath;
}

constexpr std::string_view STR_RECENT_CONFIGS = "str_recent_configs";
constexpr std::string_view STR_GAME_OVERRIDE = "str_game_override";

} // namespace

Window::Window(QWidget* parent)
		: QMainWindow(parent)
		, gameDefault(PROJECT_DEFAULT_MOD.data()) {
	this->setWindowTitle(PROJECT_NAME.data());
	this->setMinimumHeight(400);

	// Icon
	this->setWindowIcon(QIcon{getSDKLauncherIconPath()});

	// Config menu
	auto* configMenu = this->menuBar()->addMenu(tr("Config"));

	this->config_loadDefault = configMenu->addAction("Load Default", Qt::CTRL | Qt::Key_R, [this] {
		this->loadGameConfig(QString(":/config/%1.json").arg(PROJECT_DEFAULT_MOD.data()));
	});

	configMenu->addSeparator();

	configMenu->addAction(this->style()->standardIcon(QStyle::SP_DirIcon), tr("Load Custom..."), Qt::CTRL | Qt::Key_O, [this] {
		auto filename = QFileDialog::getOpenFileName(this, tr("Open Config"));
		if (!filename.isEmpty()) {
			this->loadGameConfig(filename);
		}
	});

	this->recent = configMenu->addMenu(this->style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Load Recent..."));
	// Will be regenerated naturally later on

	// Game menu
	auto* gameMenu = this->menuBar()->addMenu(tr("Game"));

	this->utilities_createNewMod = gameMenu->addAction(this->style()->standardIcon(QStyle::SP_FileIcon), tr("Create New Mod"), [this] {
		NewModDialog::open(::getRootPath(), this);
	});

	this->utilities_createNewAddon = gameMenu->addAction(this->style()->standardIcon(QStyle::SP_FileIcon), tr("Create New Addon"), [this] {
		QString gameRoot;
		if (QSettings settings; settings.contains(STR_GAME_OVERRIDE)) {
			gameRoot = settings.value(STR_GAME_OVERRIDE).toString();
		} else {
			gameRoot = this->gameDefault;
		}
		if (!QDir::isAbsolutePath(gameRoot)) {
			gameRoot = ::getRootPath() + QDir::separator() + gameRoot;
		}
		NewAddonDialog::open(gameRoot, this);
	});

	gameMenu->addSeparator();

	this->game_overrideGame = gameMenu->addAction(tr("Override Game Folder"), [this] {
		const auto rootPath = ::getRootPath();
		if (auto path = QFileDialog::getExistingDirectory(this, tr("Override Game Folder"), rootPath); !path.isEmpty()) {
			const QDir rootDir{rootPath};
			QSettings settings;
			settings.setValue(STR_GAME_OVERRIDE, QDir::cleanPath(rootDir.relativeFilePath(path)));
			this->loadGameConfig(settings.value(STR_RECENT_CONFIGS).toStringList().first());
		}
	});

	this->game_resetToDefault = gameMenu->addAction(tr("Reset to Default"), [this] {
		QSettings settings;
		settings.remove(STR_GAME_OVERRIDE);
		this->loadGameConfig(settings.value(STR_RECENT_CONFIGS).toStringList().first());
	});

	// Help menu
	auto* toolsMenu = this->menuBar()->addMenu(tr("Tools"));

	toolsMenu->addAction(tr("Configure Paths"),[]() {
		QMessageBox::warning(nullptr, "Feature not implemented", "TODO");
	} );

	toolsMenu->addAction(tr("Generate a MegaTexture" ),[]() {
		QMessageBox::warning(nullptr, "Feature not implemented", "TODO");
	} );

	// Help menu
	auto* helpMenu = this->menuBar()->addMenu(tr("Help"));

	helpMenu->addAction(tr("Official Modding Wiki"),[]() {
		QDesktopServices::openUrl( QUrl( "https://wiki.splashdamage.com/index.php/Main_Page" ) );
	} );
	helpMenu->addAction(tr("Official Community Site"),[]() {
		QDesktopServices::openUrl( QUrl( "http://community.enemyterritory.com" ) );
	} );
	helpMenu->addAction(tr("Official ET:QW Site"),[]() {
		QDesktopServices::openUrl( QUrl( "http://www.enemyterritory.com/" ) );
	} );
	helpMenu->addAction(tr("Official Modding Forums"),[]() {
		QDesktopServices::openUrl( QUrl( "http://community.enemyterritory.com/forums/forumdisplay.php?f=24" ) );
	} );

	helpMenu->addSeparator();

	helpMenu->addAction(this->style()->standardIcon(QStyle::SP_DialogHelpButton), tr("About"), Qt::Key_F1, [this] {
		QMessageBox about(this);
		about.setWindowTitle(tr("About"));
		about.setIconPixmap(QIcon{ getSDKLauncherIconPath() }.pixmap(125, 125));
		about.setTextFormat(Qt::TextFormat::MarkdownText);
		if (QFile file(":/markdown/about.md"); file.open(QIODevice::ReadOnly)) {
			QTextStream in(&file);
			QString mdContent = in.readAll();
			file.close();

			QString parse = mdContent.arg( PROJECT_TITLE.data() ).arg( PROJECT_VERSION.data() ).arg( QString::number( QDate::currentDate().year() ) );
			about.setText( parse );
		}
		about.exec();
	});

	helpMenu->addAction(this->style()->standardIcon(QStyle::SP_DialogHelpButton), tr("About Qt"), Qt::ALT | Qt::Key_F1, [this] {
		QMessageBox::aboutQt(this);
	});

	// Add content
	auto* scrollArea = new QScrollArea;
	scrollArea->setFrameStyle(0);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	scrollArea->setWidgetResizable(true);

	this->main = new QWidget;
	scrollArea->setWidget(this->main);
	this->setCentralWidget(scrollArea);

	new QVBoxLayout(this->main);

	if (QSettings settings; !settings.contains(STR_RECENT_CONFIGS)) {
		settings.setValue(STR_RECENT_CONFIGS, QStringList{});
		if (auto defaultConfigPath = QCoreApplication::applicationDirPath() + "/SDKLauncherDefault.json"; QFile::exists(defaultConfigPath)) {
			this->loadGameConfig(defaultConfigPath);
		} else {
			this->loadGameConfig(QString(":/config/%1.json").arg(PROJECT_DEFAULT_MOD.data()));
		}
	} else {
		this->loadGameConfig(settings.value(STR_RECENT_CONFIGS).value<QStringList>().first());
	}
}

QString Window::getSplashDamageIconPath() {
	return ":/icons/splashdamage.png";
}

QString Window::getSDKLauncherIconPath() {
	if constexpr (PROJECT_DEFAULT_MOD == "game_default") {
		return ":/icons/etqw_sdk.png";
	} else {
		return getSplashDamageIconPath();
	}
}

void Window::loadGameConfig(const QString& path) {
	auto* layout = dynamic_cast<QVBoxLayout*>(this->main->layout());
	::clearLayout(layout);

	auto gameConfig = GameConfig::parse(path);
	if (!gameConfig) {
		auto* test = new QLabel(tr("Invalid game configuration."), this->main);
		layout->addWidget(test);
		layout->addStretch();
		return;
	}

	this->utilities_createNewAddon->setDisabled(!gameConfig->supportsAddons());

	QSettings settings;
	auto recentConfigs = settings.value(STR_RECENT_CONFIGS).value<QStringList>();
	if (recentConfigs.contains(path)) {
		recentConfigs.removeAt(recentConfigs.indexOf(path));
	}
	recentConfigs.push_front(path);
	if (recentConfigs.size() > 10) {
		recentConfigs.pop_back();
	}
	settings.setValue(STR_RECENT_CONFIGS, recentConfigs);
	this->regenerateRecentConfigs();

	// Set ${ROOT}
	const auto rootPath = ::getRootPath();
	gameConfig->setVariable("ROOT", rootPath);

	// Set ${PLATFORM} and ${PLATFORM64}
	gameConfig->setVariable("PLATFORM", "");
	gameConfig->setVariable("PLATFORM64", "x64");

	// Set ${SPLASHDAMAGE_ICON}
	gameConfig->setVariable("SPLASHDAMAGE_ICON", getSplashDamageIconPath());

	// Set ${SDKLAUNCHER_ICON}
	gameConfig->setVariable("SDKLAUNCHER_ICON", getSDKLauncherIconPath());

	// Get default game
	this->gameDefault = gameConfig->getGameDefault();

	// massive hack: set the icons for the tools
	gameConfig->setVariable("WORLD_ICON", ":/icons/worldEditor.png");
	gameConfig->setVariable("ATMOS_ICON", ":/icons/atmosEditor.png");
	gameConfig->setVariable("AMBIENT_ICON", ":/icons/ambientEditor.png");
	gameConfig->setVariable("ATLAS_ICON", ":/icons/atlasEditor.png");
	gameConfig->setVariable("SOUND_ICON", ":/icons/soundEditor.png");
	gameConfig->setVariable("FX_ICON", ":/icons/fxEditor.png");
	gameConfig->setVariable("AF_ICON", ":/icons/afEditor.png");
	gameConfig->setVariable("STAMP_ICON", ":/icons/stampsEditor.png");

	// tiny hack: get default game icon before ${GAME} substitution
	QString defaultGameIconPath = gameConfig->getGameIcon();
	defaultGameIconPath.replace("${GAME}", this->gameDefault);
	if (QIcon defaultGameIcon{defaultGameIconPath}; !defaultGameIcon.isNull() && !defaultGameIcon.availableSizes().isEmpty()) {
		this->config_loadDefault->setIcon(defaultGameIcon);
		this->game_resetToDefault->setIcon(defaultGameIcon);
	} else {
		this->config_loadDefault->setIcon(this->style()->standardIcon(QStyle::SP_FileLinkIcon));
		this->game_resetToDefault->setIcon(this->style()->standardIcon(QStyle::SP_FileLinkIcon));
	}

	// Set ${GAME}
	QString gameDir = settings.contains(STR_GAME_OVERRIDE) ? settings.value(STR_GAME_OVERRIDE).toString() : this->gameDefault;
	gameConfig->setVariable("GAME", gameDir);

	// Set ${GAME_ICON}
	if (QIcon gameIcon{gameConfig->getGameIcon()}; !gameIcon.isNull() && !gameIcon.availableSizes().isEmpty()) {
		this->game_overrideGame->setIcon(gameIcon);
		gameConfig->setVariable("GAME_ICON", gameConfig->getGameIcon());
	} else {
		this->game_overrideGame->setIcon(this->style()->standardIcon(QStyle::SP_FileLinkIcon));
		gameConfig->setVariable("GAME_ICON", "");
	}

	for (int i = 0; i < gameConfig->getSections().size(); i++) {
		auto& section = gameConfig->getSections()[i];

		auto* name = new QLabel(section.name, this->main);
		name->setStyleSheet("QLabel { font-size: 11pt; }");
		layout->addWidget(name);

		auto* line = new QFrame(this->main);
		line->setFrameShape(QFrame::HLine);
		layout->addWidget(line);

		for (auto& entry : section.entries) {
			auto* button = new LaunchButton(this->main);
			button->setStyleSheet(
				"LaunchButton          { background-color: rgba(0, 0, 0,  0); border: none; }\n"
				"LaunchButton::pressed { background-color: rgba(0, 90, 250, 32); border: none; }\n"
				"LaunchButton::hover   { background-color: rgba(0, 60, 250, 32); border: none; }" );
			button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			button->setText(entry.name);
			button->setIconSize({24, 24});
			button->setFixedWidth(gameConfig->getWindowWidth() - 28);
			layout->addWidget(button);

			bool iconSet = false;
			if (!entry.iconOverride.isEmpty()) {
				button->setIcon(QIcon{entry.iconOverride});
				iconSet = true;
			}

			QString action = entry.action;
			if (action.endsWith('/') || action.endsWith('\\')) {
				action = action.sliced(0, action.size() - 1);
			}
			switch (entry.type) {
				case GameConfig::ActionType::INVALID:
					button->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxCritical));
					button->setToolTip(tr("This button has an invalid type. Check the config for any spelling errors."));
					break;
				case GameConfig::ActionType::COMMAND:
					if (!iconSet) {
#ifdef _WIN32
						if (auto icon = ::getExecutableIcon(action + ".exe"); !icon.isNull()) {
							button->setIcon(icon);
						} else {
							button->setIcon(this->style()->standardIcon(QStyle::SP_FileLinkIcon));
						}
#else
						button->setIcon(this->style()->standardIcon(QStyle::SP_FileLinkIcon));
#endif
					}
					button->setToolTip(action + " " + entry.arguments.join(" "));
					QObject::connect(button, &LaunchButton::doubleClicked, this, [this, action, args=entry.arguments, cwd=rootPath] {
						auto* process = new QProcess;
						QObject::connect(process, &QProcess::errorOccurred, this, [this, timeStart = std::chrono::steady_clock::now()](QProcess::ProcessError code) {
							QString error;
							switch (code) {
								case QProcess::FailedToStart:
									error = tr("The process failed to start. Perhaps the executable it points to might not exist?");
									break;
								case QProcess::Crashed: {
									auto timeEnd = std::chrono::steady_clock::now();
									if (std::chrono::duration<float, std::milli>(timeEnd - timeStart).count() > 30'000) {
										return;
									}
									error = tr("The process crashed.");
									break;
								}
								case QProcess::Timedout:
									error = tr("The process timed out.");
									break;
								case QProcess::ReadError:
								case QProcess::WriteError:
									error = tr("The process hit an I/O error.");
									break;
								case QProcess::UnknownError:
									error = tr("The process hit an unknown error.");
									break;
							}
							QMessageBox::critical(this, tr("Error"), tr("An error occurred executing this command: %1").arg(error));
						});
						process->setWorkingDirectory(cwd);
						process->start(action, args);
					});
					break;
				case GameConfig::ActionType::LINK:
					if (!iconSet) {
						button->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxInformation));
					}
					button->setToolTip(action);
					QObject::connect(button, &LaunchButton::doubleClicked, this, [action] {
						QDesktopServices::openUrl({action});
					});
					break;
				case GameConfig::ActionType::DIRECTORY:
					if (!iconSet) {
						button->setIcon(this->style()->standardIcon(QStyle::SP_DirLinkIcon));
					}
					button->setToolTip(action);
					QObject::connect(button, &LaunchButton::doubleClicked, this, [action] {
						QDesktopServices::openUrl({QString("file:///") + action});
					});
					break;
			}
		}

		if (i + 1 != gameConfig->getSections().size()) {
			layout->addSpacing(16);
		}
	}

	layout->addStretch();

	// Set window sizing
	this->resize(gameConfig->getWindowWidth(), gameConfig->getWindowHeight());
	this->setFixedWidth(gameConfig->getWindowWidth());
}

void Window::regenerateRecentConfigs() {
	this->recent->clear();

	auto paths = QSettings().value(STR_RECENT_CONFIGS).value<QStringList>();
	if (paths.empty()) {
		auto* noRecentFilesAction = this->recent->addAction(tr("No recent files."));
		noRecentFilesAction->setDisabled(true);
		return;
	}
	for (int i = 0; i < paths.size(); i++) {
		this->recent->addAction(("&%1: \"" + paths[i] + "\"").arg((i + 1) % 10), [this, path=paths[i]] {
			this->loadGameConfig(path);
		});
	}
	this->recent->addSeparator();
	this->recent->addAction(tr("Clear"), [this] {
		QSettings().setValue(STR_RECENT_CONFIGS, QStringList{});
		this->regenerateRecentConfigs();
	});
}
