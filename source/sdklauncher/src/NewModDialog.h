#pragma once

class NewModDialog : public QDialog {
	Q_OBJECT;

public:
	NewModDialog(QString gameRoot_, QWidget* parent = nullptr);

	[[nodiscard]] QString getModInstallDirParent() const;

	[[nodiscard]] QString getModInstallDir() const;

	static void open(QString gameRoot, QWidget* parent = nullptr);

private:
	QString gameRoot;

	QComboBox* parentFolder;
	QLineEdit* parentFolderCustom;
	QLineEdit* modID;
	QCheckBox* addShortcutOnDesktop;
};
