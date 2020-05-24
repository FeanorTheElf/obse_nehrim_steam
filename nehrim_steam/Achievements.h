#pragma once
#include "steam\steam_api.h"

namespace Achievements {

	class AchievementHolder {
	private:
		ISteamUserStats * stats;
		CCallback<AchievementHolder, UserStatsReceived_t, false> callback;

	public:
		AchievementHolder();

		void requestStats();
		void onUserStatsReceived(UserStatsReceived_t * event);
		bool setAchievementUnlocked(const char * achievementName);
	};

	static std::unique_ptr<AchievementHolder> singleton(nullptr);

	void runSteamCallbacks();
	void startSteam();
	bool setAchievementUnlocked(const char * achievementCode);
}
