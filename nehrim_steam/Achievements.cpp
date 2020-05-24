#include "Achievements.h"

const char * nehrimSteamAppId = "1014940";

namespace Achievements {

	AchievementHolder::AchievementHolder(): stats(SteamUserStats()), callback(this, &AchievementHolder::onUserStatsReceived) {
	}

	void AchievementHolder::requestStats() {
		_MESSAGE("Begin requestion current user statistics");
		bool success = this->stats->RequestCurrentStats();
		std::string msg("Request current user statistics was successful: ");
		msg += std::to_string(success);
		_MESSAGE(msg.c_str());
	}

	void AchievementHolder::onUserStatsReceived(UserStatsReceived_t * event) {
		try {
			std::string msg = "User id: " + std::to_string(event->m_steamIDUser.ConvertToUint64()) + ", game id: " + std::to_string(event->m_nGameID) + ", success state: " + std::to_string(event->m_eResult);
			_MESSAGE(msg.c_str());

			uint32 achievementCount = this->stats->GetNumAchievements();
			msg = "There are " + std::to_string(achievementCount) + " achievements";
			_MESSAGE(msg.c_str());
		}
		catch (const std::exception& ex) {
			std::string msg = "Exception during steam callback: onUserStatsReceived. Failed to print data: " + std::string(ex.what());
			_MESSAGE(msg.c_str());
		}
	}

	bool AchievementHolder::setAchievementUnlocked(const char * achievementCode) {
		std::string msg = "Unlocking achievement: " + std::string(achievementCode);
		_MESSAGE(msg.c_str());
		bool success = this->stats->SetAchievement(achievementCode);
		if (!success) {
			_MESSAGE("Error while unlocking achievement");
			return false;
		}
		success = this->stats->StoreStats();
		if (!success) {
			_MESSAGE("Error while storing unlocked achievement");
		}
		return success;
	}

	void startSteam() {
		try {
			if (singleton.get() == nullptr) {
				SteamAPI_Shutdown();
				SetEnvironmentVariable("SteamAppID", nehrimSteamAppId);
				SetEnvironmentVariable("SteamGameId", nehrimSteamAppId);
				bool success = SteamAPI_Init();
				if (success)
					_MESSAGE("Steam api init was successfull");
				else
					_MESSAGE("Error while initializing the steam api");

				singleton.swap(std::make_unique<AchievementHolder>());
				singleton->requestStats();
			}
			else {
				_MESSAGE("Already initialized steam api, skipping it");
			}
		}
		catch (const std::exception& ex) {
			std::string msg = "Exception while initializing the Steam API, steam achievements will not be available: " + std::string(ex.what());
			_MESSAGE(msg.c_str());
		}
	}

	void runSteamCallbacks() {
		SteamAPI_RunCallbacks();
	}

	bool setAchievementUnlocked(const char * achievementCode) {
		if (singleton == nullptr) {
			_MESSAGE("AchievementHolder singleton was not initialized, was startSteam() called?");
		}
		return singleton->setAchievementUnlocked(achievementCode);
	}
}
