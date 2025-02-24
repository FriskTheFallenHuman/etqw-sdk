#pragma once

class NewAddonDialog : public QDialog {
	Q_OBJECT;

public:
	NewAddonDialog(QString gameRoot_, QWidget* parent = nullptr);

	[[nodiscard]] QString getAddonInstallDir() const;

	static void open(QString gameRoot, QWidget* parent = nullptr);

private:
	QString gameRoot;

	QLineEdit* addonID;
	QLineEdit* addonName;
	QLineEdit* addonCheckSum;
	QComboBox* addonType;
	QCheckBox* addonVisible;
	QCheckBox* addShortcutOnDesktop;
};
