#define NOMINMAX
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


namespace {

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
        funcs_[L"pvi"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] * a[1]; }};  // P = V * I
        funcs_[L"pir"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] * a[0] * a[1]; }};  // P = I² * R
        funcs_[L"pvr"] = {2, [](const std::vector<double>& a, AngleMode) { return (a[0] * a[0]) / a[1]; }};  // P = V² / R
        funcs_[L"vir"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] * a[1]; }};  // V = I * R
        funcs_[L"ivr"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] / a[1]; }};  // I = V / R
        funcs_[L"rvi"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] / a[1]; }};  // R = V / I

        // Additional derived calculations
        funcs_[L"vpi"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] / a[1]; }};  // V = P / I
        funcs_[L"ipv"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] / a[1]; }};  // I = P / V
        funcs_[L"rpi"] = {2, [](const std::vector<double>& a, AngleMode) { return (a[0] * a[1] * a[1]); }};  // R = P / I²
        funcs_[L"rpv"] = {2, [](const std::vector<double>& a, AngleMode) { return (a[0] * a[1]) / (a[0] * a[0]); }};  // R = V² / P (fixed: V²/P)
        funcs_[L"vpr"] = {2, [](const std::vector<double>& a, AngleMode) { return std::sqrt(a[0] * a[1]); }};  // V = √(P * R)
        funcs_[L"ipr"] = {2, [](const std::vector<double>& a, AngleMode) { return std::sqrt(a[0] / a[1]); }};  // I = √(P / R)

        // AC Power Functions (3-arg: V, I, angle in current mode)
        funcs_[L"preal"] = {3, [](const std::vector<double>& a, AngleMode m) {
                             double angle = toRad(a[2], m);
                             return a[0] * a[1] * std::cos(angle);
                         }};
        funcs_[L"preact"] = {3, [](const std::vector<double>& a, AngleMode m) {
                              double angle = toRad(a[2], m);
                              return a[0] * a[1] * std::sin(angle);
                          }};
        funcs_[L"papp"] = {2, [](const std::vector<double>& a, AngleMode) { return a[0] * a[1]; }};  // Apparent power S = V * I
        funcs_[L"pf"] = {1, [](const std::vector<double>& a, AngleMode m) {
                          double angle = toRad(a[0], m);
                          return std::cos(angle);
                      }};

        // Impedance & Reactance
        funcs_[L"zrx"] = {2, [](const std::vector<double>& a, AngleMode) {
                           return std::sqrt(a[0] * a[0] + a[1] * a[1]);
                       }};  // Z = √(R² + X²)
        funcs_[L"xc"] = {2, [](const std::vector<double>& a, AngleMode) {
                          if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("xc args must be > 0");
                          return 1.0 / (2.0 * kPi * a[0] * a[1]);
                      }};  // Xc = 1/(2πfC)
        funcs_[L"xl"] = {2, [](const std::vector<double>& a, AngleMode) {
                          if (a[0] < 0 || a[1] < 0) throw std::runtime_error("xl args must be >= 0");
                          return 2.0 * kPi * a[0] * a[1];
                      }};  // Xl = 2πfL

        // Resonant Frequency
        funcs_[L"fres"] = {2, [](const std::vector<double>& a, AngleMode) {
                            if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("fres args must be > 0");
                            return 1.0 / (2.0 * kPi * std::sqrt(a[0] * a[1]));
                        }};  // f₀ = 1/(2π√(LC))

        // Decibel Calculations
        funcs_[L"dbv"] = {2, [](const std::vector<double>& a, AngleMode) {
                           if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("dbv args must be > 0");
                           return 20.0 * std::log10(a[0] / a[1]);
                       }};  // dB = 20*log10(V1/V2)
        funcs_[L"dbp"] = {2, [](const std::vector<double>& a, AngleMode) {
                           if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("dbp args must be > 0");
                           return 10.0 * std::log10(a[0] / a[1]);
                       }};  // dB = 10*log10(P1/P2)

        // Voltage Divider
        funcs_[L"vdiv"] = {3, [](const std::vector<double>& a, AngleMode) {
                            if (a[1] + a[2] == 0) throw std::runtime_error("vdiv R1+R2 cannot be 0");
                            return a[0] * a[2] / (a[1] + a[2]);
                        }};  // Vout = Vin * R2 / (R1 + R2)

        // === CALCULUS FUNCTIONS ===
        
        // Summation: sum(n) = 1+2+...+n = n(n+1)/2
        funcs_[L"sum"] = {1, [](const std::vector<double>& a, AngleMode) {
                           if (a[0] < 0 || !isNearlyInt(a[0])) throw std::runtime_error("sum needs integer >= 0");
                           double n = std::round(a[0]);
                           return n * (n + 1) / 2.0;
                       }};
        
        // Sum of squares: sum2(n) = 1²+2²+...+n² = n(n+1)(2n+1)/6
        funcs_[L"sum2"] = {1, [](const std::vector<double>& a, AngleMode) {
                            if (a[0] < 0 || !isNearlyInt(a[0])) throw std::runtime_error("sum2 needs integer >= 0");
                            double n = std::round(a[0]);
                            return n * (n + 1) * (2 * n + 1) / 6.0;
                        }};
        
        // Sum of cubes: sum3(n) = 1³+2³+...+n³ = (n(n+1)/2)²
        funcs_[L"sum3"] = {1, [](const std::vector<double>& a, AngleMode) {
                            if (a[0] < 0 || !isNearlyInt(a[0])) throw std::runtime_error("sum3 needs integer >= 0");
                            double n = std::round(a[0]);
                            double t = n * (n + 1) / 2.0;
                            return t * t;
                        }};
        
        // Geometric sum: geom(a, r, n) = a(1-r^n)/(1-r) for r≠1
        funcs_[L"geom"] = {3, [](const std::vector<double>& a, AngleMode) {
                            double a0 = a[0], r = a[1], n = a[2];
                            if (std::fabs(r - 1.0) < 1e-12) return a0 * (n + 1);
                            return a0 * (1.0 - std::pow(r, n + 1)) / (1.0 - r);
                        }};
        
        // === NUMERICAL INTEGRALS ===
        
        // Integral of x^k from a to b: intpow(a, b, k) = (b^(k+1) - a^(k+1))/(k+1)
        funcs_[L"intpow"] = {3, [](const std::vector<double>& a, AngleMode) {
                             double lo = a[0], hi = a[1], k = a[2];
                             if (std::fabs(k + 1) < 1e-12) {
                                 // k = -1, integral of 1/x = ln(x)
                                 if (lo <= 0 || hi <= 0) throw std::runtime_error("intpow: x must be > 0 for k=-1");
                                 return std::log(hi) - std::log(lo);
                             }
                             return (std::pow(hi, k + 1) - std::pow(lo, k + 1)) / (k + 1);
                         }};
        
        // Integral of e^x from a to b: intexp(a, b) = e^b - e^a
        funcs_[L"intexp"] = {2, [](const std::vector<double>& a, AngleMode) {
                             return std::exp(a[1]) - std::exp(a[0]);
                         }};
        
        // Integral of sin(x) from a to b: intsin(a, b) = -cos(b) + cos(a)
        funcs_[L"intsin"] = {2, [](const std::vector<double>& a, AngleMode) {
                             return -std::cos(a[1]) + std::cos(a[0]);
                         }};
        
        // Integral of cos(x) from a to b: intcos(a, b) = sin(b) - sin(a)
        funcs_[L"intcos"] = {2, [](const std::vector<double>& a, AngleMode) {
                             return std::sin(a[1]) - std::sin(a[0]);
                         }};
        
        // Integral of 1/x from a to b: intlog(a, b) = ln(b) - ln(a)
        funcs_[L"intlog"] = {2, [](const std::vector<double>& a, AngleMode) {
                             if (a[0] <= 0 || a[1] <= 0) throw std::runtime_error("intlog: bounds must be > 0");
                             return std::log(a[1]) - std::log(a[0]);
                         }};
        
        // === NUMERICAL DERIVATIVES (using central difference) ===
        
        // Derivative of x^n at x: derivpow(x, n, h) ≈ n*x^(n-1)
        funcs_[L"derivpow"] = {3, [](const std::vector<double>& a, AngleMode) {
                               double x = a[0], n = a[1], h = a[2];
                               if (h <= 0) h = 1e-6;
                               // Central difference: (f(x+h) - f(x-h)) / (2h)
                               double fxh = std::pow(x + h, n);
                               double fxmh = std::pow(x - h, n);
                               return (fxh - fxmh) / (2 * h);
                           }};
        
        // Derivative of e^x at x: derivexp(x, h)
        funcs_[L"derivexp"] = {2, [](const std::vector<double>& a, AngleMode) {
                               double x = a[0], h = a[1];
                               if (h <= 0) h = 1e-6;
                               return (std::exp(x + h) - std::exp(x - h)) / (2 * h);
                           }};
        
        // Derivative of sin(x) at x: derivsin(x, h)
        funcs_[L"derivsin"] = {2, [](const std::vector<double>& a, AngleMode) {
                               double x = a[0], h = a[1];
                               if (h <= 0) h = 1e-6;
                               return (std::sin(x + h) - std::sin(x - h)) / (2 * h);
                           }};
        
        // Derivative of cos(x) at x: derivcos(x, h)
        funcs_[L"derivcos"] = {2, [](const std::vector<double>& a, AngleMode) {
                               double x = a[0], h = a[1];
                               if (h <= 0) h = 1e-6;
                               return (std::cos(x + h) - std::cos(x - h)) / (2 * h);
                           }};
        
        // Derivative of ln(x) at x: derivln(x, h)
        funcs_[L"derivln"] = {2, [](const std::vector<double>& a, AngleMode) {
                              double x = a[0], h = a[1];
                              if (h <= 0) h = 1e-6;
                              if (x - h <= 0) throw std::runtime_error("derivln: x-h must be > 0");
                              return (std::log(x + h) - std::log(x - h)) / (2 * h);
                          }};
        
        // === LIMITS (numerical approximation) ===
        
        // Limit from right: limr(x0, h) - evaluates behavior as x -> x0+
        // For x^n: lim(x0, n, dir) where dir=1 for right, -1 for left
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
            if (iswspace(c)) {
                ++i;
                continue;
            }
            if (iswdigit(c) || c == L'.') {
                size_t j = i;
                bool dot = false;
                while (j < e.size()) {
                    wchar_t d = e[j];
                    if (d == L'.') {
                        if (dot) break;
                        dot = true;
                        ++j;
                    } else if (iswdigit(d)) {
                        ++j;
                    } else {
                        break;
                    }
                }
                auto s = e.substr(i, j - i);
                t.push_back({TT::Number, s, std::stod(std::string(s.begin(), s.end()))});
                i = j;
                continue;
            }
            if (iswalpha(c) || c == L'_') {
                size_t j = i;
                while (j < e.size() && (iswalnum(e[j]) || e[j] == L'_')) ++j;
                t.push_back({TT::Name, lower(e.substr(i, j - i)), 0.0});
                i = j;
                continue;
            }
            if (c == L'+' || c == L'-' || c == L'*' || c == L'/' || c == L'^' || c == L'%' || c == L'!') {
                t.push_back({TT::Operator, std::wstring(1, c), 0.0});
                ++i;
                continue;
            }
            if (c == L'(') {
                t.push_back({TT::LParen, L"(", 0.0});
                ++i;
                continue;
            }
            if (c == L')') {
                t.push_back({TT::RParen, L")", 0.0});
                ++i;
                continue;
            }
            if (c == L',') {
                t.push_back({TT::Comma, L",", 0.0});
                ++i;
                continue;
            }
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
                if (curVal && nxtVal) {
                    out.push_back({TT::Operator, L"*", 0.0});
                }
            }
        }
        return out;
    }

    std::vector<Tok> toRpn(const std::vector<Tok>& in) const {
        std::vector<Tok> out, st;
        bool expectUnary = true;
        for (size_t i = 0; i < in.size(); ++i) {
            Tok tk = in[i];
            if (tk.type == TT::Number) {
                out.push_back(tk);
                expectUnary = false;
                continue;
            }
            if (tk.type == TT::Name) {
                bool isFunc = (i + 1 < in.size() && in[i + 1].type == TT::LParen);
                if (isFunc) st.push_back(tk);
                else out.push_back(tk);
                expectUnary = false;
                continue;
            }
            if (tk.type == TT::Comma) {
                while (!st.empty() && st.back().type != TT::LParen) {
                    out.push_back(st.back());
                    st.pop_back();
                }
                if (st.empty()) throw std::runtime_error("misplaced comma");
                expectUnary = true;
                continue;
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
                    out.push_back(st.back());
                    st.pop_back();
                }
                st.push_back(tk);
                expectUnary = tk.text != L"!";
                continue;
            }
            if (tk.type == TT::LParen) {
                st.push_back(tk);
                expectUnary = true;
                continue;
            }
            if (tk.type == TT::RParen) {
                bool found = false;
                while (!st.empty()) {
                    Tok top = st.back();
                    st.pop_back();
                    if (top.type == TT::LParen) {
                        found = true;
                        break;
                    }
                    out.push_back(top);
                }
                if (!found) throw std::runtime_error("mismatched parentheses");
                if (!st.empty() && st.back().type == TT::Name) {
                    out.push_back(st.back());
                    st.pop_back();
                }
                expectUnary = false;
            }
        }
        while (!st.empty()) {
            if (st.back().type == TT::LParen) throw std::runtime_error("mismatched parentheses");
            out.push_back(st.back());
            st.pop_back();
        }
        return out;
    }

    double evalRpn(const std::vector<Tok>& rpn, AngleMode mode, const std::map<std::wstring, double>& vars) const {
        std::vector<double> st;
        for (const auto& tk : rpn) {
            if (tk.type == TT::Number) {
                st.push_back(tk.n);
            } else if (tk.type == TT::Name) {
                auto f = funcs_.find(tk.text);
                if (f != funcs_.end()) {
                    int a = f->second.arity;
                    if (static_cast<int>(st.size()) < a) throw std::runtime_error("not enough function args");
                    std::vector<double> args(a);
                    for (int i = a - 1; i >= 0; --i) {
                        args[i] = st.back();
                        st.pop_back();
                    }
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
                    double x = st.back();
                    st.pop_back();
                    if (tk.text == L"u+") st.push_back(+x);
                    else if (tk.text == L"u-") st.push_back(-x);
                    else if (tk.text == L"!") st.push_back(factorial(x));
                } else {
                    double b = st.back(); st.pop_back();
                    double a1 = st.back(); st.pop_back();
                    if (tk.text == L"+") st.push_back(a1 + b);
                    else if (tk.text == L"-") st.push_back(a1 - b);
                    else if (tk.text == L"*") st.push_back(a1 * b);
                    else if (tk.text == L"/") {
                        if (std::fabs(b) < 1e-15) throw std::runtime_error("division by zero");
                        st.push_back(a1 / b);
                    } else if (tk.text == L"%") {
                        if (std::fabs(b) < 1e-15) throw std::runtime_error("modulo by zero");
                        st.push_back(std::fmod(a1, b));
                    } else if (tk.text == L"^") st.push_back(std::pow(a1, b));
                }
            }
        }
        if (st.size() != 1) throw std::runtime_error("invalid expression");
        return st.back();
    }
};

