#include "validate.h"
#include "utility/utility.h"
#include "exceptions.h"

#include <set>
#include <asa/core/logging.h>

namespace lingling
{
    namespace
    {
        std::map<std::string, validation_callback_t> validation_callbacks;
    }

    void add_config_validation(const std::string& category, validation_callback_t fn)
    {
        // Make sure there isnt already callback for the category.
        if (validation_callbacks.contains(category)) {
            throw duplicate_category_callback(category);
        }
        validation_callbacks.emplace(category, std::move(fn));
        asa::get_logger()->debug("Validation callback attached for '{}'.", category);
    }

    bool patch_json(const std::string& name, json_t& to_validate, const json_t& schema)
    {
                   // There is no data here, so just copy the schema into the data.
            bool recursive_patches = false;
            if (to_validate.empty()) {
                to_validate = schema;
                return true;
            }

            for (auto it = to_validate.begin(); it != to_validate.end(); ++it) {
                if (json_t& value = it.value();
                    value.is_object() && schema.contains(it.key())) {
                    recursive_patches |= patch_json(name, value, schema.at(it.key()));
                }
            }

            std::vector<std::string> to_remove;
            std::vector<std::string> to_add;

            for (auto it = to_validate.begin(); it != to_validate.end(); ++it) {
                const std::string key = it.key();

                if (!schema.contains(key)) {
                    asa::get_logger()->warn("Removing '{}' from '{}' (not in schema).", key, name);
                    to_remove.push_back(key);
                    continue;
                }
                const auto schema_type = std::string(schema.at(key).type_name());
                const auto validate_type = to_validate.at(key).type_name();

                if (schema_type != validate_type) {
                    asa::get_logger()->warn("Removing '{}' from '{}' (type mismatch).", key, name);
                    to_remove.push_back(key);
                }
            }

            for (auto it = schema.begin(); it != schema.end(); ++it) {
                const std::string key = it.key();

                if (!to_validate.contains(it.key())) {
                    asa::get_logger()->warn("Adding '{}' to {} (not in data).", key, name);
                    to_add.push_back(it.key());
                    continue;
                }
                const auto schema_type = std::string(schema.at(key).type_name());
                const auto validate_type = to_validate.at(key).type_name();

                if (schema_type != validate_type) {
                    asa::get_logger()->warn("Adding '{}' to '{}' (types mismatch).", key, name);
                    to_add.push_back(it.key());
                }
            }

            for (const auto& key: to_remove) { to_validate.erase(key); }
            for (const auto& key: to_add) { to_validate[key] = schema.at(key); }

            return !to_remove.empty() || !to_add.empty() || recursive_patches;
    }

    void validate_config_integrity()
    {
        const auto path = get_config_path();
        asa::get_logger()->info("Performing config validation: {}..", path.string());

        if (!exists(path)) {
            asa::get_logger()->warn("Configuration file does not exist, creating..");

            // Only create up to the parent, the last component is the file, not a directory.
            create_directories(path.parent_path());
            utility::dump(json_t(), get_config_path());
        }

        json_t& data = get_config_data();
        bool patches = false;
        std::set<std::string> called;
        for (const auto& [k, v]: data.items()) {
            if (!validation_callbacks.contains(k)) {
                asa::get_logger()->warn("No validation is registered for '{}'!", k);
                continue;
            }
            // Call the validation function registered for this category and check if
            // it has made any changes so that we need to dump.
            patches |= validation_callbacks.at(k)(v);
            called.emplace(k);
        }
        asa::get_logger()->info("First validation complete. Check for missing entries..");

        // Its possible a category didnt exist in the file and needs to be created now.
        // In that case it hasnt been called in the previous validation
        for (const auto& [k, v]: validation_callbacks) {
            if (!called.contains(k)) {
                // This category was not validated because it didnt exist
                // The validation function will receive an empty json object reference to
                // populate with the default data.
                data[k] = json_t();
                patches |= v(data.at(k));
            }
        }
        asa::get_logger()->info("Configuration validated (patches: {}).", patches);

        // If any changes were made to the data during the validation process, dump them
        // so we dont have to perform them again on next startup (duh).
        if (patches) { utility::dump(data, get_config_path()); }
    }
}
