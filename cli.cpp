#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <csignal>
#include <readline/readline.h>
#include <readline/history.h>

#include "axi_dsp.h"

static volatile bool running = true;
static std::vector<std::string> command_list;
static std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>> dispatch;

void signal_handler(int sig)
{
    if (sig == SIGINT) {
        std::cout << "\nExiting...\n";
        running = false;
        rl_done = 1; // Tell readline to stop
    }
}

static std::string trim(const std::string &s)
{
    size_t b = 0, e = s.size();
    while (b < e && std::isspace((unsigned char)s[b])) {
        ++b;
    }
    while (e > b && std::isspace((unsigned char)s[e - 1])) {
        --e;
    }
    return s.substr(b, e - b);
}

static std::vector<std::string> split_args(const std::string &s)
{
    std::vector<std::string> out;
    std::string cur;
    int depth = 0;

    for (char c : s) {
        if (c == ',' && depth == 0) {
            out.push_back(trim(cur));
            cur.clear();
        } else {
            if (c == '(') {
                ++depth;
            } else if (c == ')') {
                --depth;
            }
            cur += c;
        }
    }
    if (!cur.empty()) {
        out.push_back(trim(cur));
    }
    return out;
}

static bool parse_call(const std::string &input, std::string &name, std::vector<std::string> &args)
{
    auto lp = input.find('(');
    auto rp = input.rfind(')');
    if (lp == std::string::npos || rp == std::string::npos || rp < lp) {
        return false;
    }

    name = trim(input.substr(0, lp));
    std::string inside = input.substr(lp + 1, rp - lp - 1);
    inside = trim(inside);

    if (inside.empty()) {
        args.clear();
        return true;
    }

    args = split_args(inside);
    return true;
}

static uint32_t to_u32(const std::string &s)
{
    return static_cast<uint32_t>(std::stoul(s));
}

static float to_f32(const std::string &s)
{
    return std::stof(s);
}

// Autocompletion function for readline
// Fix the warning in command_generator (line 96)
char *command_generator(const char *text, int state)
{
    static int idx, len;
    static std::vector<std::string> matches;

    if (state == 0) {
        matches.clear();
        len = strlen(text);
        for (const auto &cmd : command_list) {
            if (strncmp(cmd.c_str(), text, len) == 0) {
                matches.push_back(cmd);
            }
        }
        idx = 0;
    }

    if (idx < (int)matches.size()) { // Cast to int to fix warning
        return strdup(matches[idx++].c_str());
    }

    return nullptr;
}

char **dsp_completion(const char *text, int start, int end)
{
    char **matches = nullptr;
    (void)end; // Suppress unused parameter warning

    // Only complete at the beginning of the line (not in arguments)
    if (start == 0) {
        matches = rl_completion_matches(text, command_generator);

        // Add "(" after completion if there's exactly one match
        if (matches && matches[0] && !matches[1]) {
            char *completed = matches[0];
            size_t len = strlen(completed);
            char *with_paren = (char *)malloc(len + 2);
            strcpy(with_paren, completed);
            with_paren[len] = '(';
            with_paren[len + 1] = '\0';
            free(matches[0]);
            matches[0] = with_paren;
        }
    }

    return matches;
}