enum : int {
    IDC_EDIT = 1000,
    IDC_DEG_RAD = 1001,
    IDC_MS = 1002,
    IDC_MR = 1003,
    IDC_MC = 1004,
    IDC_MPLUS = 1005,
    IDC_MMINUS = 1006,
    IDC_BACK = 1007,
    IDC_CLEAR = 1008,
    IDC_EQUALS = 1009,
    IDC_STATUS = 1010,
    IDC_GRAPH = 1011,
    IDC_PLOT = 1012,
    IDC_GRAPHCLEAR = 1013,
    IDC_ZOOMIN = 1014,
    IDC_ZOOMOUT = 1015,
    IDC_BTN_BASE = 2000
};

struct ButtonDef {
    const wchar_t* label;
    const wchar_t* insert;
};

const ButtonDef kButtons[] = {
    // Row 1: Trig functions
    {L"sin", L"sin("}, {L"cos", L"cos("}, {L"tan", L"tan("}, {L"asin", L"asin("}, {L"acos", L"acos("}, {L"atan", L"atan("},
    // Row 2: Math functions
    {L"sqrt", L"sqrt("}, {L"ln", L"ln("}, {L"log", L"log("}, {L"x^2", L"^2"}, {L"10^x", L"10^("}, {L"+/-", L"+/-"},
    // Row 3: Numbers and operators
    {L"7", L"7"}, {L"8", L"8"}, {L"9", L"9"}, {L"/", L"/"}, {L"(", L"("}, {L")", L")"},
    {L"4", L"4"}, {L"5", L"5"}, {L"6", L"6"}, {L"*", L"*"}, {L"^", L"^"}, {L"!", L"!"},
    {L"1", L"1"}, {L"2", L"2"}, {L"3", L"3"}, {L"-", L"-"}, {L"pi", L"pi"}, {L"e", L"e"},
    {L"0", L"0"}, {L".", L"."}, {L"Ans", L"ans"}, {L"+", L"+"}, {L",", L","}, {L"%", L"%"},
    // Row 7: Ohm's Law - Power calculations
    {L"P=VI", L"pvi("}, {L"P=I²R", L"pir("}, {L"P=V²/R", L"pvr("}, {L"V=IR", L"vir("}, {L"I=V/R", L"ivr("}, {L"R=V/I", L"rvi("},
    // Row 8: Derived power calculations
    {L"V=P/I", L"vpi("}, {L"I=P/V", L"ipv("}, {L"V=√PR", L"vpr("}, {L"I=√P/R", L"ipr("}, {L"Z=√(R²X²)", L"zrx("}, {L"PF", L"pf("},
    // Row 9: AC Power & Impedance
    {L"Preal", L"preal("}, {L"Preact", L"preact("}, {L"Papp", L"papp("}, {L"Xc", L"xc("}, {L"Xl", L"xl("}, {L"fres", L"fres("},
    // Row 10: Decibels & Voltage Divider
    {L"dB(V)", L"dbv("}, {L"dB(P)", L"dbp("}, {L"Vdiv", L"vdiv("}, {L"abs", L"abs("}, {L"pow", L"pow("}, {L"max", L"max("},
    // Row 11: Calculus - Summations
    {L"Σ(n)", L"sum("}, {L"Σ(n²)", L"sum2("}, {L"Σ(n³)", L"sum3("}, {L"geom", L"geom("}, {L"min", L"min("}, {L"max", L"max("},
    // Row 12: Calculus - Integrals
    {L"∫x^n", L"intpow("}, {L"∫e^x", L"intexp("}, {L"∫sin", L"intsin("}, {L"∫cos", L"intcos("}, {L"∫1/x", L"intlog("}, {L"lim", L"limpow("},
    // Row 13: Calculus - Derivatives
    {L"d/dx x^n", L"derivpow("}, {L"d/dx e^x", L"derivexp("}, {L"d/dx sin", L"derivsin("}, {L"d/dx cos", L"derivcos("}, {L"d/dx ln", L"derivln("}, {L"h=1e-6", L"0.000001"},
    // Row 14: Graphing Presets - Basic Functions
    {L"y=sin(x)", L"sin(x)"}, {L"y=cos(x)", L"cos(x)"}, {L"y=tan(x)", L"tan(x)"}, {L"y=x²", L"x^2"}, {L"y=x³", L"x^3"}, {L"y=√x", L"sqrt(abs(x))"},
    // Row 15: Graphing Presets - More Functions
    {L"y=e^x", L"e^x"}, {L"y=ln(x)", L"ln(abs(x))"}, {L"y=1/x", L"1/x"}, {L"y=|x|", L"abs(x)"}, {L"y=sin(2x)", L"sin(2*x)"}, {L"y=cos(2x)", L"cos(2*x)"},
    // Row 16: Graphing Presets - Wave Functions
    {L"y=e^(-x)sin", L"e^(-abs(x))*sin(x)"}, {L"y=sin+cos", L"sin(x)+cos(x)"}, {L"y=sin²x", L"sin(x)^2"}, {L"y=cos²x", L"cos(x)^2"}, {L"y=sin(x)/x", L"sin(x)/(x+0.0001)"}, {L"y=x*sin(x)", L"x*sin(x)"},
};

