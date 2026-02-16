into the #define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cwctype>
#include <iostream>
#include <iomanip>

constexpr double kPi = 3.14159265358979323846;
constexpr double kE = 2.71828182845904523536;

enum class AngleMode { Radians, Degrees };

struct FunctionSpec {
    int arity;
    std::function<double(const std::vector<double>&, AngleMode)> apply;
};

struct OperatorInfo {
    int precedence;
    bool rightAssociative;
    int arity;
};

class ExpressionEngine {
public:
    ExpressionEngine() {
        ops_[L"+"] = {2, false, 2};
        ops_[L"-"] = {2, false, 2};
        ops_[L"*"] = {3, false, 2};
        ops_[L"/"] = {3, false, 2};
        ops_[L"%"] = {3, false, 2};
        ops_[L"^"] = {4, true, 2};
        ops_[L"u+"] = {5, true, 1};
        ops_[L"u-"] = {5, true, 1};
        ops_[L"!"] = {6, false, 1};

        funcs_[L"sin"] = {1, [](const std::vector<double>& a, AngleMode m) { return std::sin(toRad(a[0], m)); }};
        funcs_[L"cos"] = {1, [](const std::vector<double>& a, AngleMode m) { return std::cos(toRad(a[0], m)); }};
        funcs_[L"tan"] = {1, [](const std::vector<double>& a, AngleMode m) { return std::tan(toRad(a[0], m)); }};
        funcs_[L"asin"] = {1, [](const std::vector<double>& a, AngleMode m) {
                             if (a[0] < -1.0 || a[0] > 1.0) throw std::runtime_error("asin domain [-1,1]");
                             double r = std::asin(a[0]);
                             return m == AngleMode::Degrees ? (r * 180.0 / kPi) : r;
                         }};
        funcs_[L"acos"] = {1, [](const std::vector<double>& a, AngleMode m) {
                             if (a[0] < -1.0 || a[0] > 1.0) throw std::runtime_error("acos domain [-1,1]");
                             double r = std::acos(a[0]);
                             return m == AngleMode::Degrees ? (r * 180.0 / kPi) : r;
                         }};
        funcs_[L"atan"] = {1, [](const std::vector<double>& a, AngleMode m) {
                             double r = std::atan(a[0]);
                             return m == AngleMode::Degrees ? (r * 180.0 / kPi) : r;
                         }};
        funcs_[L"sqrt"] = {1, [](const std::vector<double>& a, AngleMode) {
                             if (a[0] < 0.0) throw std::runtime_error("sqrt domain x>=0");
                             return std::sqrt(a[0]);
                         }};
        funcs_[L"ln"] = {1, [](const std::vector<double>& a, AngleMode) {
                           if (a[0] <= 0.0) throw std::runtime_error("ln domain x>0");
                           return std::log(a[0]);
                       }};
        funcs_[L"log"] = {1, [](const std::vector<double>& a, AngleMode) {
                            if (a[0] <= 0.0) throw std::runtime_error("log domain x>0");
                            return std::log10(a[0]);
                        }};
        funcs_[L"abs"] = {1, [](const std::vector<double>& a, AngleMode) { return std::fabs(a[0]); }};
        funcs_[L"pow"] = {2, [](const std::vector<double>& a, AngleMode) { return std::pow(a[0], a[1]); }};
        funcs_[L"min"] = {2, [](const std::vector<double>& a, AngleMode) { return std::min(a[0], a[1]); }};
        funcs_[L"max"] = {2, [](const std::vector<double>& a, AngleMode) { return std::max(a[0], a[1]); }};

        // Electrical Engineering Functions - Ohm's Law & Power
        funcs_[L"pvi"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] * a[1]; }};
        funcs_[L"pir"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] * a[0] * a[1]; }};
        funcs_[L"pvr"] = {2, [](const std::vector<double>& a, AngleMode) { return (a[0] * a[0]) / a[1]; }};
        funcs_[L"vir"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] * a[1]; }};
        funcs_[L"ivr"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] / a[1]; }};
        funcs_[L"rvi"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] / a[1]; }};

        // Additional derived calculations
        funcs_[L"vpi"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] / a[1]; }};
        funcs_[L"ipv"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] / a[1]; }};
        funcs_[L"rpi"] = {2, [](const std::vector<double>& a, AngleMode) { return (a[0] / (a[1] * a[1])); }};
        funcs_[L"rpv"] = {2, [](const std::vector<double>& a, AngleMode) { return (a[1] * a[1]) / a[0]; }};
        funcs_[L"vpr"] = {2, [](const std::vector<double>& a, AngleMode) { return std::sqrt(a[0] * a[1]); }};
        funcs_[L"ipr"] = {2, [](const std::vector<double>& a, AngleMode) { return std::sqrt(a[0] / a[1]); }};

        // AC Power Functions
        funcs_[L"preal"] = {3, [](const std::vector<double>& a, AngleMode m) {
                             double angle = toRad(a[2], m);
                             return a[0] * a[1] * std::cos(angle);
                         }};
        funcs_[L"preact"] = {3, [](const std::vector<double>& a, AngleMode m) {
                              double angle = toRad(a[2], m);
                              return a[0] * a[1] * std::sin(angle);
                          }};
        funcs_[L"papp"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] * a[1]; }};
        funcs_[L"pf"] = {1, [](const std::vector<double>& a, AngleMode m) {
                          double angle = toRad(a[0], m);
                          return std::cos(angle);
                      }};

        // Impedance & Reactance
        funcs_[L"zrx"] = {2, [](const std::vector<double>& a, AngleMode) {
                           return std::sqrt(a[0] * a[0] + a[1] * a[1]);
                       }};
        funcs_[L"xc"] = {2, [](const std::vector<double>& a, AngleMode) {
                          if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("xc args must be > 0");
                          return 1.0 / (2.0 * kPi * a[0] * a[1]);
                      }};
        funcs_[L"xl"] = {2, [](const std::vector<double>& a, AngleMode) {
                          if (a[0] < 0 || a[1] < 0) throw std::runtime_error("xl args must be >= 0");
                          return 2.0 * kPi * a[0] * a[1];
                      }};

        // Resonant Frequency
        funcs_[L"fres"] = {2, [](const std::vector<double>& a, AngleMode) {
                            if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("fres args must be > 0");
                            return 1.0 / (2.0 * kPi * std::sqrt(a[0] * a[1]));
                        }};

        // Decibel Calculations
        funcs_[L"dbv"] = {2, [](const std::vector<double>& a, AngleMode) {
                           if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("dbv args must be > 0");
                           return 20.0 * std::log10(a[0] / a[1]);
                       }};
        funcs_[L"dbp"] = {2, [](const std::vector<double>& a, AngleMode) {
                           if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("dbp args must be > 0");
                           return 10.0 * std::log10(a[0] / a[1]);
                       }};

        // Voltage Divider
        funcs_[L"vdiv"] = {3, [](const std::vector<double>& a, AngleMode) {
                            if (a[1] + a[2] == 0) throw std::runtime_error("vdiv R1+R2 cannot be 0");
                            return a[0] * a[2] / (a[1] + a[2]);
                        }};

        // Calculus Functions
        funcs_[L"sum"] = {1, [](const std::vector<double>& a, AngleMode) {
                           if (a[0] < 0 || !isNearlyInt(a[0])) throw std::runtime_error("sum needs integer >= 0");
                           double n = std::round(a[0]);
                           return n * (n + 1) / 2.0;
                       }};
        funcs_[L"sum2"] = {1, [](const std::vector<double>& a, AngleMode) {
                            if (a[0] < 0 || !isNearlyInt(a[0])) throw std::runtime_error("sum2 needs integer >= 0");
                            double n = std::round(a[0]);
                            return n * (n + 1) * (2 * n + 1) / 6.0;
                        }};
        funcs_[L"sum3"] = {1, [](const std::vector<double>& a, AngleMode) {
                            if (a[0] < 0 || !isNearlyInt(a[0])) throw std::runtime_error("sum3 needs integer >= 0");
                            double n = std::round(a[0]);
                            double t = n * (n + 1) / 2.0;
                            return t * t;
                        }};
        funcs_[L"geom"] = {3, [](const std::vector<double>& a, AngleMode) {
                            double a0 = a[0], r = a[1], n = a[2];
                            if (std::fabs(r - 1.0) < 1e-12) return a0 * (n + 1);
                            return a0 * (1.0 - std::pow(r, n + 1)) / (1.0 - r);
                        }};
        funcs_[L"intpow"] = {3, [](const std::vector<double>& a, AngleMode) {
                             double lo = a[0], hi = a[1], k = a[2];
                             if (std::fabs(k + 1) < 1e-12) {
                                 if (lo <= 0 || hi <= 0) throw std::runtime_error("intpow: x must be > 0 for k=-1");
                                 return std::log(hi) - std::log(lo);
                             }
                             return (std::pow(hi, k + 1) - std::pow(lo, k + 1)) / (k + 1);
                         }};
        funcs_[L"intexp"] = {2, [](const std::vector<double>& a, AngleMode) {
                             return std::exp(a[1]) - std::exp(a[0]);
                         }};
        funcs_[L"intsin"] = {2, [](const std::vector<double>& a, AngleMode) {
                             return -std::cos(a[1]) + std::cos(a[0]);
                         }};
        funcs_[L"intcos"] = {2, [](const std::vector<double>& a, AngleMode) {
                             return std::sin(a[1]) - std::sin(a[0]);
                         }};
        funcs_[L"intlog"] = {2, [](const std::vector<double>& a, AngleMode) {
                             if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("intlog: bounds must be > 0");
                             return std::log(a[1]) - std::log(a[0]);
                         }};
        funcs_[L"derivpow"] = {3, [](const std::vector<double>& a, AngleMode) {
                               double x = a[0], n = a[1], h = a[2];
                               if (h <= 0) h = 1e-6;
                               double fxh = std::pow(x + h, n);
                               double fxmh = std::pow(x - h, n);
                               return (fxh - fxmh) / (2 * h);
                           }};
        funcs_[L"derivexp"] = {2, [](const std::vector<double>& a, AngleMode) {
                               double x = a[0], h = a[1];
                               if (h <= 0) h = 1e-6;
                               return (std::exp(x + h) - std::exp(x - h)) / (2 * h);
                           }};
        funcs_[L"derivsin"] = {2, [](const std::vector<double>& a, AngleMode) {
                               double x = a[0], h = a[1];
                               if (h <= 0) h = 1e-6;
                               return (std::sin(x + h) - std::sin(x - h)) / (2 * h);
                           }};
        funcs_[L"derivcos"] = {2, [](const std::vector<double>& a, AngleMode) {
                               double x = a[0], h = a[1];
                               if (h <= 0) h = 1e-6;
                               return (std::cos(x + h) - std::cos(x - h)) / (2 * h);
                           }};
        funcs_[L"derivln"] = {2, [](const std::vector<double>& a, AngleMode) {
                              double x = a[0], h = a[1];
                              if (h <= 0) h = 1e-6;
                              if (x - h <= 0) throw std::runtime_error("derivln: x-h must be > 0");
                              return (std::log(x + h) - std::log(x - h)) / (2 * h);
                          }};
        funcs_[L"limpow"] = {3, [](const std::vector<double>& a, AngleMode) {
                             double x0 = a[0], n = a[1], dir = a[2];
                             double eps = 1e-10;
                             double x = x0 + (dir >= 0 ? eps : -eps);
                             return std::pow(x, n);
                         }};
    }

    double evaluate(const std::wstring& expr, AngleMode mode, double ans, double mem) const {
        std::map<std::wstring, double> vars{{L"pi", kPi}, {L"e", kE}, {L"ans", ans}, {L"mem", mem}};
        auto tokens = tokenize(expr);
        tokens = insertImplicitMult(tokens);
        auto rpn = toRpn(tokens);
        return evalRpn(rpn, mode, vars);
    }

