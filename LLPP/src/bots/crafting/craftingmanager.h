#pragma once
#include "../../core/istationmanager.h"
#include "../../core/discord.h"
#include "sparkpowderstation.h"
#include "gunpowderstation.h"

namespace llpp::bots::crafting
{
    class CraftingManager final : public core::IStationManager
    {
    public:
        CraftingManager();
        ~CraftingManager() override;

        bool run() override;
        [[nodiscard]] bool is_ready_to_run() const override;
        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    private:
        void register_slash_commands();
        static void slashcommand_callback(const dpp::slashcommand_t& event);

        bool has_registered_commands_ = false;

        std::vector<std::unique_ptr<SparkpowderStation>> spark_stations_;
        std::vector<std::unique_ptr<GunpowderStation>> gunpowder_stations_;
    };
}