// Example expressions that can be copied (without the EX: prefix)
std::wstring getExampleExpression(const std::wstring& insertText) {
    // Trig functions
    if (insertText == L"sin(") return L"sin(45)";
    if (insertText == L"cos(") return L"cos(60)";
    if (insertText == L"tan(") return L"tan(45)";
    if (insertText == L"asin(") return L"asin(0.5)";
    if (insertText == L"acos(") return L"acos(0.5)";
    if (insertText == L"atan(") return L"atan(1)";
    if (insertText == L"sqrt(") return L"sqrt(16)";
    if (insertText == L"ln(") return L"ln(e)";
    if (insertText == L"log(") return L"log(100)";
    if (insertText == L"10^(") return L"10^(2)";
    if (insertText == L"^2") return L"5^2";
    
    // EE Functions
    if (insertText == L"pvi(") return L"pvi(12,2)";
    if (insertText == L"pir(") return L"pir(2,10)";
    if (insertText == L"pvr(") return L"pvr(12,6)";
    if (insertText == L"vir(") return L"vir(2,10)";
    if (insertText == L"ivr(") return L"ivr(12,4)";
    if (insertText == L"rvi(") return L"rvi(12,3)";
    if (insertText == L"vpi(") return L"vpi(24,2)";
    if (insertText == L"ipv(") return L"ipv(24,12)";
    if (insertText == L"vpr(") return L"vpr(100,4)";
    if (insertText == L"ipr(") return L"ipr(100,4)";
    if (insertText == L"zrx(") return L"zrx(3,4)";
    if (insertText == L"pf(") return L"pf(30)";
    if (insertText == L"preal(") return L"preal(120,5,30)";
    if (insertText == L"preact(") return L"preact(120,5,30)";
    if (insertText == L"papp(") return L"papp(120,5)";
    if (insertText == L"xc(") return L"xc(1000,0.000001)";
    if (insertText == L"xl(") return L"xl(1000,0.001)";
    if (insertText == L"fres(") return L"fres(0.001,0.000001)";
    if (insertText == L"dbv(") return L"dbv(10,1)";
    if (insertText == L"dbp(") return L"dbp(100,1)";
    if (insertText == L"vdiv(") return L"vdiv(12,1000,1000)";
    if (insertText == L"abs(") return L"abs(-5)";
    if (insertText == L"pow(") return L"pow(2,3)";
    if (insertText == L"min(") return L"min(3,7)";
    if (insertText == L"max(") return L"max(3,7)";
    
    // Calculus
    if (insertText == L"sum(") return L"sum(10)";
    if (insertText == L"sum2(") return L"sum2(3)";
    if (insertText == L"sum3(") return L"sum3(3)";
    if (insertText == L"geom(") return L"geom(1,2,3)";
    if (insertText == L"intpow(") return L"intpow(0,2,3)";
    if (insertText == L"intexp(") return L"intexp(0,1)";
    if (insertText == L"intsin(") return L"intsin(0,pi)";
    if (insertText == L"intcos(") return L"intcos(0,pi)";
    if (insertText == L"intlog(") return L"intlog(1,e)";
    if (insertText == L"derivpow(") return L"derivpow(2,3,0.000001)";
    if (insertText == L"derivexp(") return L"derivexp(1,0.000001)";
    if (insertText == L"derivsin(") return L"derivsin(0,0.000001)";
    if (insertText == L"derivcos(") return L"derivcos(0,0.000001)";
    if (insertText == L"derivln(") return L"derivln(2,0.000001)";
    if (insertText == L"limpow(") return L"limpow(0,2,1)";
    if (insertText == L"0.000001") return L"0.000001";
    
    return L"";
}

