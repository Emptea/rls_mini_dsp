#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <cctype>
#include <cstdlib>

#include "axi_dsp.h"

static std::string trim(const std::string& s) {
    size_t b = 0, e = s.size();
    while (b < e && std::isspace((unsigned char)s[b])) ++b;
    while (e > b && std::isspace((unsigned char)s[e - 1])) --e;
    return s.substr(b, e - b);
}

static std::vector<std::string> split_args(const std::string& s) {
    std::vector<std::string> out;
    std::string cur;
    int depth = 0;

    for (char c : s) {
        if (c == ',' && depth == 0) {
            out.push_back(trim(cur));
            cur.clear();
        } else {
            if (c == '(') ++depth;
            else if (c == ')') --depth;
            cur += c;
        }
    }
    if (!cur.empty()) out.push_back(trim(cur));
    return out;
}

static bool parse_call(const std::string& input, std::string& name, std::vector<std::string>& args) {
    auto lp = input.find('(');
    auto rp = input.rfind(')');
    if (lp == std::string::npos || rp == std::string::npos || rp < lp) return false;

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

static uint32_t to_u32(const std::string& s) {
    return static_cast<uint32_t>(std::stoul(s));
}

static float to_f32(const std::string& s) {
    return std::stof(s);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage:\n";
        std::cerr << "  app \"axi_dsp_set_azimuth_angle(18.0)\"\n";
        std::cerr << "  app \"axi_dsp_set_output_source(1,0)\"\n";
        return 1;
    }

    std::string call = argv[1];

    uint32_t init_ok = axi_dsp_init();
    if (!init_ok) {
        std::cerr << "axi_dsp_init failed\n";
        return 1;
    }

    std::string fname;
    std::vector<std::string> args;
    if (!parse_call(call, fname, args)) {
        std::cerr << "Bad syntax. Expected function_name(arg1, arg2)\n";
        axi_dsp_deinit();
        return 1;
    }

    std::unordered_map<std::string, std::function<void()>> dispatch;

    dispatch["axi_dsp_get_ip_ver"] = []() {
        auto v = axi_dsp_get_ip_ver();
        std::cout << "Version: " << v.MAJ_VER << "." << v.MIN_VER << "\n";
    };

    dispatch["axi_dsp_get_kill"] = []() {
        auto v = axi_dsp_get_kill();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_test_point"] = []() {
        auto v = axi_dsp_get_test_point();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_channel"] = []() {
        auto v = axi_dsp_get_channel();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_compensation_mode"] = []() {
        auto v = axi_dsp_get_compensation_mode();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_manual_compensation"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_manual_compensation(channel)");
        cmplx_f64 v = axi_dsp_get_manual_compensation(to_u32(args[0]));
        std::cout << "real=" << v.real << " imag=" << v.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_0"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_diagram_0(channel)");
        cmplx_f64 r = axi_dsp_get_diagram_0(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_1"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_diagram_1(channel)");
        cmplx_f64 r = axi_dsp_get_diagram_1(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };


    dispatch["axi_dsp_get_diagram_2"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_diagram_2(channel)");
        cmplx_f64 r = axi_dsp_get_diagram_2(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };


    dispatch["axi_dsp_get_diagram_3"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_diagram_3(channel)");
        cmplx_f64 r = axi_dsp_get_diagram_3(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };


    dispatch["axi_dsp_get_diagram_4"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_diagram_4(channel)");
        cmplx_f64 r = axi_dsp_get_diagram_4(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };


    dispatch["axi_dsp_get_diagram_5"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_diagram_5(channel)");
        cmplx_f64 r = axi_dsp_get_diagram_5(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };


    dispatch["axi_dsp_get_diagram_6"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_diagram_6(channel)");
        cmplx_f64 r = axi_dsp_get_diagram_6(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_diagram_7"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_diagram_7(channel)");
        cmplx_f64 r = axi_dsp_get_diagram_7(to_u32(args[0]));
        std::cout << "real=" << r.real << " imag=" << r.imag << "\n";
    };

    dispatch["axi_dsp_get_motion_selector"] = []() {
        auto v = axi_dsp_get_motion_selector();
        std:: cout << "ONOFF: " << v.ONOFF << ", FILTER: " << v.FILTER << "\n";
    };

    dispatch["axi_dsp_get_diagram_angle"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_get_diagram_angle(channel)");
        auto v = axi_dsp_get_diagram_angle(to_u32(args[0]));
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_output_source"] = []() {
        auto v = axi_dsp_get_output_source();
        std:: cout << "SOURCE: " << v.SOURCE << ", SOURCE_CHANNEL: " << v.SOURCE_CHANNEL << "\n";
    };

    dispatch["axi_dsp_get_apu_rank"] = []() {
        auto v = axi_dsp_get_apu_rank();
        std:: cout << "RANK: " << v.RANK << ", WINDOW: " << v.WINDOW << "\n";
    };

    dispatch["axi_dsp_get_detector_level"] = []() {
        auto v = axi_dsp_get_detector_level();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_azimuth_angle"] = []() {
        auto v = axi_dsp_get_azimuth_angle();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_get_apply"] = []() {
        auto v = axi_dsp_get_apply();
        std::cout << v << "\n";
    };

    dispatch["axi_dsp_set_test_point"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_set_test_point(tp)");
        axi_dsp_set_test_point(to_u32(args[0]));
    };

    dispatch["axi_dsp_set_channel"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_set_channel(channel)");
        axi_dsp_set_channel(to_u32(args[0]));
    };

    dispatch["axi_dsp_set_compensation_mode"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_set_compensation_mode(mode)");
        axi_dsp_set_compensation_mode(to_u32(args[0]));
    };

    dispatch["axi_dsp_set_manual_compensation"] = [&]() {
        if (args.size() != 3) throw std::runtime_error("axi_dsp_set_manual_compensation(real, imag, channel)");
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_manual_compensation(c, to_u32(args[2]));
    };

    dispatch["axi_dsp_set_diagram_0"] = [&]() {
        if (args.size() != 3) throw std::runtime_error("axi_dsp_set_diagram_0(real, imag, channel)");
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_diagram_0(c, to_u32(args[2]));
    };

    dispatch["axi_dsp_set_diagram_1"] = [&]() {
        if (args.size() != 3) throw std::runtime_error("axi_dsp_set_diagram_1(real, imag, channel)");
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_diagram_1(c, to_u32(args[2]));
    };

    dispatch["axi_dsp_set_diagram_2"] = [&]() {
        if (args.size() != 3) throw std::runtime_error("axi_dsp_set_diagram_2(real, imag, channel)");
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_diagram_2(c, to_u32(args[2]));
    };

    dispatch["axi_dsp_set_diagram_3"] = [&]() {
        if (args.size() != 3) throw std::runtime_error("axi_dsp_set_diagram_3(real, imag, channel)");
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_diagram_3(c, to_u32(args[2]));
    };

    dispatch["axi_dsp_set_diagram_4"] = [&]() {
        if (args.size() != 3) throw std::runtime_error("axi_dsp_set_diagram_4(real, imag, channel)");
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_diagram_4(c, to_u32(args[2]));
    };

    dispatch["axi_dsp_set_diagram_5"] = [&]() {
        if (args.size() != 3) throw std::runtime_error("axi_dsp_set_diagram_5(real, imag, channel)");
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_diagram_5(c, to_u32(args[2]));
    };

    dispatch["axi_dsp_set_diagram_6"] = [&]() {
        if (args.size() != 3) throw std::runtime_error("axi_dsp_set_diagram_6(real, imag, channel)");
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_diagram_6(c, to_u32(args[2]));
    };

    dispatch["axi_dsp_set_diagram_7"] = [&]() {
        if (args.size() != 3) throw std::runtime_error("axi_dsp_set_diagram_7(real, imag, channel)");
        cmplx_f64 c{to_f32(args[0]), to_f32(args[1])};
        axi_dsp_set_diagram_7(c, to_u32(args[2]));
    };

    dispatch["axi_dsp_set_motion_selector"] = [&]() {
        if (args.size() != 2) throw std::runtime_error("axi_dsp_set_motion_selector(filter, onoff)");
        axi_dsp_set_output_source(to_u32(args[0]), to_u32(args[1]));
    };

    dispatch["axi_dsp_set_diagram_angle"] = [&]() {
        if (args.size() != 2) throw std::runtime_error("axi_dsp_set_diagram_angle(angle, channel)");
        axi_dsp_set_diagram_angle(to_f32(args[0]), to_u32(args[1]));
    };

    dispatch["axi_dsp_set_output_source"] = [&]() {
        if (args.size() != 2) throw std::runtime_error("axi_dsp_set_output_source(src, src_channel)");
        axi_dsp_set_output_source(to_u32(args[0]), to_u32(args[1]));
    };

    dispatch["axi_dsp_set_apu_rank"] = [&]() {
        if (args.size() != 2) throw std::runtime_error("axi_dsp_set_apu_rank(rank, window)");
        axi_dsp_set_apu_rank(to_u32(args[0]), to_u32(args[1]));
    };

    dispatch["axi_dsp_set_detector_level"] = [&]() {
        if (args.size() != 2) throw std::runtime_error("axi_dsp_set_detector_level(level, num)");
        axi_dsp_set_detector_level(to_u32(args[0]), to_u32(args[1]));
    };

    dispatch["axi_dsp_set_azimuth_angle"] = [&]() {
        if (args.size() != 1) throw std::runtime_error("axi_dsp_set_azimuth_angle(angle)");
        axi_dsp_set_azimuth_angle(to_f32(args[0]));
    };

    dispatch["axi_dsp_kill"] = []() {
        axi_dsp_kill();
    };

    dispatch["axi_dsp_apply"] = []() {
        axi_dsp_apply();
    };


    try {
        auto it = dispatch.find(fname);
        if (it == dispatch.end()) {
            std::cerr << "Unknown function: " << fname << "\n";
            axi_dsp_deinit();
            return 1;
        }

        it->second();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        axi_dsp_deinit();
        return 1;
    }

    axi_dsp_deinit();
    return 0;
}