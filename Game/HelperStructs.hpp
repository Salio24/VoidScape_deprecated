#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <nlohmannjson/json.hpp>
#include <SDL3/SDL.h>

using json = nlohmann::json;

enum class Levels {
	LEVEL_1,
	TEST_LEVEL,
	END
};

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
	bool debugMode{ false };
	bool autoRestart{ false };
	bool showFpsInGame{ true };

	int SFXVolume{ 5 };
	int MusicVolume{ 5 };

	float textSizeMultiplier{ 800.0f };

	glm::ivec2 screenSize{ 0 };

	int currentWindowModeIndex{ 0 };
	int currentResolutionIndex{ 4 };

	glm::ivec2 viewportOffset{ 0 };

	friend void to_json(json& j, const Settings& s) {
		j = json{
			{"VSync", s.VSync},
			{"debugMode", s.debugMode},
			{"autoRestart", s.autoRestart},
			{"SFXVolume", s.SFXVolume},
			{"MusicVolume", s.MusicVolume},
			{"textSizeMultiplier", s.textSizeMultiplier},
			{"currentWindowModeIndex", s.currentWindowModeIndex},
			{"currentResolutionIndex", s.currentResolutionIndex},
			{"showFpsInGame", s.showFpsInGame}
		};
	}

	friend void from_json(const json& j, Settings& s) {
		j.at("VSync").get_to(s.VSync);
		j.at("debugMode").get_to(s.debugMode);
		j.at("autoRestart").get_to(s.autoRestart);
		j.at("SFXVolume").get_to(s.SFXVolume);
		j.at("MusicVolume").get_to(s.MusicVolume);
		j.at("textSizeMultiplier").get_to(s.textSizeMultiplier);
		j.at("currentWindowModeIndex").get_to(s.currentWindowModeIndex);
		j.at("currentResolutionIndex").get_to(s.currentResolutionIndex);
		j.at("showFpsInGame").get_to(s.showFpsInGame);
	}
};

struct SaveData {
	int Level_1_best_levelTime_minutes{ 0 };
	int Level_1_best_levelTime_seconds{ 0 };
	int Level_1_best_levelTime_centiseconds{ 0 };

	friend void to_json(json& j, const SaveData& s) {
		j = json{
			{"Level_1_TimeM", s.Level_1_best_levelTime_minutes},
			{"Level_1_TimeS", s.Level_1_best_levelTime_seconds},
			{"Level_1_TimeCS", s.Level_1_best_levelTime_centiseconds},
		};
	}

	friend void from_json(const json& j, SaveData& s) {
		j.at("Level_1_TimeM").get_to(s.Level_1_best_levelTime_minutes);
		j.at("Level_1_TimeS").get_to(s.Level_1_best_levelTime_seconds);
		j.at("Level_1_TimeCS").get_to(s.Level_1_best_levelTime_centiseconds);
	}
};

struct Controls {
	SDL_Scancode LEFT_KEY_BIND{ SDL_SCANCODE_A };
	SDL_Scancode RIGHT_KEY_BIND{ SDL_SCANCODE_D };
	SDL_Scancode JUMP_KEY_BIND{ SDL_SCANCODE_SPACE };
	SDL_Scancode DUCK_KEY_BIND{ SDL_SCANCODE_LSHIFT };

	friend void to_json(json& j, const Controls& s) {
		j = json{
			{"LEFT_KEY_BIND", SDL_GetKeyName(SDL_GetKeyFromScancode(s.LEFT_KEY_BIND, SDL_KMOD_NONE, false))},
			{"RIGHT_KEY_BIND", SDL_GetKeyName(SDL_GetKeyFromScancode(s.RIGHT_KEY_BIND, SDL_KMOD_NONE, false))},
			{"JUMP_KEY_BIND", SDL_GetKeyName(SDL_GetKeyFromScancode(s.JUMP_KEY_BIND, SDL_KMOD_NONE, false))},
			{"DUCK_KEY_BIND", SDL_GetKeyName(SDL_GetKeyFromScancode(s.DUCK_KEY_BIND, SDL_KMOD_NONE, false))},

		};
	}

	friend void from_json(const json& j, Controls& s) {
		auto parseKey = [](const std::string& keyName, SDL_Scancode defaultScancode) {
			SDL_Keycode keycode = SDL_GetKeyFromName(keyName.c_str());
			if (keycode == SDLK_UNKNOWN) {
				return defaultScancode;  // Fallback to default if invalid
			}
			return SDL_GetScancodeFromKey(keycode, SDL_KMOD_NONE);
			};

		s.LEFT_KEY_BIND = parseKey(j.value("LEFT_KEY_BIND", "A"), SDL_SCANCODE_A);
		s.RIGHT_KEY_BIND = parseKey(j.value("RIGHT_KEY_BIND", "D"), SDL_SCANCODE_D);
		s.JUMP_KEY_BIND = parseKey(j.value("JUMP_KEY_BIND", "Space"), SDL_SCANCODE_SPACE);
		s.DUCK_KEY_BIND = parseKey(j.value("DUCK_KEY_BIND", "Left Shift"), SDL_SCANCODE_LSHIFT);
	}
};