// Help text for each function - shows what inputs are needed with examples
std::wstring getFunctionHelp(const std::wstring& insertText) {
    // Trig functions
    if (insertText == L"sin(") return L"EX: sin(45) in DEG mode = 0.707";
    if (insertText == L"cos(") return L"EX: cos(60) in DEG mode = 0.5";
    if (insertText == L"tan(") return L"EX: tan(45) in DEG mode = 1";
    if (insertText == L"asin(") return L"EX: asin(0.5) in DEG mode = 30";
    if (insertText == L"acos(") return L"EX: acos(0.5) in DEG mode = 60";
    if (insertText == L"atan(") return L"EX: atan(1) in DEG mode = 45";
    if (insertText == L"sqrt(") return L"EX: sqrt(16) = 4";
    if (insertText == L"ln(") return L"EX: ln(2.718) = 1 (natural log)";
    if (insertText == L"log(") return L"EX: log(100) = 2 (base 10)";
    if (insertText == L"10^(") return L"EX: 10^(2) = 100";
    if (insertText == L"^2") return L"EX: 5^2 = 25 (squares previous number)";
    
    // EE Functions - Ohm's Law
    if (insertText == L"pvi(") return L"EX: pvi(12,2) = 24W (12V × 2A)";
    if (insertText == L"pir(") return L"EX: pir(2,10) = 40W (2²A × 10Ω)";
    if (insertText == L"pvr(") return L"EX: pvr(12,6) = 24W (12²V / 6Ω)";
    if (insertText == L"vir(") return L"EX: vir(2,10) = 20V (2A × 10Ω)";
    if (insertText == L"ivr(") return L"EX: ivr(12,4) = 3A (12V / 4Ω)";
    if (insertText == L"rvi(") return L"EX: rvi(12,3) = 4Ω (12V / 3A)";
    
    // Derived calculations
    if (insertText == L"vpi(") return L"EX: vpi(24,2) = 12V (24W / 2A)";
    if (insertText == L"ipv(") return L"EX: ipv(24,12) = 2A (24W / 12V)";
    if (insertText == L"vpr(") return L"EX: vpr(100,4) = 20V (√100×4)";
    if (insertText == L"ipr(") return L"EX: ipr(100,4) = 5A (√100/4)";
    if (insertText == L"zrx(") return L"EX: zrx(3,4) = 5Ω (√3²+4²)";
    if (insertText == L"pf(") return L"EX: pf(30) in DEG = 0.866 (cos30°)";
    
    // AC Power
    if (insertText == L"preal(") return L"EX: preal(120,5,30) = 519.6W (DEG mode)";
    if (insertText == L"preact(") return L"EX: preact(120,5,30) = 300VAR (DEG)";
    if (insertText == L"papp(") return L"EX: papp(120,5) = 600VA";
    
    // Reactance & Resonance
    if (insertText == L"xc(") return L"EX: xc(1000,0.000001) = 159Ω (1kHz,1µF)";
    if (insertText == L"xl(") return L"EX: xl(1000,0.001) = 6.28Ω (1kHz,1mH)";
    if (insertText == L"fres(") return L"EX: fres(0.001,0.000001) = 5033Hz";
    
    // Decibels
    if (insertText == L"dbv(") return L"EX: dbv(10,1) = 20dB (voltage gain)";
    if (insertText == L"dbp(") return L"EX: dbp(100,1) = 20dB (power gain)";
    if (insertText == L"vdiv(") return L"EX: vdiv(12,1000,1000) = 6V";
    
    // Math utilities
    if (insertText == L"abs(") return L"EX: abs(-5) = 5";
    if (insertText == L"pow(") return L"EX: pow(2,3) = 8 (2³)";
    if (insertText == L"min(") return L"EX: min(3,7) = 3";
    if (insertText == L"max(") return L"EX: max(3,7) = 7";
    
    // Calculus - Summations
    if (insertText == L"sum(") return L"EX: sum(10) = 55 (1+2+...+10)";
    if (insertText == L"sum2(") return L"EX: sum2(3) = 14 (1²+2²+3²)";
    if (insertText == L"sum3(") return L"EX: sum3(3) = 36 (1³+2³+3³)";
    if (insertText == L"geom(") return L"EX: geom(1,2,3) = 15 (1+2+4+8)";
    
    // Calculus - Integrals
    if (insertText == L"intpow(") return L"EX: intpow(0,2,3) = 4 (∫₀² x³ dx)";
    if (insertText == L"intexp(") return L"EX: intexp(0,1) = 1.718 (∫₀¹ e^x dx)";
    if (insertText == L"intsin(") return L"EX: intsin(0,pi) = 2 (∫sin x dx)";
    if (insertText == L"intcos(") return L"EX: intcos(0,pi) = 0 (∫cos x dx)";
    if (insertText == L"intlog(") return L"EX: intlog(1,e) = 1 (∫1/x dx)";
    
    // Calculus - Derivatives
    if (insertText == L"derivpow(") return L"EX: derivpow(2,3,0.000001) = 12 (d/dx x³ at x=2)";
    if (insertText == L"derivexp(") return L"EX: derivexp(1,0.000001) = 2.718 (d/dx e^x)";
    if (insertText == L"derivsin(") return L"EX: derivsin(0,0.000001) = 1 (d/dx sin at 0)";
    if (insertText == L"derivcos(") return L"EX: derivcos(0,0.000001) = 0 (d/dx cos at 0)";
    if (insertText == L"derivln(") return L"EX: derivln(2,0.000001) = 0.5 (d/dx ln at 2)";
    if (insertText == L"limpow(") return L"EX: limpow(0,2,1) = 0 (lim x² as x→0+)";
    if (insertText == L"0.000001") return L"Default h value for numerical derivatives";
    
    return L"";
}

