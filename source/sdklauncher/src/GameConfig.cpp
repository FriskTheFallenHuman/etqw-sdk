#include "precompiled.h"
#pragma hdrstop

GameConfig::ActionType GameConfig::actionTypeFromString(const QString& string) {
	if (string == "command") {
		return ActionType::COMMAND;
	} else if (string == "link") {
		return ActionType::LINK;
	} else if (string == "directory") {
		return ActionType::DIRECTORY;
	}
	return ActionType::INVALID;
}

GameConfig::OS GameConfig::osFromString(const QString& string) {
	auto out = static_cast<unsigned char>(OS::NONE);
	if (string.contains("windows")) {
		out |= static_cast<unsigned char>(OS::WINDOWS);
	}
	if (string.contains("linux")) {
		out |= static_cast<unsigned char>(OS::LINUX);
	}
	if (out == static_cast<unsigned char>(OS::NONE)) {
		out = static_cast<unsigned char>(OS::ALL);
	}
	return static_cast<OS>(out);
}

std::optional<GameConfig> GameConfig::parse(const QString& path) {
	QString config;
	if (QFile file(path); file.open(QIODevice::ReadOnly)) {
		QTextStream in(&file);
		config += in.readAll();
		file.close();
	} else {
		return std::nullopt;
	}

	QJsonDocument configJson = QJsonDocument::fromJson(config.toUtf8());
	if (!configJson.isObject()) {
		return std::nullopt;
	}
	QJsonObject configObject = configJson.object();

	GameConfig gameConfig;

	if (!configObject.contains("game_default") || !configObject["game_default"].isString()) {
		return std::nullopt;
	}
	gameConfig.gameDefault = configObject["game_default"].toString();

	if (configObject.contains("game_icon") && configObject["game_icon"].isString()) {
		gameConfig.gameIcon = configObject["game_icon"].toString();
	} else {
		gameConfig.gameIcon = "${ROOT}/${GAME}/game.ico";
	}

	if (configObject.contains("window_height")) {
		gameConfig.windowHeight = configObject["window_height"].toInt(DEFAULT_WINDOW_HEIGHT);
	}

	if (configObject.contains("window_width")) {
		gameConfig.windowWidth = configObject["window_width"].toInt(DEFAULT_WINDOW_WIDTH);
	}

	if (configObject.contains("supports_addons") && configObject["supports_addons"].isBool()) {
		gameConfig.addonsSupported = configObject["supports_addons"].toBool();
	}

	if (!configObject.contains("sections") || !configObject["sections"].isArray()) {
		return std::nullopt;
	}
	QJsonArray sections = configObject["sections"].toArray();

	for (const auto& sectionValue : sections) {
		if (!sectionValue.isObject()) {
			continue;
		}

		QJsonObject sectionObject = sectionValue.toObject();
		if (!sectionObject.contains("name") || !sectionObject["name"].isString() || !sectionObject.contains("entries") || !sectionObject["entries"].isArray()) {
			continue;
		}

		auto& gameConfigSection = gameConfig.sections.emplace_back();
		gameConfigSection.name = sectionObject["name"].toString();

		auto entries = sectionObject["entries"].toArray();
		for (const auto& entryValue : entries) {
			if (!entryValue.isObject()) {
				continue;
			}

			QJsonObject entryObject = entryValue.toObject();
			if (!entryObject.contains("name") || !entryObject["name"].isString() || !entryObject.contains("type") || !entryObject["type"].isString() || !entryObject.contains("action") || !entryObject["action"].isString()) {
				continue;
			}

			auto& gameConfigSectionEntry = gameConfigSection.entries.emplace_back();
			gameConfigSectionEntry.name = entryObject["name"].toString();
			gameConfigSectionEntry.type = actionTypeFromString(entryObject["type"].toString());
			gameConfigSectionEntry.action = entryObject["action"].toString();

			if (entryObject.contains("arguments") && entryObject["arguments"].isArray()) {
				for (const auto& argument : entryObject["arguments"].toArray()) {
					if (!argument.isString()) {
						continue;
					}
					gameConfigSectionEntry.arguments.push_back(argument.toString());
				}
			}

			if (entryObject.contains("icon_override") && entryObject["icon_override"].isString()) {
				gameConfigSectionEntry.iconOverride = entryObject["icon_override"].toString();
			}

			auto os = static_cast<unsigned char>((!entryObject.contains("os") || !entryObject["os"].isString()) ? OS::ALL : osFromString(entryObject["os"].toString()));
#if defined(_WIN32)
			if (!(os & static_cast<unsigned char>(OS::WINDOWS))) {
				gameConfigSection.entries.pop_back();
			}
#elif defined(__linux__)
			if (!(os & static_cast<unsigned char>(OS::LINUX))) {
				gameConfigSection.entries.pop_back();
			}
#endif
		}

		if (gameConfigSection.entries.empty()) {
			gameConfig.sections.pop_back();
		}
	}
	return gameConfig;
}

void GameConfig::setVariable(const QString& variable, const QString& replacement) {
	const auto setVar = [&variable, &replacement](QString& str) {
		str.replace(QString("${%1}").arg(variable), replacement);
	};
	setVar(this->gameDefault);
	setVar(this->gameIcon);
	for (auto& section : this->sections) {
		setVar(section.name);
		for (auto& entry : section.entries) {
			setVar(entry.name);
			setVar(entry.action);
			for (auto& argument : entry.arguments) {
				setVar(argument);
			}
			setVar(entry.iconOverride);
		}
	}
}
