#pragma once
#include "common.h"
#include "core/startup.h"
#include <dpp/dpp.h>

namespace lingling::discord
{
    /**
     * @brief Creates the /setup command group designed to handle the setup of a
     * category and channel with the relevant permissions.
     *
     * @return The slashcommand and the function to handle the command being triggered.
     */
    [[nodiscard]] command_register_t create_setup_command(const dpp::snowflake& app_id);

    void add_setup_command_create_listener(command_create_t callback);

    discord::command_callback_t add_setup_tribelog_option(dpp::slashcommand& cmd);

    void init_tribelog_module();

    inline run_on_startup tribelog_load{ startup_time::STARTUP_EARLY, init_tribelog_module  };

}