ExpressionEngine g_engine;
double g_ans = 0.0;
double g_mem = 0.0;
bool g_justEvaluated = false;
AngleMode g_mode = AngleMode::Radians;

// Graph state
HWND g_hwndGraph = nullptr;
double g_graphXMin = -10.0, g_graphXMax = 10.0;
double g_graphYMin = -10.0, g_graphYMax = 10.0;
std::wstring g_graphExpr;
std::wstring g_lastExampleExpr;  // Last example shown in status bar

WNDPROC g_origEditProc = nullptr;

HFONT g_fontDisplay = nullptr;
HFONT g_fontButton = nullptr;
HBRUSH g_brushWindow = nullptr;
HBRUSH g_brushEdit = nullptr;
HBRUSH g_brushStatus = nullptr;

COLORREF buttonBgColor(int id) {
    if (id == IDC_EQUALS) return RGB(33, 150, 243);
    if (id == IDC_CLEAR || id == IDC_BACK) return RGB(239, 83, 80);
    if (id == IDC_DEG_RAD) return RGB(171, 71, 188);
    if (id >= IDC_MS && id <= IDC_MMINUS) return RGB(0, 150, 136);
    if (id >= IDC_BTN_BASE && id < IDC_BTN_BASE + static_cast<int>(std::size(kButtons))) {
        const std::wstring t = kButtons[id - IDC_BTN_BASE].label;
        // Operators (orange)
        if (t == L"+" || t == L"-" || t == L"*" || t == L"/" || t == L"^" || t == L"%" || t == L"!") return RGB(255, 167, 38);
        // Scientific functions (light blue)
        if (t == L"sin" || t == L"cos" || t == L"tan" || t == L"asin" || t == L"acos" || t == L"atan" ||
            t == L"sqrt" || t == L"ln" || t == L"log" || t == L"x^2" || t == L"10^x") return RGB(66, 165, 245);
        if (t == L"+/-" || t == L"Ans") return RGB(0, 150, 136);
        // Electrical Engineering functions (green/teal)
        if (t == L"P=VI" || t == L"P=I²R" || t == L"P=V²/R" || t == L"V=IR" || t == L"I=V/R" || t == L"R=V/I" ||
            t == L"V=P/I" || t == L"I=P/V" || t == L"V=√PR" || t == L"I=√P/R" || t == L"Z=√(R²+X²)" || t == L"PF") return RGB(76, 175, 80);
        // AC Power & Impedance (cyan)
        if (t == L"Preal" || t == L"Preact" || t == L"Papp" || t == L"Xc" || t == L"Xl" || t == L"fres") return RGB(0, 188, 212);
        // Decibels & misc (purple)
        if (t == L"dB(V)" || t == L"dB(P)" || t == L"Vdiv" || t == L"abs" || t == L"pow" || t == L"min(") return RGB(156, 39, 176);
        // Calculus - Summations (amber/orange)
        if (t == L"Σ(n)" || t == L"Σ(n²)" || t == L"Σ(n³)" || t == L"geom") return RGB(255, 193, 7);
        // Calculus - Integrals (red/pink)
        if (t == L"∫x^n" || t == L"∫e^x" || t == L"∫sin" || t == L"∫cos" || t == L"∫1/x" || t == L"lim") return RGB(244, 67, 54);
        // Calculus - Derivatives (indigo)
        if (t == L"d/dx x^n" || t == L"d/dx e^x" || t == L"d/dx sin" || t == L"d/dx cos" || t == L"d/dx ln" || t == L"h=1e-6") return RGB(63, 81, 181);
        // Graphing Presets - Basic (bright green)
        if (t == L"y=sin(x)" || t == L"y=cos(x)" || t == L"y=tan(x)" || t == L"y=x²" || t == L"y=x³" || t == L"y=√x") return RGB(0, 200, 83);
        // Graphing Presets - More (teal)
        if (t == L"y=e^x" || t == L"y=ln(x)" || t == L"y=1/x" || t == L"y=|x|" || t == L"y=sin(2x)" || t == L"y=cos(2x)") return RGB(0, 172, 193);
        // Graphing Presets - Wave (pink/magenta)
        if (t.find(L"y=") == 0) return RGB(233, 30, 99);
    }
    return RGB(97, 97, 97);
}

std::wstring getText(HWND h) {
    int len = GetWindowTextLengthW(h);
    std::wstring s(len, L'\0');
    GetWindowTextW(h, &s[0], len + 1);
    return s;
}

void setText(HWND h, const std::wstring& s) {
    SetWindowTextW(h, s.c_str());
}

void setStatus(HWND hwnd, const std::wstring& msg) {
    setText(GetDlgItem(hwnd, IDC_STATUS), msg);
}