// Initialize all commands
void init_commands()
{
    // Get commands (all with empty lambdas for now - will be replaced)
    command_list = {
        "axi_dsp_get_ip_ver", "axi_dsp_get_kill", "axi_dsp_get_test_point",
        "axi_dsp_get_channel", "axi_dsp_get_compensation_mode", "axi_dsp_get_manual_compensation",
        "axi_dsp_get_diagram_0", "axi_dsp_get_diagram_1", "axi_dsp_get_diagram_2",
        "axi_dsp_get_diagram_3", "axi_dsp_get_diagram_4", "axi_dsp_get_diagram_5",
        "axi_dsp_get_diagram_6", "axi_dsp_get_diagram_7", "axi_dsp_get_motion_selector",
        "axi_dsp_get_diagram_angle", "axi_dsp_get_output_source", "axi_dsp_get_apu_rank",
        "axi_dsp_get_detector_level", "axi_dsp_get_azimuth_angle", "axi_dsp_get_apply",
        "axi_dsp_set_test_point", "axi_dsp_set_channel", "axi_dsp_set_compensation_mode",
        "axi_dsp_set_manual_compensation", "axi_dsp_set_diagram_0", "axi_dsp_set_diagram_1",
        "axi_dsp_set_diagram_2", "axi_dsp_set_diagram_3", "axi_dsp_set_diagram_4",
        "axi_dsp_set_diagram_5", "axi_dsp_set_diagram_6", "axi_dsp_set_diagram_7",
        "axi_dsp_set_motion_selector", "axi_dsp_set_diagram_angle", "axi_dsp_set_output_source",
        "axi_dsp_set_apu_rank", "axi_dsp_set_detector_level", "axi_dsp_set_azimuth_angle",
        "axi_dsp_kill", "axi_dsp_apply", "help", "exit", "quit", "clear"};

    // Register all commands with their implementations
    dispatch["axi_dsp_get_ip_ver"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_ip_ver();
        std::cout << "Version: " << v.MAJ_VER << "." << v.MIN_VER << "\n";
    };

    dispatch["axi_dsp_get_kill"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_kill();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_test_point"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_test_point();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_channel"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_channel();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_compensation_mode"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_compensation_mode();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_manual_compensation"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_manual_compensation(channel)");
        }
        cmplx_f64 v = axi_dsp_get_manual_compensation(to_u32(args[0]));
        std::cout << "real=" << v.real << " imag=" << v.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_0"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_diagram_0(channel)");
        }
        cmplx_f64 r = axi_dsp_get_diagram_0(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_1"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_diagram_1(channel)");
        }
        cmplx_f64 r = axi_dsp_get_diagram_1(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_2"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_diagram_2(channel)");
        }
        cmplx_f64 r = axi_dsp_get_diagram_2(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_3"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_diagram_3(channel)");
        }
        cmplx_f64 r = axi_dsp_get_diagram_3(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_4"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_diagram_4(channel)");
        }
        cmplx_f64 r = axi_dsp_get_diagram_4(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_5"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_diagram_5(channel)");
        }
        cmplx_f64 r = axi_dsp_get_diagram_5(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_6"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_diagram_6(channel)");
        }
        cmplx_f64 r = axi_dsp_get_diagram_6(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_7"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_diagram_7(channel)");
        }
        cmplx_f64 r = axi_dsp_get_diagram_7(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_motion_selector"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_motion_selector();
        std::cout << "ONOFF: " << v.ONOFF << ", FILTER: " << v.FILTER << "\n";
    };

    dispatch["axi_dsp_get_diagram_angle"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_diagram_angle(channel)");
        }
        auto v = axi_dsp_get_diagram_angle(to_u32(args[0]));
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_output_source"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_output_source();
        std::cout << "SOURCE: " << v.SOURCE << ", SOURCE_CHANNEL: " << v.SOURCE_CHANNEL << "\n";
    };

    dispatch["axi_dsp_get_apu_rank"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_apu_rank();
        std::cout << "RANK: " << v.RANK << ", WINDOW: " << v.WINDOW << "\n";
    };

    dispatch["axi_dsp_get_detector_level"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_get_detector_level(num[0 or 1])");
        }
        auto v = axi_dsp_get_detector_level(to_u32(args[0]));
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_azimuth_angle"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_azimuth_angle();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_apply"] = [](const std::vector<std::string> &) {
        auto v = axi_dsp_get_apply();
        std::cout << v << "\n";
    };

    // Set commands
    dispatch["axi_dsp_set_test_point"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_set_test_point(tp)");
        }
        axi_dsp_set_test_point(to_u32(args[0]));
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_set_channel"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_set_channel(channel)");
        }
        axi_dsp_set_channel(to_u32(args[0]));
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_set_compensation_mode"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_set_compensation_mode(mode)");
        }
        axi_dsp_set_compensation_mode(to_u32(args[0]));
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_set_manual_compensation"] = [](const std::vector<std::string> &args) {
        if (args.size() != 3) {
            throw std::runtime_error("axi_dsp_set_manual_compensation(real, imag, channel)");
        }
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_manual_compensation(c, to_u32(args[2]));
        std::cout << "OK\n";
    };

    for (int i = 0; i <= 7; i++) {
        std::string cmd_name = "axi_dsp_set_diagram_" + std::to_string(i);

        dispatch[cmd_name] = [i, cmd_name](const std::vector<std::string> &args) {
            if (args.size() != 3) {
                throw std::runtime_error(cmd_name + "(real, imag, channel)");
            }
            cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};

            // Call the appropriate function based on i
            switch (i) {
            case 0:
                axi_dsp_set_diagram_0(c, to_u32(args[2]));
                break;
            case 1:
                axi_dsp_set_diagram_1(c, to_u32(args[2]));
                break;
            case 2:
                axi_dsp_set_diagram_2(c, to_u32(args[2]));
                break;
            case 3:
                axi_dsp_set_diagram_3(c, to_u32(args[2]));
                break;
            case 4:
                axi_dsp_set_diagram_4(c, to_u32(args[2]));
                break;
            case 5:
                axi_dsp_set_diagram_5(c, to_u32(args[2]));
                break;
            case 6:
                axi_dsp_set_diagram_6(c, to_u32(args[2]));
                break;
            case 7:
                axi_dsp_set_diagram_7(c, to_u32(args[2]));
                break;
            }
            std::cout << "OK\n";
        };
    }

    dispatch["axi_dsp_set_motion_selector"] = [](const std::vector<std::string> &args) {
        if (args.size() != 2) {
            throw std::runtime_error("axi_dsp_set_motion_selector(filter, onoff)");
        }
        axi_dsp_set_motion_selector(to_u32(args[0]), to_u32(args[1]));
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_set_diagram_angle"] = [](const std::vector<std::string> &args) {
        if (args.size() != 2) {
            throw std::runtime_error("axi_dsp_set_diagram_angle(angle, channel)");
        }
        axi_dsp_set_diagram_angle(to_f32(args[0]), to_u32(args[1]));
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_set_output_source"] = [](const std::vector<std::string> &args) {
        if (args.size() != 2) {
            throw std::runtime_error("axi_dsp_set_output_source(src, src_channel)");
        }
        axi_dsp_set_output_source(to_u32(args[0]), to_u32(args[1]));
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_set_apu_rank"] = [](const std::vector<std::string> &args) {
        if (args.size() != 2) {
            throw std::runtime_error("axi_dsp_set_apu_rank(rank, window)");
        }
        axi_dsp_set_apu_rank(to_u32(args[0]), to_u32(args[1]));
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_set_detector_level"] = [](const std::vector<std::string> &args) {
        if (args.size() != 2) {
            throw std::runtime_error("axi_dsp_set_detector_level(level, num)");
        }
        axi_dsp_set_detector_level(to_u32(args[0]), to_u32(args[1]));
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_set_azimuth_angle"] = [](const std::vector<std::string> &args) {
        if (args.size() != 1) {
            throw std::runtime_error("axi_dsp_set_azimuth_angle(angle)");
        }
        axi_dsp_set_azimuth_angle(to_f32(args[0]));
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_kill"] = [](const std::vector<std::string> &) {
        axi_dsp_kill();
        std::cout << "OK\n";
    };

    dispatch["axi_dsp_apply"] = [](const std::vector<std::string> &) {
        axi_dsp_apply();
        std::cout << "OK\n";
    };

    // Utility commands
    dispatch["help"] = [](const std::vector<std::string> &) {
        std::cout << "Available commands:\n";
        for (const auto &cmd : command_list) {
            if (cmd != "help" && cmd != "exit" && cmd != "quit" && cmd != "clear") {
                std::cout << "  " << cmd << "()\n";
            }
        }
        std::cout << "\nUtility commands:\n";
        std::cout << "  help - show this help\n";
        std::cout << "  clear - clear screen\n";
        std::cout << "  exit, quit - exit program\n";
        std::cout << "\nTips:\n";
        std::cout << "  - Press TAB to autocomplete commands\n";
        std::cout << "  - Use Up/Down arrows for command history\n";
        std::cout << "  - Press Ctrl+C to exit\n";
    };

    dispatch["exit"] = [](const std::vector<std::string> &) {
        running = false;
    };

    dispatch["quit"] = [](const std::vector<std::string> &) {
        running = false;
    };

    dispatch["clear"] = [](const std::vector<std::string> &) {
        std::cout << "\033[2J\033[1;1H"; // Clear screen
    };
}