private:
    enum class TT { Number, Name, Operator, LParen, RParen, Comma };
    struct Tok {
        TT type;
        std::wstring text;
        double n = 0.0;
    };

    std::map<std::wstring, OperatorInfo> ops_;
    std::map<std::wstring, FunctionSpec> funcs_;

    static double toRad(double x, AngleMode m) {
        return m == AngleMode::Degrees ? (x * kPi / 180.0) : x;
    }

    static bool isNearlyInt(double x) {
        return std::fabs(x - std::round(x)) < 1e-12;
    }

    static double factorial(double x) {
        if (x < 0 || !isNearlyInt(x)) throw std::runtime_error("factorial needs integer >= 0");
        long long n = static_cast<long long>(std::llround(x));
        if (n > 170) throw std::runtime_error("factorial too large (>170)");
        double r = 1.0;
        for (long long i = 2; i <= n; ++i) r *= i;
        return r;
    }

    static std::wstring lower(std::wstring s) {
        std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c) {
            return static_cast<wchar_t>(std::towlower(c));
        });
        return s;
    }

    std::vector<Tok> tokenize(const std::wstring& e) const {
        std::vector<Tok> t;
        for (size_t i = 0; i < e.size();) {
            wchar_t c = e[i];
            if (iswspace(c)) { ++i; continue; }
            if (iswdigit(c) || c == L'.') {
                size_t j = i;
                bool dot = false;
                while (j < e.size()) {
                    wchar_t d = e[j];
                    if (d == L'.') { if (dot) break; dot = true; ++j; }
                    else if (iswdigit(d)) ++j;
                    else break;
                }
                auto s = e.substr(i, j - i);
                t.push_back({TT::Number, s, std::stod(std::string(s.begin(), s.end()))});
                i = j; continue;
            }
            if (iswalpha(c) || c == L'_') {
                size_t j = i;
                while (j < e.size() && (iswalnum(e[j]) || e[j] == L'_')) ++j;
                t.push_back({TT::Name, lower(e.substr(i, j - i)), 0.0});
                i = j; continue;
            }
            if (c == L'+' || c == L'-' || c == L'*' || c == L'/' || c == L'^' || c == L'%' || c == L'!') {
                t.push_back({TT::Operator, std::wstring(1, c), 0.0}); ++i; continue;
            }
            if (c == L'(') { t.push_back({TT::LParen, L"(", 0.0}); ++i; continue; }
            if (c == L')') { t.push_back({TT::RParen, L")", 0.0}); ++i; continue; }
            if (c == L',') { t.push_back({TT::Comma, L",", 0.0}); ++i; continue; }
            throw std::runtime_error("invalid character");
        }
        return t;
    }

    std::vector<Tok> insertImplicitMult(const std::vector<Tok>& in) const {
        std::vector<Tok> out;
        for (size_t i = 0; i < in.size(); ++i) {
            out.push_back(in[i]);
            if (i + 1 < in.size()) {
                const Tok& cur = in[i];
                const Tok& nxt = in[i + 1];
                bool curVal = (cur.type == TT::Number || cur.type == TT::RParen ||
                              (cur.type == TT::Operator && cur.text == L"!") ||
                              (cur.type == TT::Name && funcs_.find(cur.text) == funcs_.end()));
                bool nxtVal = (nxt.type == TT::Number || nxt.type == TT::LParen || nxt.type == TT::Name);
                if (curVal && nxtVal) out.push_back({TT::Operator, L"*", 0.0});
            }
        }
        return out;
    }

    std::vector<Tok> toRpn(const std::vector<Tok>& in) const {
        std::vector<Tok> out, st;
        bool expectUnary = true;
        for (size_t i = 0; i < in.size(); ++i) {
            Tok tk = in[i];
            if (tk.type == TT::Number) { out.push_back(tk); expectUnary = false; continue; }
            if (tk.type == TT::Name) {
                bool isFunc = (i + 1 < in.size() && in[i + 1].type == TT::LParen);
                if (isFunc) st.push_back(tk); else out.push_back(tk);
                expectUnary = false; continue;
            }
            if (tk.type == TT::Comma) {
                while (!st.empty() && st.back().type != TT::LParen) { out.push_back(st.back()); st.pop_back(); }
                if (st.empty()) throw std::runtime_error("misplaced comma");
                expectUnary = true; continue;
            }
            if (tk.type == TT::Operator) {
                if ((tk.text == L"+" || tk.text == L"-") && expectUnary) tk.text = (tk.text == L"+") ? L"u+" : L"u-";
                auto cur = ops_.find(tk.text);
                if (cur == ops_.end()) throw std::runtime_error("unsupported operator");
                while (!st.empty() && st.back().type == TT::Operator) {
                    auto top = ops_.find(st.back().text);
                    if (top == ops_.end()) break;
                    bool pop = cur->second.rightAssociative ? (cur->second.precedence < top->second.precedence)
                                                            : (cur->second.precedence <= top->second.precedence);
                    if (!pop) break;
                    out.push_back(st.back()); st.pop_back();
                }
                st.push_back(tk); expectUnary = tk.text != L"!"; continue;
            }
            if (tk.type == TT::LParen) { st.push_back(tk); expectUnary = true; continue; }
            if (tk.type == TT::RParen) {
                bool found = false;
                while (!st.empty()) {
                    Tok top = st.back(); st.pop_back();
                    if (top.type == TT::LParen) { found = true; break; }
                    out.push_back(top);
                }
                if (!found) throw std::runtime_error("mismatched parentheses");
                if (!st.empty() && st.back().type == TT::Name) { out.push_back(st.back()); st.pop_back(); }
                expectUnary = false;
            }
        }
        while (!st.empty()) {
            if (st.back().type == TT::LParen) throw std::runtime_error("mismatched parentheses");
            out.push_back(st.back()); st.pop_back();
        }
        return out;
    }

    double evalRpn(const std::vector<Tok>& rpn, AngleMode mode, const std::map<std::wstring, double>& vars) const {
        std::vector<double> st;
        for (const auto& tk : rpn) {
            if (tk.type == TT::Number) st.push_back(tk.n);
            else if (tk.type == TT::Name) {
                auto f = funcs_.find(tk.text);
                if (f != funcs_.end()) {
                    int a = f->second.arity;
                    if (static_cast<int>(st.size()) < a) throw std::runtime_error("not enough function args");
                    std::vector<double> args(a);
                    for (int i = a - 1; i >= 0; --i) { args[i] = st.back(); st.pop_back(); }
                    st.push_back(f->second.apply(args, mode));
                } else {
                    auto v = vars.find(tk.text);
                    if (v == vars.end()) throw std::runtime_error("unknown identifier");
                    st.push_back(v->second);
                }
            } else if (tk.type == TT::Operator) {
                int a = ops_.at(tk.text).arity;
                if (static_cast<int>(st.size()) < a) throw std::runtime_error("not enough operands");
                if (a == 1) {
                    double x = st.back(); st.pop_back();
                    if (tk.text == L"u+") st.push_back(+x);
                    else if (tk.text == L"u-") st.push_back(-x);
                    else if (tk.text == L"!") st.push_back(factorial(x));
                } else {
                    double b = st.back(); st.pop_back();
                    double a1 = st.back(); st.pop_back();
                    if (tk.text == L"+") st.push_back(a1 + b);
                    else if (tk.text == L"-") st.push_back(a1 - b);
                    else if (tk.text == L"*") st.push_back(a1 * b);
                    else if (tk.text == L"/") { if (std::fabs(b) < 1e-15) throw std::runtime_error("division by zero"); st.push_back(a1 / b); }
                    else if (tk.text == L"%") { if (std::fabs(b) < 1e-15) throw std::runtime_error("modulo by zero"); st.push_back(std::fmod(a1, b)); }
                    else if (tk.text == L"^") st.push_back(std::pow(a1, b));
                }
            }
        }
        if (st.size() != 1) throw std::runtime_error("invalid expression");
        return st.back();
    }
};

