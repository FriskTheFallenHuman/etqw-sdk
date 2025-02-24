#include "precompiled.h"
#pragma hdrstop

NewModDialog::NewModDialog(QString gameRoot_, QWidget* parent)
		: QDialog(parent)
		, gameRoot(std::move(gameRoot_)) {

	// Window setup
	this->setModal(true);
	this->setWindowTitle(tr("New Mod Wizard"));
	this->setMinimumWidth(350);

	// Create UI elements
	auto* layout = new QFormLayout{this};

	this->parentFolder = new QComboBox{this};
	this->parentFolder->addItem(tr("Game Folder"));
	this->parentFolder->addItem(tr("Custom Location"));
	layout->addRow(tr("Install Location"), this->parentFolder);

	auto* parentFolderCustomLabel = new QLabel{tr("Custom Location"), this};
	this->parentFolderCustom = new QLineEdit{this};
	this->parentFolderCustom->setPlaceholderText(tr("path/to/mod/parent/folder"));
	layout->addRow(parentFolderCustomLabel, this->parentFolderCustom);

	this->modID = new QLineEdit{this};
	this->modID->setPlaceholderText(tr("For example: base, q4base"));
	layout->addRow(tr("Mod ID"), this->modID);

	this->addShortcutOnDesktop = new QCheckBox{this};
	this->addShortcutOnDesktop->setCheckState(Qt::Checked);
	layout->addRow(tr("Create Desktop Shortcut"), this->addShortcutOnDesktop);

	auto* buttonBox = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this};
	layout->addWidget(buttonBox);

	// We want the custom input to be invisible unless the combo box is on the custom option
	parentFolderCustomLabel->hide();
	this->parentFolderCustom->hide();
	QObject::connect(this->parentFolder, &QComboBox::currentIndexChanged, this, [this, parentFolderCustomLabel]( int index) {
		parentFolderCustomLabel->setVisible( index == 1 );
		this->parentFolderCustom->setVisible( index == 1 );
	} );

	// Connect ok/cancel buttons to download stuff
	buttonBox->show();
	QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, [this, buttonBox] {
		// Validate mod ID
		if (this->modID->text().trimmed().isEmpty()) {
			QMessageBox::critical(this, tr("Incorrect Input"), tr("Mod ID must not be empty."));
			return;
		}

		const auto modInstallDir = this->getModInstallDir();

		// Validate install location
		if (modInstallDir.isEmpty() || !std::filesystem::exists(this->getModInstallDirParent().toLocal8Bit().constData())) {
			QMessageBox::critical(this, tr("Incorrect Input"), tr("Install location does not exist."));
			return;
		}
		if (std::filesystem::exists(modInstallDir.toLocal8Bit().constData())) {
			QMessageBox::critical(this, tr("Incorrect Input"), tr("A folder with the name of the mod ID already exists at this install location."));
			return;
		}

		// Create directory structure
		QDir dir{this->getModInstallDir()};
		dir.mkpath("afs");
		dir.mkpath("defs");
		dir.mkpath("maps");
		dir.mkpath("materials");
		dir.mkpath("models");
		dir.mkpath("textures");
		dir.mkpath("sounds");
		dir.mkpath("scripts");

		// Create pakmeta.conf
		QFile pakmetaConf{this->getModInstallDir() + QDir::separator() + "pakmeta.conf" };
		pakmetaConf.open(QIODevice::WriteOnly | QIODevice::Text);

		static constexpr auto pakmetaConfContentsBase = R"(// Map Data

mapMetaData maps/sdk/etqwmap {
	"pretty_name" "ETQW map"
	"mapinfo" "etqwmap"
	"dz_deployInfo" "etqwmap"
	"server_shot_thumb" "levelshots/thumbs/etqwmap.tga"
	"show_in_browser" "1"
}
)";
		{
			QTextStream out{&pakmetaConf};
			out << QString{ pakmetaConfContentsBase };
		}
		pakmetaConf.close();

		// Create desktop shortcut
		if ( this->addShortcutOnDesktop->isChecked() ) {
			const auto shortcutPath = QStandardPaths::writableLocation( QStandardPaths::DesktopLocation ) + QDir::separator() + this->modID->text().trimmed();
#ifdef _WIN32
			QFile::link( modInstallDir, shortcutPath + ".lnk" );
#else
			QFile::link( modInstallDir, shortcutPath );
#endif
		}
		QMessageBox::information(this, tr("Info"), tr("Your mod folder has been created to ET:QW root folder, you can change to your mod from ETQW->Mods."));
		QDesktopServices::openUrl({QString( "file:///" ) + modInstallDir});

		this->accept();

	} );
	QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &NewModDialog::reject);
}

QString NewModDialog::getModInstallDirParent() const {
	auto selectedIndex = this->parentFolder->currentIndex();
	if (this->parentFolder->count() == 1) {
		selectedIndex++;
	}
	switch (selectedIndex) {
		case 0:
			return this->gameRoot;
		default:
		case 1:
			return this->parentFolderCustom->text();
	}
}

QString NewModDialog::getModInstallDir() const {
	return this->getModInstallDirParent() + QDir::separator() + this->modID->text().trimmed();
}

void NewModDialog::open(QString gameRoot, QWidget* parent) {
	auto* dialog = new NewModDialog{std::move(gameRoot), parent};
	dialog->exec();
	dialog->deleteLater();
}