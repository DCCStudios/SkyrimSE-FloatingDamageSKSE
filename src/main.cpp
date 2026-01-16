#include "PCH.h"
#include "Hooks.h"
#include "Menu.h"
#include "Settings.h"

namespace Plugin
{
    inline constexpr REL::Version VERSION{ 1, 0, 0, 0 };
    inline constexpr auto NAME = "FloatingDamageSKSE"sv;
}

namespace
{
    void InitializeLog()
    {
        auto path = logger::log_directory();
        if (!path) {
            stl::report_and_fail("Failed to find standard logging directory"sv);
        }

        *path /= fmt::format("{}.log"sv, Plugin::NAME);
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

        auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
        log->set_level(spdlog::level::info);
        log->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("[%H:%M:%S.%e] [%l] %v");
    }

    void OnMessage(SKSE::MessagingInterface::Message* a_msg)
    {
        if (!a_msg) {
            return;
        }

        switch (a_msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            Settings::GetSingleton()->Load();
            Hooks::Install();
            Menu::Register();
            RE::DebugNotification("FloatingDamageSKSE loaded");
            logger::info("Floating Damage SKSE initialized");
            break;
        default:
            break;
        }
    }
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
    a_info->infoVersion = SKSE::PluginInfo::kVersion;
    a_info->name = Plugin::NAME.data();
    a_info->version = Plugin::VERSION.pack();

    if (a_skse->IsEditor()) {
        return false;
    }

    const auto ver = a_skse->RuntimeVersion();
    if (ver < SKSE::RUNTIME_SSE_1_5_39) {
        return false;
    }

    return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v;
    v.PluginVersion(Plugin::VERSION);
    v.PluginName(Plugin::NAME);
    v.AuthorName("DCCStudios");
    v.UsesAddressLibrary();
    v.UsesNoStructs();
    v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });
    return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
    InitializeLog();
    logger::info("{} v{} loading...", Plugin::NAME, Plugin::VERSION.string());

    SKSE::Init(a_skse);

    auto messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener(OnMessage)) {
        logger::error("Failed to register SKSE message listener");
        return false;
    }

    return true;
}

