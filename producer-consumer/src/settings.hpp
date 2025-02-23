#include "messages.hpp"
#include "nlohmann/detail/macro_scope.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <optional>
#include <string>
#include <vector>

struct Params
{
    double mean;
    double stddev;
};

struct Station
{
    GasType type;
    Params handle;
};

struct Settings
{
    std::vector<Station> stations_params;
    Params create;
    int number_message_queue;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Params, mean, stddev);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Station, type, handle);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Settings, stations_params, create, number_message_queue);

inline std::optional<Settings> GetSettings(const std::string &path)
try
{
    std::ifstream conf{path};

    nlohmann::json json_conf;
    conf >> json_conf;

    Settings settings = json_conf;

    return settings;
}
catch (...)
{
    return {};
}
