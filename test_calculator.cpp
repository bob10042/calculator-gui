// Calculator Diagnostic Test Suite
// Compile with: g++ -std=c++17 test_calculator.cpp -o test_calculator.exe

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <functional>
#include <iomanip>

constexpr double kPi = 3.14159265358979323846;
constexpr double kE = 2.71828182845904523536;

int testsPassed = 0;
int testsFailed = 0;

void test(const std::string& name, double got, double expected, double tolerance = 1e-9) {
    bool pass = std::fabs(got - expected) < tolerance;
    if (pass) {
        std::cout << "[PASS] " << name << " = " << got << "\n";
        testsPassed++;
    } else {
        std::cout << "[FAIL] " << name << " got " << got << " expected " << expected << "\n";
        testsFailed++;
    }
}

int main() {
    std::cout << "=== CALCULATOR DIAGNOSTIC TEST SUITE ===\n\n";
    
    std::cout << "--- Basic Arithmetic ---\n";
    test("2+3", 2+3, 5);
    test("10-4", 10-4, 6);
    test("6*7", 6*7, 42);
    test("15/3", 15.0/3.0, 5);
    test("17%5", std::fmod(17,5), 2);
    test("2^10", std::pow(2,10), 1024);
    test("5!", 120.0, 120); // factorial 5 = 120
    
    std::cout << "\n--- Trig Functions (Degrees) ---\n";
    double deg30 = 30.0 * kPi / 180.0;
    double deg45 = 45.0 * kPi / 180.0;
    double deg60 = 60.0 * kPi / 180.0;
    
    test("sin(30)", std::sin(deg30), 0.5, 1e-6);
    test("sin(45)", std::sin(deg45), 0.7071067811865476, 1e-6);
    test("cos(60)", std::cos(deg60), 0.5, 1e-6);
    test("cos(0)", std::cos(0), 1.0);
    test("tan(45)", std::tan(deg45), 1.0, 1e-6);
    
    std::cout << "\n--- Inverse Trig Functions ---\n";
    test("asin(0.5) in DEG", std::asin(0.5) * 180.0 / kPi, 30.0, 1e-6);
    test("acos(0.5) in DEG", std::acos(0.5) * 180.0 / kPi, 60.0, 1e-6);
    test("atan(1) in DEG", std::atan(1.0) * 180.0 / kPi, 45.0, 1e-6);
    
    std::cout << "\n--- Logarithm Functions ---\n";
    test("ln(e)", std::log(kE), 1.0, 1e-9);
    test("ln(10)", std::log(10), 2.302585093, 1e-6);
    test("log(100)", std::log10(100), 2.0);
    test("log(1000)", std::log10(1000), 3.0);
    
    std::cout << "\n--- Power & Root ---\n";
    test("sqrt(16)", std::sqrt(16), 4.0);
    test("sqrt(2)", std::sqrt(2), 1.414213562, 1e-6);
    test("2^3", std::pow(2,3), 8.0);
    test("10^2", std::pow(10,2), 100.0);
    
    std::cout << "\n--- Constants ---\n";
    test("pi", kPi, 3.14159265358979323846, 1e-12);
    test("e", kE, 2.71828182845904523536, 1e-12);
    
    std::cout << "\n========================================\n";
    std::cout << "--- ELECTRICAL ENGINEERING FUNCTIONS ---\n";
    std::cout << "========================================\n\n";
    
    std::cout << "--- Ohm's Law: V = I * R ---\n";
    test("vir(2,10) - 2A * 10Ohm = 20V", 2.0 * 10.0, 20.0);
    test("vir(0.5,100) - 0.5A * 100Ohm = 50V", 0.5 * 100.0, 50.0);
    
    std::cout << "\n--- Ohm's Law: I = V / R ---\n";
    test("ivr(12,4) - 12V / 4Ohm = 3A", 12.0 / 4.0, 3.0);
    test("ivr(230,100) - 230V / 100Ohm = 2.3A", 230.0 / 100.0, 2.3);
    
    std::cout << "\n--- Ohm's Law: R = V / I ---\n";
    test("rvi(12,3) - 12V / 3A = 4Ohm", 12.0 / 3.0, 4.0);
    test("rvi(240,2) - 240V / 2A = 120Ohm", 240.0 / 2.0, 120.0);
    
    std::cout << "\n--- Power: P = V * I ---\n";
    test("pvi(12,2) - 12V * 2A = 24W", 12.0 * 2.0, 24.0);
    test("pvi(230,5) - 230V * 5A = 1150W", 230.0 * 5.0, 1150.0);
    
    std::cout << "\n--- Power: P = I^2 * R ---\n";
    test("pir(2,10) - 2A^2 * 10Ohm = 40W", 2.0 * 2.0 * 10.0, 40.0);
    test("pir(5,10) - 5A^2 * 10Ohm = 250W", 5.0 * 5.0 * 10.0, 250.0);
    
    std::cout << "\n--- Power: P = V^2 / R ---\n";
    test("pvr(12,6) - 12V^2 / 6Ohm = 24W", (12.0 * 12.0) / 6.0, 24.0);
    test("pvr(120,10) - 120V^2 / 10Ohm = 1440W", (120.0 * 120.0) / 10.0, 1440.0);
    
    std::cout << "\n--- Derived: V = P / I ---\n";
    test("vpi(24,2) - 24W / 2A = 12V", 24.0 / 2.0, 12.0);
    test("vpi(1000,5) - 1000W / 5A = 200V", 1000.0 / 5.0, 200.0);
    
    std::cout << "\n--- Derived: I = P / V ---\n";
    test("ipv(24,12) - 24W / 12V = 2A", 24.0 / 12.0, 2.0);
    test("ipv(1000,200) - 1000W / 200V = 5A", 1000.0 / 200.0, 5.0);
    
    std::cout << "\n--- Derived: V = sqrt(P * R) ---\n";
    test("vpr(100,4) - sqrt(100W * 4Ohm) = 20V", std::sqrt(100.0 * 4.0), 20.0);
    test("vpr(1000,10) - sqrt(1000W * 10Ohm) = 100V", std::sqrt(1000.0 * 10.0), 100.0);
    
    std::cout << "\n--- Derived: I = sqrt(P / R) ---\n";
    test("ipr(100,4) - sqrt(100W / 4Ohm) = 5A", std::sqrt(100.0 / 4.0), 5.0);
    test("ipr(1000,10) - sqrt(1000W / 10Ohm) = 10A", std::sqrt(1000.0 / 10.0), 10.0);
    
    std::cout << "\n--- Impedance: Z = sqrt(R^2 + X^2) ---\n";
    test("zrx(3,4) - 3-4-5 triangle = 5Ohm", std::sqrt(3.0*3.0 + 4.0*4.0), 5.0);
    test("zrx(10,10) - sqrt(200) = 14.142Ohm", std::sqrt(100.0 + 100.0), 14.142135623, 1e-4);
    
    std::cout << "\n--- Power Factor: PF = cos(theta) ---\n";
    test("pf(0) - cos(0) = 1", std::cos(0), 1.0);
    test("pf(30deg) - cos(30deg) = 0.866", std::cos(30.0 * kPi / 180.0), 0.86602540378, 1e-4);
    test("pf(60deg) - cos(60deg) = 0.5", std::cos(60.0 * kPi / 180.0), 0.5, 1e-4);
    
    std::cout << "\n--- AC Power: Real Power P = V * I * cos(theta) ---\n";
    double angle30 = 30.0 * kPi / 180.0;
    test("preal(120,5,30deg) = 120*5*cos(30) = 519.6W", 120.0 * 5.0 * std::cos(angle30), 519.615, 0.1);
    test("preal(230,10,0deg) = 230*10*cos(0) = 2300W", 230.0 * 10.0 * std::cos(0), 2300.0);
    
    std::cout << "\n--- AC Power: Reactive Power Q = V * I * sin(theta) ---\n";
    test("preact(120,5,30deg) = 120*5*sin(30) = 300VAR", 120.0 * 5.0 * std::sin(angle30), 300.0);
    test("preact(230,10,90deg) = 230*10*sin(90) = 2300VAR", 230.0 * 10.0 * std::sin(kPi/2), 2300.0);
    
    std::cout << "\n--- AC Power: Apparent Power S = V * I ---\n";
    test("papp(120,5) = 600VA", 120.0 * 5.0, 600.0);
    test("papp(230,10) = 2300VA", 230.0 * 10.0, 2300.0);
    
    std::cout << "\n--- Capacitive Reactance: Xc = 1/(2*pi*f*C) ---\n";
    test("xc(1000,0.000001) - 1kHz,1uF = 159.15Ohm", 1.0 / (2.0 * kPi * 1000.0 * 0.000001), 159.1549, 0.1);
    test("xc(50,0.0001) - 50Hz,100uF = 31.83Ohm", 1.0 / (2.0 * kPi * 50.0 * 0.0001), 31.8309, 0.1);
    
    std::cout << "\n--- Inductive Reactance: Xl = 2*pi*f*L ---\n";
    test("xl(1000,0.001) - 1kHz,1mH = 6.283Ohm", 2.0 * kPi * 1000.0 * 0.001, 6.28318, 0.01);
    test("xl(50,0.1) - 50Hz,100mH = 31.416Ohm", 2.0 * kPi * 50.0 * 0.1, 31.4159, 0.1);
    
    std::cout << "\n--- Resonant Frequency: f0 = 1/(2*pi*sqrt(L*C)) ---\n";
    test("fres(0.001,0.000001) - 1mH,1uF = 5033Hz", 1.0 / (2.0 * kPi * std::sqrt(0.001 * 0.000001)), 5032.92, 1.0);
    test("fres(0.01,0.00001) - 10mH,10uF = 503Hz", 1.0 / (2.0 * kPi * std::sqrt(0.01 * 0.00001)), 503.29, 0.1);
    
    std::cout << "\n--- Decibels (Voltage): dB = 20*log10(V1/V2) ---\n";
    test("dbv(10,1) = 20*log10(10) = 20dB", 20.0 * std::log10(10.0 / 1.0), 20.0);
    test("dbv(100,1) = 20*log10(100) = 40dB", 20.0 * std::log10(100.0 / 1.0), 40.0);
    test("dbv(2,1) = 20*log10(2) = 6.02dB", 20.0 * std::log10(2.0 / 1.0), 6.0206, 0.01);
    
    std::cout << "\n--- Decibels (Power): dB = 10*log10(P1/P2) ---\n";
    test("dbp(100,1) = 10*log10(100) = 20dB", 10.0 * std::log10(100.0 / 1.0), 20.0);
    test("dbp(1000,1) = 10*log10(1000) = 30dB", 10.0 * std::log10(1000.0 / 1.0), 30.0);
    test("dbp(2,1) = 10*log10(2) = 3.01dB", 10.0 * std::log10(2.0 / 1.0), 3.0103, 0.01);
    
    std::cout << "\n--- Voltage Divider: Vout = Vin * R2 / (R1 + R2) ---\n";
    test("vdiv(12,1000,1000) - 12V, equal resistors = 6V", 12.0 * 1000.0 / (1000.0 + 1000.0), 6.0);
    test("vdiv(12,3000,1000) - 12V, 3:1 ratio = 3V", 12.0 * 1000.0 / (3000.0 + 1000.0), 3.0);
    test("vdiv(10,8000,2000) - 10V, 4:1 ratio = 2V", 10.0 * 2000.0 / (8000.0 + 2000.0), 2.0);
    
    std::cout << "\n--- Utility Functions ---\n";
    test("abs(-5) = 5", std::fabs(-5.0), 5.0);
    test("abs(5) = 5", std::fabs(5.0), 5.0);
    test("pow(2,3) = 8", std::pow(2.0, 3.0), 8.0);
    test("pow(10,5) = 100000", std::pow(10.0, 5.0), 100000.0);
    
    std::cout << "\n========================================\n";
    std::cout << "TEST RESULTS SUMMARY\n";
    std::cout << "========================================\n";
    std::cout << "Tests PASSED: " << testsPassed << "\n";
    std::cout << "Tests FAILED: " << testsFailed << "\n";
    std::cout << "Total tests: " << (testsPassed + testsFailed) << "\n";
    
    if (testsFailed == 0) {
        std::cout << "\n*** ALL TESTS PASSED - CALCULATOR FUNCTIONS CORRECTLY ***\n";
        return 0;
    } else {
        std::cout << "\n*** SOME TESTS FAILED - REVIEW RESULTS ABOVE ***\n";
        return 1;
    }
}