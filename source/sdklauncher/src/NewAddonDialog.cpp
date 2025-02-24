#include "precompiled.h"
#pragma hdrstop

NewAddonDialog::NewAddonDialog(QString gameRoot_, QWidget* parent)
	: QDialog(parent)
	, gameRoot(std::move(gameRoot_)) {
	// Window setup
	this->setModal(true);
	this->setWindowTitle(tr("New Addon Wizard"));
	this->setMinimumWidth(350);

	// Create UI elements
	auto* layout = new QFormLayout{this};

	this->addonID = new QLineEdit{this};
	this->addonID->setPlaceholderText(tr("For example: my_map, mycampaign"));
	this->addonID->setToolTip(tr("This is use for identification purposes, keep it lower case."));
	layout->addRow(tr("Addon ID"), this->addonID);

	this->addonName = new QLineEdit{this};
	this->addonName->setPlaceholderText(tr("Name of your addon/campaign"));
	layout->addRow(tr("Addon Name"), this->addonName);

	this->addonCheckSum = new QLineEdit{this};
	this->addonCheckSum->setPlaceholderText(tr("0x00000000"));
	this->addonCheckSum->setToolTip(tr("Pak dependency checksums go here you can obtain the checksum for a pk4 by loading the game,\nsetting 'developer 1' and typing 'path' into the console"));
	layout->addRow(tr("Addon Checksum"), this->addonCheckSum);

	this->addonType = new QComboBox{this};
	this->addonType->addItem("Campaign");
	this->addonType->addItem("Map");
	this->addonType->setToolTip(tr("Determines the type of addon to be create only two types of addons are supported: Map and Campaign"));
	layout->addRow(tr("Addon Type"), this->addonType);

	this->addonVisible = new QCheckBox{this};
	this->addonVisible->setCheckState(Qt::Checked);
	layout->addRow(tr("Show Addon In Server Browser"), this->addonVisible);

	this->addShortcutOnDesktop = new QCheckBox{this};
	this->addShortcutOnDesktop->setCheckState(Qt::Unchecked);
	layout->addRow(tr("Create Desktop Shortcut"), this->addShortcutOnDesktop);

	auto* buttonBox = new QDialogButtonBox{ QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this };
	layout->addWidget(buttonBox);

	// Connect ok/cancel buttons to download stuff
	buttonBox->show();
	QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, [this] {
		// Validate mod ID
		if (this->addonID->text().trimmed().isEmpty()) {
			QMessageBox::critical(this, tr("Incorrect Input"), tr("Addon ID must not be empty."));
			return;
		}

		const auto addonInstallDir = this->getAddonInstallDir();

		// Validate install location
		if (std::filesystem::exists(addonInstallDir.toLocal8Bit().constData())) {
			QMessageBox::critical(this, tr("Incorrect Input"), tr("A folder with the name of the addon ID already exists at this install location."));
			return;
		}

		// Create directory structure
		QDir dir{this->getAddonInstallDir()};
		dir.mkpath("maps");
		dir.mkpath("mapinfo");
		dir.mkpath("materials");
		dir.mkpath("scripts");

		QMap<QString, QString> addonTypeMap;
		addonTypeMap["Map"] = "mapMetaData";
		addonTypeMap["Campaign"] = "campaignMetaData";

		// Create addon.conf
		QFile addonConf{ this->getAddonInstallDir() + QDir::separator() + "addon.conf" };
		addonConf.open(QIODevice::WriteOnly | QIODevice::Text);

		static constexpr auto addonConfContentsBase = R"(addonDef {
	"%3"
}

%5 maps/%1 {
	"pretty_name"		"%2"
	"mapinfo" "%1"
	"dz_deployInfo" "%1"
	"server_shot_thumb"	"levelshots/thumbs/%1.tga"
	"show_in_browser"	"%4"
})";
		{
			QTextStream out{&addonConf};
			QString addonTypeKey = this->addonType->currentText();
			QString addonTypeStr = addonTypeMap.value(addonTypeKey, "mapMetaData");
			out << QString{addonConfContentsBase}.arg(this->addonID->text(), this->addonName->text(), this->addonCheckSum->text(), QString::number(this->addonVisible->isChecked() ? 1 : 0), addonTypeStr).toLocal8Bit();
		}
		addonConf.close();

		// Create <mapinfo>/mapname.md
		QFile mapInfoMd{ this->getAddonInstallDir() + QDir::separator() + "mapinfo" + QDir::separator() + this->addonID->text() + ".md" };
		mapInfoMd.open(QIODevice::WriteOnly | QIODevice::Text);

		static constexpr auto mapInfoContentsBase = R"(#include <mapinfo/mapinfo.include>

mapInfoDef "%1" {
	_default_mapinfo( "%1" )

	data {
		"mapLocation"			"maps/%1/location"
		"script_entrypoint"		"EtqwMap_MapScript" // You must change this before shipping
		"mapBriefing"			"maps/%1/briefing"
		"campaignDescription"	"maps/%1/campaign"
		"numObjectives"			"4"
		"mtr_serverShot"		"levelshots/%1"
		"mtr_backdrop"			"levelshots/campaigns/northamerica"
		"mapPosition"			"311 140"
		"snd_music"				"sounds/music/load1"
		"strogg_endgame_pause"	"6.0"
		"gdf_endgame_pause"		"5.0"
	}
})";
		{
			QTextStream out{&mapInfoMd};
			out << QString{mapInfoContentsBase}.arg(this->addonID->text()).toLocal8Bit();
		}
		mapInfoMd.close();

		// Create <materials>/gui_mapname.md
		QFile mapMat{ this->getAddonInstallDir() + QDir::separator() + "materials" + QDir::separator() + "gui_" + this->addonID->text() + ".mtr" };
		mapMat.open(QIODevice::WriteOnly | QIODevice::Text);

		static constexpr auto mapMatContentsBase = R"(material levelshots/%1 {
	sort gui
	{
		blend blend
		vertexColor
		map nopicmip clamp	"levelshots/%1"
	}
}

material levelshots/thumbs/%1 {
	sort gui
	{
		blend blend
		vertexColor
		map nopicmip clamp	"levelshots/thumbs/%1"
	}
})";
		{
			QTextStream out{&mapMat};
			out << QString{mapMatContentsBase}.arg(this->addonID->text()).toLocal8Bit();
		}
		mapMat.close();

		// Create desktop shortcut
		if (this->addShortcutOnDesktop->isChecked()) {
			const auto shortcutPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QDir::separator() + this->addonID->text().trimmed();
#ifdef _WIN32
			QFile::link(addonInstallDir, shortcutPath + ".lnk");
#else
			QFile::link(addonInstallDir, shortcutPath);
#endif
		}
		QMessageBox::information(this, tr("Info"), tr("Your addon has been created to ET:QW game folder, which means it will load into the game! This requires you to restart the game once."));
		QDesktopServices::openUrl({QString("file:///") + addonInstallDir});

		this->accept();
		});
	QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &NewAddonDialog::reject);
}

QString NewAddonDialog::getAddonInstallDir() const {
	return this->gameRoot + QDir::separator() + this->addonID->text().trimmed();
}

void NewAddonDialog::open(QString gameRoot, QWidget* parent) {
	auto* dialog = new NewAddonDialog{std::move(gameRoot), parent};
	dialog->exec();
	dialog->deleteLater();
}
