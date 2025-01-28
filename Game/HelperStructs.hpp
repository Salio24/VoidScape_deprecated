#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>

enum class Levels {
	LEVEL_1,
	TEST_LEVEL,
	END
};

struct EnumClassHash
{
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};

namespace std {
	template <> struct hash<Levels> {
		size_t operator() (const Levels& t) const { return size_t(t); }
	};
}

struct Box {
	glm::vec2 Position;
	glm::vec4 Color;
	glm::vec2 Size;
	glm::vec2 TexturePosition;
	int TextureIndex;
	float RotationalAngle;
	float AngleModifier;
	glm::vec2 TranslateMargin;
};

struct Text {
	glm::vec2 Possition{0.0f, 0.0f};
	glm::vec3 Color{0.0f, 0.0f, 0.0f};
	std::string Text{""};
	glm::mat4 *ModelMatrix;
	bool CenteredX{ true };
	bool CenteredY{ true };
	bool RightSided{ false };
	float Scale;
};

struct Settings {
	bool VSync{ false };
	bool debugMode{ true };
	bool autoRestart{ false };

	int SFXVolume{ 3 };
	int MusicVolume{ 3 };

	float textSizeMultiplier{ 800.0f };

	glm::ivec2 screenSize{ 0 };

	int currentWindowModeIndex{ 0 };
	int currentResolutionIndex{ 4 };

	glm::ivec2 viewportOffset{ 0 };
};

struct SaveData {
	int Level_1_best_levelTime_minutes{ 0 };
	int Level_1_best_levelTime_seconds{ 0 };
	int Level_1_best_levelTime_centiseconds{ 0 };

	std::unordered_map<Levels, int> best_levelTime_minutesMap;
	std::unordered_map<Levels, int> best_levelTime_secondsMap;
	std::unordered_map<Levels, int> best_levelTime_centisecondsMap;

};