#pragma once

class Window : public QMainWindow {
	Q_OBJECT;

public:
	explicit Window(QWidget* parent = nullptr);

	[[nodiscard]] static QString getSplashDamageIconPath();

	[[nodiscard]] static QString getSDKLauncherIconPath();

	void loadGameConfig(const QString& path);

	void regenerateRecentConfigs();

private:
	QString gameDefault;

	QMenu* recent;
	QAction* config_loadDefault;
	QAction* game_overrideGame;
	QAction* game_resetToDefault;
	QAction* utilities_createNewMod;
	QAction* utilities_createNewAddon;

	QWidget* main;
};