int main()
{
    // Setup signal handler
    std::signal(SIGINT, signal_handler);

    // Initialize readline
    rl_readline_name = "dsp";
    rl_attempted_completion_function = dsp_completion;

    // Initialize commands
    init_commands();

    // Initialize DSP
    uint32_t init_ok = axi_dsp_init();
    if (init_ok != FD_ERR_NONE) {
        std::cerr << "Failed to initialize DSP\n";
        return 1;
    }

    std::cout << "\n\x1b[36mDSP CLI Started\x1b[0m\n";
    std::cout << "Type 'help' for commands, TAB for autocomplete, Ctrl+C to exit\n\n";

    // Load command history
    using_history();
    read_history(".dsp_history");

    // Main command loop
    while (running) {
        char *input_cstr = readline("\x1b[32mdsp>\x1b[0m ");

        if (!input_cstr) {
            // Ctrl+D pressed
            std::cout << "\n";
            break;
        }

        std::string input = trim(input_cstr);
        free(input_cstr);

        if (input.empty()) {
            continue;
        }

        // Add to history
        add_history(input.c_str());

        // Parse the command
        std::string fname;
        std::vector<std::string> args;

        // Auto-add parentheses if missing and not a utility command
        if (input.find('(') == std::string::npos && input != "help" && input != "exit" && input != "quit" && input != "clear") {
            input = input + "()";
        }

        if (!parse_call(input, fname, args)) {
            // Check if it's a utility command without parentheses
            if (dispatch.find(input) != dispatch.end()) {
                fname = input;
                args.clear();
            } else {
                std::cerr << "Bad syntax. Expected function_name(arg1, arg2)\n";
                continue;
            }
        }

        // Execute the command
        auto it = dispatch.find(fname);
        if (it != dispatch.end()) {
            try {
                it->second(args);
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
        } else {
            std::cerr << "Unknown command: " << fname << "\n";
        }
    }

    // Save history
    write_history(".dsp_history");

    // Cleanup
    axi_dsp_deinit();
    std::cout << "\nGoodbye!\n";

    return 0;
}