int testsPassed = 0;
int testsFailed = 0;

void test(const char* name, const std::wstring& expr, double expected, AngleMode mode = AngleMode::Radians, double tolerance = 1e-6) {
    ExpressionEngine engine;
    try {
        double result = engine.evaluate(expr, mode, 0, 0);
        if (std::fabs(result - expected) < tolerance) {
            std::cout << "[PASS] " << name << ": " << std::string(expr.begin(), expr.end()) << " = " << result << "\n";
            testsPassed++;
        } else {
            std::cout << "[FAIL] " << name << ": " << std::string(expr.begin(), expr.end()) 
                      << " expected " << expected << " but got " << result << "\n";
            testsFailed++;
        }
    } catch (const std::exception& e) {
        std::cout << "[FAIL] " << name << ": " << std::string(expr.begin(), expr.end()) 
                  << " threw exception: " << e.what() << "\n";
        testsFailed++;
    }
}

int main() {
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "=== CALCULATOR FUNCTION TESTS ===\n\n";

    std::cout << "--- Basic Arithmetic ---\n";
    test("Addition", L"2+3", 5);
    test("Subtraction", L"10-4", 6);
    test("Multiplication", L"6*7", 42);
    test("Division", L"15/3", 5);
    test("Power", L"2^10", 1024);
    test("Modulo", L"17%5", 2);
    test("Factorial", L"5!", 120);
    test("Negative", L"-5+3", -2);
    test("Parentheses", L"(2+3)*4", 20);
    test("Order of ops", L"2+3*4", 14);

    std::cout << "\n--- Constants ---\n";
    test("Pi", L"pi", kPi);
    test("Euler's e", L"e", kE);

    std::cout << "\n--- Trig Functions (Radians) ---\n";
    test("sin(0)", L"sin(0)", 0);
    test("sin(pi/2)", L"sin(pi/2)", 1);
    test("cos(0)", L"cos(0)", 1);
    test("cos(pi)", L"cos(pi)", -1, AngleMode::Radians, 1e-5);
    test("tan(pi/4)", L"tan(pi/4)", 1, AngleMode::Radians, 1e-5);
    test("asin(1)", L"asin(1)", kPi/2);
    test("acos(0)", L"acos(0)", kPi/2);
    test("atan(1)", L"atan(1)", kPi/4);

    std::cout << "\n--- Trig Functions (Degrees) ---\n";
    test("sin(90 deg)", L"sin(90)", 1, AngleMode::Degrees);
    test("cos(180 deg)", L"cos(180)", -1, AngleMode::Degrees, 1e-5);
    test("tan(45 deg)", L"tan(45)", 1, AngleMode::Degrees, 1e-5);

    std::cout << "\n--- Math Functions ---\n";
    test("sqrt(16)", L"sqrt(16)", 4);
    test("sqrt(2)", L"sqrt(2)", std::sqrt(2.0));
    test("ln(e)", L"ln(e)", 1);
    test("ln(10)", L"ln(10)", std::log(10.0));
    test("log(100)", L"log(100)", 2);
    test("abs(-5)", L"abs(-5)", 5);
    test("pow(2,8)", L"pow(2,8)", 256);
    test("min(3,7)", L"min(3,7)", 3);
    test("max(3,7)", L"max(3,7)", 7);

    std::cout << "\n--- Ohm's Law Functions ---\n";
    test("P=VI (12V,2A)", L"pvi(12,2)", 24);
    test("P=I²R (2A,10Ω)", L"pir(2,10)", 40);
    test("P=V²/R (12V,6Ω)", L"pvr(12,6)", 24);
    test("V=IR (2A,10Ω)", L"vir(2,10)", 20);
    test("I=V/R (12V,4Ω)", L"ivr(12,4)", 3);
    test("R=V/I (12V,3A)", L"rvi(12,3)", 4);

    std::cout << "\n--- Derived Power Functions ---\n";
    test("V=P/I (24W,2A)", L"vpi(24,2)", 12);
    test("I=P/V (24W,12V)", L"ipv(24,12)", 2);
    test("V=√PR (100W,4Ω)", L"vpr(100,4)", 20);
    test("I=√P/R (100W,4Ω)", L"ipr(100,4)", 5);

    std::cout << "\n--- AC Power Functions ---\n";
    test("Z=√(R²+X²) (3,4)", L"zrx(3,4)", 5);
    test("PF(30 deg)", L"pf(30)", std::cos(30*kPi/180), AngleMode::Degrees, 1e-5);
    test("Preal(120V,5A,30deg)", L"preal(120,5,30)", 120*5*std::cos(30*kPi/180), AngleMode::Degrees, 0.1);
    test("Preact(120V,5A,30deg)", L"preact(120,5,30)", 120*5*std::sin(30*kPi/180), AngleMode::Degrees, 0.1);
    test("Papp(120V,5A)", L"papp(120,5)", 600);

    std::cout << "\n--- Reactance Functions ---\n";
    test("Xc(1kHz,1uF)", L"xc(1000,0.000001)", 1/(2*kPi*1000*0.000001), AngleMode::Radians, 0.1);
    test("Xl(1kHz,1mH)", L"xl(1000,0.001)", 2*kPi*1000*0.001, AngleMode::Radians, 0.1);
    test("fres(1mH,1uF)", L"fres(0.001,0.000001)", 1/(2*kPi*std::sqrt(0.001*0.000001)), AngleMode::Radians, 1);

    std::cout << "\n--- Decibel Functions ---\n";
    test("dB(V) 10/1", L"dbv(10,1)", 20);
    test("dB(P) 100/1", L"dbp(100,1)", 20);
    test("Vdiv(12V,1k,1k)", L"vdiv(12,1000,1000)", 6);

    std::cout << "\n--- Calculus: Summations ---\n";
    test("sum(10)", L"sum(10)", 55);
    test("sum(100)", L"sum(100)", 5050);
    test("sum2(3)", L"sum2(3)", 14);  // 1+4+9=14
    test("sum2(10)", L"sum2(10)", 385);
    test("sum3(3)", L"sum3(3)", 36);  // 1+8+27=36
    test("sum3(5)", L"sum3(5)", 225);
    test("geom(1,2,3)", L"geom(1,2,3)", 15);  // 1+2+4+8=15

    std::cout << "\n--- Calculus: Integrals ---\n";
    test("∫x³ from 0 to 2", L"intpow(0,2,3)", 4);  // x^4/4 from 0 to 2 = 16/4 = 4
    test("∫x² from 0 to 3", L"intpow(0,3,2)", 9);  // x^3/3 from 0 to 3 = 27/3 = 9
    test("∫e^x from 0 to 1", L"intexp(0,1)", kE-1, AngleMode::Radians, 0.001);
    test("∫sin from 0 to pi", L"intsin(0,pi)", 2, AngleMode::Radians, 0.001);
    test("∫cos from 0 to pi", L"intcos(0,pi)", 0, AngleMode::Radians, 0.001);
    test("∫1/x from 1 to e", L"intlog(1,e)", 1, AngleMode::Radians, 0.001);

    std::cout << "\n--- Calculus: Derivatives ---\n";
    test("d/dx x³ at x=2", L"derivpow(2,3,0.000001)", 12, AngleMode::Radians, 0.01);  // 3*2²=12
    test("d/dx x² at x=3", L"derivpow(3,2,0.000001)", 6, AngleMode::Radians, 0.01);   // 2*3=6
    test("d/dx e^x at x=0", L"derivexp(0,0.000001)", 1, AngleMode::Radians, 0.01);
    test("d/dx e^x at x=1", L"derivexp(1,0.000001)", kE, AngleMode::Radians, 0.01);
    test("d/dx sin at x=0", L"derivsin(0,0.000001)", 1, AngleMode::Radians, 0.01);    // cos(0)=1
    test("d/dx cos at x=0", L"derivcos(0,0.000001)", 0, AngleMode::Radians, 0.01);    // -sin(0)=0
    test("d/dx ln at x=2", L"derivln(2,0.000001)", 0.5, AngleMode::Radians, 0.01);    // 1/2=0.5

    std::cout << "\n--- Complex Expressions ---\n";
    test("2+3*4-5", L"2+3*4-5", 9);
    test("2^3^2", L"2^3^2", 512);  // Right associative: 2^(3^2) = 2^9 = 512
    test("sin(pi/6)*2", L"sin(pi/6)*2", 1, AngleMode::Radians, 1e-5);
    test("sqrt(3^2+4^2)", L"sqrt(3^2+4^2)", 5);
    test("ln(e^2)", L"ln(e^2)", 2);
    test("10^(log(5))", L"10^(log(5))", 5);

    std::cout << "\n=== TEST SUMMARY ===\n";
    std::cout << "Passed: " << testsPassed << "\n";
    std::cout << "Failed: " << testsFailed << "\n";
    std::cout << "Total:  " << (testsPassed + testsFailed) << "\n";
    
    if (testsFailed == 0) {
        std::cout << "\n*** ALL TESTS PASSED! ***\n";
        return 0;
    } else {
        std::cout << "\n*** SOME TESTS FAILED! ***\n";
        return 1;
    }
}