void appendToEdit(HWND edit, const std::wstring& text) {
    DWORD selStart = 0, selEnd = 0;
    SendMessageW(edit, EM_GETSEL, reinterpret_cast<WPARAM>(&selStart), reinterpret_cast<LPARAM>(&selEnd));
    SendMessageW(edit, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(text.c_str()));
}

void evaluateNow(HWND hwnd) {
    HWND edit = GetDlgItem(hwnd, IDC_EDIT);
    std::wstring expr = getText(edit);
    if (expr.empty()) return;
    // Auto-close unmatched parentheses
    int openParens = 0;
    for (wchar_t c : expr) {
        if (c == L'(') openParens++;
        else if (c == L')') openParens--;
    }
    for (int i = 0; i < openParens; i++) expr += L")";
    try {
        double result = g_engine.evaluate(expr, g_mode, g_ans, g_mem);
        g_ans = result;
        std::wostringstream ss;
        ss.precision(15);
        ss << result;
        setText(edit, ss.str());
        setStatus(hwnd, L"OK");
        g_justEvaluated = true;
    } catch (...) {
        setStatus(hwnd, L"Error: invalid expression or domain");
    }
}

LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN && wParam == VK_RETURN) {
        evaluateNow(GetParent(hwnd));
        return 0;
    }
    if (msg == WM_CHAR) {
        if (wParam == L'\r') return 0;
        if (g_justEvaluated) {
            wchar_t ch = static_cast<wchar_t>(wParam);
            bool isOp = (ch == L'+' || ch == L'-' || ch == L'*' || ch == L'/' || ch == L'^' || ch == L'%' || ch == L'!');
            if (!isOp && ch != L'\b') {
                SetWindowTextW(hwnd, L"");
            }
            g_justEvaluated = false;
        }
    }
    return CallWindowProcW(g_origEditProc, hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_brushWindow = CreateSolidBrush(RGB(32, 32, 32));
        g_brushEdit = CreateSolidBrush(RGB(18, 18, 18));
        g_brushStatus = CreateSolidBrush(RGB(32, 32, 32));

        g_fontDisplay = CreateFontW(28, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        g_fontButton = CreateFontW(18, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                   CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        HWND hEdit = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
                        10, 10, 535, 30, hwnd, reinterpret_cast<HMENU>(IDC_EDIT), nullptr, nullptr);
        SendMessageW(hEdit, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontDisplay), TRUE);
        g_origEditProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(hEdit, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(EditSubclassProc)));

        HWND b = CreateWindowW(L"BUTTON", L"RAD/DEG", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      10, 50, 80, 28, hwnd, reinterpret_cast<HMENU>(IDC_DEG_RAD), nullptr, nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        b = CreateWindowW(L"BUTTON", L"MS", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      100, 50, 45, 28, hwnd, reinterpret_cast<HMENU>(IDC_MS), nullptr, nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        b = CreateWindowW(L"BUTTON", L"MR", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      150, 50, 45, 28, hwnd, reinterpret_cast<HMENU>(IDC_MR), nullptr, nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        b = CreateWindowW(L"BUTTON", L"MC", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      200, 50, 45, 28, hwnd, reinterpret_cast<HMENU>(IDC_MC), nullptr, nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        b = CreateWindowW(L"BUTTON", L"M+", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      250, 50, 45, 28, hwnd, reinterpret_cast<HMENU>(IDC_MPLUS), nullptr, nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        b = CreateWindowW(L"BUTTON", L"M-", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      300, 50, 45, 28, hwnd, reinterpret_cast<HMENU>(IDC_MMINUS), nullptr, nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        b = CreateWindowW(L"BUTTON", L"Back", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      355, 50, 55, 28, hwnd, reinterpret_cast<HMENU>(IDC_BACK), nullptr, nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        b = CreateWindowW(L"BUTTON", L"Clear", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      415, 50, 55, 28, hwnd, reinterpret_cast<HMENU>(IDC_CLEAR), nullptr, nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        b = CreateWindowW(L"BUTTON", L"=", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      475, 50, 70, 28, hwnd, reinterpret_cast<HMENU>(IDC_EQUALS), nullptr, nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);

        int x0 = 10, y0 = 90, bw = 85, bh = 34, cols = 6;
        for (int i = 0; i < static_cast<int>(std::size(kButtons)); ++i) {
            int row = i / cols, col = i % cols;
            HWND hb = CreateWindowW(L"BUTTON", kButtons[i].label, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                          x0 + col * (bw + 5), y0 + row * (bh + 5), bw, bh,
                          hwnd, reinterpret_cast<HMENU>(IDC_BTN_BASE + i), nullptr, nullptr);
            SendMessageW(hb, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        }

        // Help/Example bar - clickable to copy example
        HWND hs = CreateWindowW(L"BUTTON", L"Mode: RAD - Click example to copy", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      10, 720, 535, 32, hwnd, reinterpret_cast<HMENU>(IDC_STATUS), nullptr, nullptr);
        SendMessageW(hs, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        
        // Graph panel on right side
        g_hwndGraph = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
                      555, 10, 280, 630, hwnd, reinterpret_cast<HMENU>(IDC_GRAPH), nullptr, nullptr);
        
        // Graph control buttons
        HWND btnPlot = CreateWindowW(L"BUTTON", L"Plot", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      555, 650, 65, 28, hwnd, reinterpret_cast<HMENU>(IDC_PLOT), nullptr, nullptr);
        SendMessageW(btnPlot, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        
        HWND btnClear = CreateWindowW(L"BUTTON", L"Clear", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      625, 650, 65, 28, hwnd, reinterpret_cast<HMENU>(IDC_GRAPHCLEAR), nullptr, nullptr);
        SendMessageW(btnClear, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        
        HWND btnZoomIn = CreateWindowW(L"BUTTON", L"Zoom+", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      695, 650, 55, 28, hwnd, reinterpret_cast<HMENU>(IDC_ZOOMIN), nullptr, nullptr);
        SendMessageW(btnZoomIn, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        
        HWND btnZoomOut = CreateWindowW(L"BUTTON", L"Zoom-", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      755, 650, 55, 28, hwnd, reinterpret_cast<HMENU>(IDC_ZOOMOUT), nullptr, nullptr);
        SendMessageW(btnZoomOut, WM_SETFONT, reinterpret_cast<WPARAM>(g_fontButton), TRUE);
        
        return 0;
    }

    case WM_CTLCOLOREDIT: {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdc, RGB(120, 255, 120));
        SetBkColor(hdc, RGB(18, 18, 18));
        return reinterpret_cast<LRESULT>(g_brushEdit);
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdc, RGB(220, 220, 220));
        SetBkColor(hdc, RGB(32, 32, 32));
        return reinterpret_cast<LRESULT>(g_brushStatus);
    }

    case WM_ERASEBKGND: {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(reinterpret_cast<HDC>(wParam), &rc, g_brushWindow);
        return 1;
    }

    case WM_DRAWITEM: {
        const LPDRAWITEMSTRUCT dis = reinterpret_cast<LPDRAWITEMSTRUCT>(lParam);
        
        // Handle graph panel drawing
        if (dis->CtlID == IDC_GRAPH) {
            RECT rc = dis->rcItem;
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;
            
            // Fill background
            HBRUSH bgBrush = CreateSolidBrush(RGB(20, 20, 30));
            FillRect(dis->hDC, &rc, bgBrush);
            DeleteObject(bgBrush);
            
            // Draw grid
            HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 60));
            SelectObject(dis->hDC, gridPen);
            
            // Vertical grid lines
            for (int i = 0; i <= 10; i++) {
                int x = rc.left + (width * i) / 10;
                MoveToEx(dis->hDC, x, rc.top, nullptr);
                LineTo(dis->hDC, x, rc.bottom);
            }
            // Horizontal grid lines
            for (int i = 0; i <= 10; i++) {
                int y = rc.top + (height * i) / 10;
                MoveToEx(dis->hDC, rc.left, y, nullptr);
                LineTo(dis->hDC, rc.right, y);
            }
            DeleteObject(gridPen);
            
            // Draw axes
            HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 120));
            SelectObject(dis->hDC, axisPen);
            
            // X axis (y=0)
            int yAxisPixel = rc.top + static_cast<int>((0 - g_graphYMin) / (g_graphYMax - g_graphYMin) * height);
            if (yAxisPixel >= rc.top && yAxisPixel <= rc.bottom) {
                MoveToEx(dis->hDC, rc.left, yAxisPixel, nullptr);
                LineTo(dis->hDC, rc.right, yAxisPixel);
            }
            // Y axis (x=0)
            int xAxisPixel = rc.left + static_cast<int>((0 - g_graphXMin) / (g_graphXMax - g_graphXMin) * width);
            if (xAxisPixel >= rc.left && xAxisPixel <= rc.right) {
                MoveToEx(dis->hDC, xAxisPixel, rc.top, nullptr);
                LineTo(dis->hDC, xAxisPixel, rc.bottom);
            }
            DeleteObject(axisPen);
            
            // Draw function if we have one
            if (!g_graphExpr.empty()) {
                HPEN funcPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 100));
                SelectObject(dis->hDC, funcPen);
                
                bool firstPoint = true;
                for (int px = 0; px < width; px++) {
                    double x = g_graphXMin + (static_cast<double>(px) / width) * (g_graphXMax - g_graphXMin);
                    
                    // Substitute x value into expression
                    std::wostringstream ss;
                    ss.precision(10);
                    ss << L"(" << x << L")";
                    std::wstring xVal = ss.str();
                    
                    // Replace 'x' with the value
                    std::wstring evalExpr = g_graphExpr;
                    size_t pos = 0;
                    while ((pos = evalExpr.find(L'x', pos)) != std::wstring::npos) {
                        evalExpr.replace(pos, 1, xVal);
                        pos += xVal.length();
                    }
                    
                    try {
                        double y = g_engine.evaluate(evalExpr, g_mode, g_ans, g_mem);
                        
                        // Check for valid y value
                        if (!std::isnan(y) && !std::isinf(y) && 
                            y >= g_graphYMin - 100 && y <= g_graphYMax + 100) {
                            
                            int py = rc.bottom - static_cast<int>((y - g_graphYMin) / (g_graphYMax - g_graphYMin) * height);
                            
                            if (py >= rc.top && py <= rc.bottom) {
                                if (firstPoint) {
                                    MoveToEx(dis->hDC, rc.left + px, py, nullptr);
                                    firstPoint = false;
                                } else {
                                    LineTo(dis->hDC, rc.left + px, py);
                                }
                            } else {
                                firstPoint = true;
                            }
                        } else {
                            firstPoint = true;
                        }
                    } catch (...) {
                        firstPoint = true;
                    }
                }
                DeleteObject(funcPen);
            }
            
            // Draw axis labels
            SetBkMode(dis->hDC, TRANSPARENT);
            SetTextColor(dis->hDC, RGB(180, 180, 180));
            
            // X axis labels
            std::wostringstream xMinStr, xMaxStr;
            xMinStr.precision(1); xMinStr << std::fixed << g_graphXMin;
            xMaxStr.precision(1); xMaxStr << std::fixed << g_graphXMax;
            TextOutW(dis->hDC, rc.left + 5, rc.bottom - 18, xMinStr.str().c_str(), static_cast<int>(xMinStr.str().length()));
            TextOutW(dis->hDC, rc.right - 35, rc.bottom - 18, xMaxStr.str().c_str(), static_cast<int>(xMaxStr.str().length()));
            
            // Y axis labels
            std::wostringstream yMinStr, yMaxStr;
            yMinStr.precision(1); yMinStr << std::fixed << g_graphYMin;
            yMaxStr.precision(1); yMaxStr << std::fixed << g_graphYMax;
            TextOutW(dis->hDC, rc.left + 5, rc.bottom - 35, yMinStr.str().c_str(), static_cast<int>(yMinStr.str().length()));
            TextOutW(dis->hDC, rc.left + 5, rc.top + 5, yMaxStr.str().c_str(), static_cast<int>(yMaxStr.str().length()));
            
            // Draw "y=" label with current expression
            if (!g_graphExpr.empty()) {
                std::wstring label = L"y=" + g_graphExpr;
                if (label.length() > 25) label = label.substr(0, 25) + L"...";
                TextOutW(dis->hDC, rc.left + 5, rc.top + 25, label.c_str(), static_cast<int>(label.length()));
            }
            
            return TRUE;
        }
        
        if (dis->CtlType != ODT_BUTTON) break;

        const int id = static_cast<int>(dis->CtlID);
        COLORREF bg = buttonBgColor(id);
        if (dis->itemState & ODS_SELECTED) {
            bg = RGB(GetRValue(bg) * 3 / 4, GetGValue(bg) * 3 / 4, GetBValue(bg) * 3 / 4);
        }

        HBRUSH b = CreateSolidBrush(bg);
        FillRect(dis->hDC, &dis->rcItem, b);
        DeleteObject(b);

        FrameRect(dis->hDC, &dis->rcItem, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

        wchar_t text[64]{};
        GetWindowTextW(dis->hwndItem, text, 63);
        SetBkMode(dis->hDC, TRANSPARENT);
        SetTextColor(dis->hDC, RGB(255, 255, 255));
        SelectObject(dis->hDC, g_fontButton);
        DrawTextW(dis->hDC, text, -1, const_cast<RECT*>(&dis->rcItem), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return TRUE;
    }

    case WM_COMMAND: {
        const int id = LOWORD(wParam);
        HWND edit = GetDlgItem(hwnd, IDC_EDIT);
        if (id >= IDC_BTN_BASE && id < IDC_BTN_BASE + static_cast<int>(std::size(kButtons))) {
            const std::wstring ins = kButtons[id - IDC_BTN_BASE].insert;
            // Handle +/- (negate) specially
            if (ins == L"+/-") {
                std::wstring cur = getText(edit);
                if (g_justEvaluated) {
                    g_ans = -g_ans;
                    std::wostringstream ss;
                    ss.precision(15);
                    ss << g_ans;
                    setText(edit, ss.str());
                } else if (!cur.empty() && cur[0] == L'-') {
                    setText(edit, cur.substr(1));
                } else {
                    setText(edit, L"-" + cur);
                }
                g_justEvaluated = false;
                SetFocus(edit);
                return 0;
            }
            // Operators chain after evaluation; other keys start fresh
            bool isOp = (ins == L"+" || ins == L"-" || ins == L"*" || ins == L"/" ||
                         ins == L"^" || ins == L"!" || ins == L"%" || ins == L"^2");
            if (g_justEvaluated) {
                if (!isOp) setText(edit, L"");
                g_justEvaluated = false;
            }
            appendToEdit(edit, ins);
            // Show help text for this function in status bar
            std::wstring helpText = getFunctionHelp(ins);
            std::wstring exampleExpr = getExampleExpression(ins);
            if (!helpText.empty()) {
                setStatus(hwnd, helpText);
                g_lastExampleExpr = exampleExpr;  // Store for copying
            }
            SetFocus(edit);
            return 0;
        }
        switch (id) {
        case IDC_DEG_RAD:
            g_mode = (g_mode == AngleMode::Radians) ? AngleMode::Degrees : AngleMode::Radians;
            setStatus(hwnd, g_mode == AngleMode::Radians ? L"Mode: RAD" : L"Mode: DEG");
            return 0;
        case IDC_MS:
            g_mem = g_ans;
            setStatus(hwnd, L"Memory stored");
            return 0;
        case IDC_MR:
            {
                if (g_justEvaluated) {
                    setText(edit, L"");
                    g_justEvaluated = false;
                }
                std::wostringstream ss;
                ss.precision(15);
                ss << g_mem;
                appendToEdit(edit, ss.str());
            }
            return 0;
        case IDC_MC:
            g_mem = 0.0;
            setStatus(hwnd, L"Memory cleared");
            return 0;
        case IDC_MPLUS:
            g_mem += g_ans;
            setStatus(hwnd, L"Memory += ans");
            return 0;
        case IDC_MMINUS:
            g_mem -= g_ans;
            setStatus(hwnd, L"Memory -= ans");
            return 0;
        case IDC_BACK: {
            std::wstring s = getText(edit);
            if (!s.empty()) s.pop_back();
            setText(edit, s);
            g_justEvaluated = false;
            return 0;
        }
        case IDC_CLEAR:
            setText(edit, L"");
            setStatus(hwnd, g_mode == AngleMode::Radians ? L"Mode: RAD" : L"Mode: DEG");
            g_justEvaluated = false;
            return 0;
        case IDC_EQUALS:
            evaluateNow(hwnd);
            return 0;
        case IDC_STATUS:
            // Click on status bar copies the example to edit field
            if (!g_lastExampleExpr.empty()) {
                setText(edit, g_lastExampleExpr);
                g_justEvaluated = false;
                SetFocus(edit);
            }
            return 0;
        case IDC_PLOT:
            // Copy current expression to graph and redraw
            g_graphExpr = getText(edit);
            
            // Auto-zoom: find Y range by sampling the function
            if (!g_graphExpr.empty()) {
                double yMin = 1e30, yMax = -1e30;
                bool foundValid = false;
                
                for (int px = 0; px < 280; px++) {
                    double x = g_graphXMin + (static_cast<double>(px) / 280.0) * (g_graphXMax - g_graphXMin);
                    
                    std::wostringstream ss;
                    ss.precision(10);
                    ss << L"(" << x << L")";
                    std::wstring xVal = ss.str();
                    
                    std::wstring evalExpr = g_graphExpr;
                    size_t pos = 0;
                    while ((pos = evalExpr.find(L'x', pos)) != std::wstring::npos) {
                        evalExpr.replace(pos, 1, xVal);
                        pos += xVal.length();
                    }
                    
                    try {
                        double y = g_engine.evaluate(evalExpr, g_mode, g_ans, g_mem);
                        if (!std::isnan(y) && !std::isinf(y) && std::fabs(y) < 1e10) {
                            if (y < yMin) yMin = y;
                            if (y > yMax) yMax = y;
                            foundValid = true;
                        }
                    } catch (...) {}
                }
                
                if (foundValid && yMax > yMin) {
                    // Add 10% padding
                    double padding = (yMax - yMin) * 0.1;
                    g_graphYMin = yMin - padding;
                    g_graphYMax = yMax + padding;
                    
                    // Ensure we don't zoom too far
                    if (g_graphYMax - g_graphYMin < 0.1) {
                        double mid = (g_graphYMax + g_graphYMin) / 2;
                        g_graphYMin = mid - 0.5;
                        g_graphYMax = mid + 0.5;
                    }
                }
            }
            
            setStatus(hwnd, L"Graphing: " + g_graphExpr);
            InvalidateRect(g_hwndGraph, nullptr, TRUE);
            return 0;
        case IDC_GRAPHCLEAR:
            g_graphExpr.clear();
            setStatus(hwnd, L"Graph cleared");
            InvalidateRect(g_hwndGraph, nullptr, TRUE);
            return 0;
        case IDC_ZOOMIN: {
            double xRange = g_graphXMax - g_graphXMin;
            double yRange = g_graphYMax - g_graphYMin;
            g_graphXMin += xRange * 0.2;
            g_graphXMax -= xRange * 0.2;
            g_graphYMin += yRange * 0.2;
            g_graphYMax -= yRange * 0.2;
            setStatus(hwnd, L"Zoomed in");
            InvalidateRect(g_hwndGraph, nullptr, TRUE);
            return 0;
        }
        case IDC_ZOOMOUT: {
            double xRange = g_graphXMax - g_graphXMin;
            double yRange = g_graphYMax - g_graphYMin;
            g_graphXMin -= xRange * 0.25;
            g_graphXMax += xRange * 0.25;
            g_graphYMin -= yRange * 0.25;
            g_graphYMax += yRange * 0.25;
            setStatus(hwnd, L"Zoomed out");
            InvalidateRect(g_hwndGraph, nullptr, TRUE);
            return 0;
        }
        }
        break;
    }

    case WM_KEYDOWN:
        if (wParam == VK_RETURN) {
            evaluateNow(hwnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        if (g_fontDisplay) DeleteObject(g_fontDisplay);
        if (g_fontButton) DeleteObject(g_fontButton);
        if (g_brushWindow) DeleteObject(g_brushWindow);
        if (g_brushEdit) DeleteObject(g_brushEdit);
        if (g_brushStatus) DeleteObject(g_brushStatus);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

} // namespace

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t* kClassName = L"ScientificCalculatorWnd";
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc)) return 1;

    HWND hwnd = CreateWindowExW(
        0, kClassName, L"Scientific Calculator with EE, Calculus & Graphing",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 870, 860,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) return 1;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
