# Scientific Calculator with EE, Calculus & Graphing

A Windows GUI scientific calculator built in C++ using the native Win32 API. Features a dark-themed interface with colour-coded buttons, a built-in function plotter, full electrical engineering calculations, calculus functions, and a standard scientific calculator.

---

## Requirements to Run

### To run the pre-built executable (`calculator_v8.exe`)

- **Operating System**: Windows 10 or Windows 11 (64-bit)
- **No installation required** — the `.exe` is fully self-contained
- **No external DLLs or libraries needed** — uses only the Windows Win32 API built into every copy of Windows
- Simply double-click `calculator_v8.exe` from File Explorer

> **Note**: Must be launched from the Windows desktop environment (not via WSL or a headless terminal) for the GUI and graphing to display correctly.

### To recompile from source (`calculator.cpp`)

| Requirement | Details |
|-------------|---------|
| Compiler | MinGW-w64 g++ (C++17 or later) |
| Download | https://www.mingw-w64.org/ or via MSYS2 |
| Standard Library | C++17 STL (`<cmath>`, `<map>`, `<sstream>`, `<vector>`, `<functional>`) |
| Windows SDK | Included with MinGW — provides `<windows.h>` |
| Build command | `g++ -std=c++17 -mwindows -municode calculator.cpp -o calculator_v8.exe` |

No CMake, no Visual Studio, no extra packages — one file, one command.

---

## What It Can Do — Full Capabilities

### Standard Calculator
- All basic arithmetic: `+`, `−`, `×`, `÷`, modulo `%`
- Exponentiation: `^` (e.g. `2^10` = 1024)
- Factorial: `!` (up to 170!)
- Negative numbers, decimal input, parentheses (auto-closed on `=`)
- Implicit multiplication: `2pi`, `3(4+1)`, `5sin(30)` all work
- Memory: store, recall, clear, add to, subtract from
- `Ans` — reuse last result in the next expression
- `pi` and `e` as built-in constants
- 15-digit precision output

### Scientific Functions
- **Trigonometry**: sin, cos, tan, asin, acos, atan — fully respects RAD/DEG mode
- **Logarithms**: natural log (ln), base-10 log (log), 10^x
- **Roots & powers**: sqrt, x², pow(x,y)
- **Utilities**: abs (absolute value), min, max

### Electrical Engineering — DC Power & Ohm's Law
Calculate any variable in the power triangle (P, V, I, R) given any two known values:
- **P=VI** — Power from voltage and current
- **P=I²R** — Power from current and resistance
- **P=V²/R** — Power from voltage and resistance
- **V=IR** — Voltage from current and resistance
- **I=V/R** — Current from voltage and resistance
- **R=V/I** — Resistance from voltage and current
- **V=P/I**, **I=P/V** — Derived from power
- **V=√(PR)**, **I=√(P/R)** — Square-root derived forms
- **Z=√(R²+X²)** — Impedance magnitude from resistance and reactance
- **PF=cos(θ)** — Power factor from phase angle

### Electrical Engineering — AC Power
- **Real power (W)**: Preal(V, I, θ) = V × I × cos(θ)
- **Reactive power (VAR)**: Preact(V, I, θ) = V × I × sin(θ)
- **Apparent power (VA)**: Papp(V, I) = V × I
- All angle inputs respect the current RAD/DEG mode setting

### Electrical Engineering — Reactance & Resonance
- **Capacitive reactance**: Xc(f, C) = 1 / (2πfC) — enter frequency in Hz, capacitance in Farads
- **Inductive reactance**: Xl(f, L) = 2πfL — enter frequency in Hz, inductance in Henries
- **Resonant frequency**: fres(L, C) = 1 / (2π√(LC)) — LC tank circuit resonance in Hz

### Electrical Engineering — Decibels & Signal
- **Voltage gain dB**: dbv(V1, V2) = 20 × log₁₀(V1/V2)
- **Power gain dB**: dbp(P1, P2) = 10 × log₁₀(P1/P2)
- **Voltage divider**: vdiv(Vin, R1, R2) = Vin × R2 / (R1 + R2)

### Calculus — Summations
- **Σ(n)**: Sum of integers 1 to n — formula: n(n+1)/2
- **Σ(n²)**: Sum of squares 1² to n² — formula: n(n+1)(2n+1)/6
- **Σ(n³)**: Sum of cubes 1³ to n³ — formula: (n(n+1)/2)²
- **Geometric series**: geom(a, r, n) = a(1−rⁿ⁺¹)/(1−r)

### Calculus — Definite Integrals (exact analytic results)
- **∫xⁿ dx** from a to b: intpow(a, b, k) — handles k=−1 (gives ln result)
- **∫eˣ dx** from a to b: intexp(a, b)
- **∫sin x dx** from a to b: intsin(a, b)
- **∫cos x dx** from a to b: intcos(a, b)
- **∫(1/x) dx** from a to b: intlog(a, b)

### Calculus — Numerical Derivatives
Uses the central difference method: (f(x+h) − f(x−h)) / (2h) for high accuracy:
- **d/dx(xⁿ)** at x: derivpow(x, n, h)
- **d/dx(eˣ)** at x: derivexp(x, h)
- **d/dx(sin x)** at x: derivsin(x, h)
- **d/dx(cos x)** at x: derivcos(x, h)
- **d/dx(ln x)** at x: derivln(x, h)
- Recommended step size h = 0.000001 (use the `h=1e-6` button)

### Limits
- **limpow(x0, n, dir)**: evaluates lim(x→x0) of xⁿ from the right (dir=+1) or left (dir=−1)

### Function Graphing
- Plot any expression involving `x` (e.g. `sin(x)`, `x^2`, `e^(-abs(x))*sin(x)`)
- Auto-scales Y axis to fit the function with 10% padding
- Zoom in / zoom out controls (20%/25% per click)
- 10×10 grid with visible X and Y axes
- 18 one-click preset graphs across three categories:
  - **Basic**: sin, cos, tan, x², x³, √x
  - **More**: eˣ, ln(x), 1/x, |x|, sin(2x), cos(2x)
  - **Wave**: damped sine, sin+cos, sin²x, cos²x, sinc (sin(x)/x), x·sin(x)
- Current expression label displayed on graph panel

---

## Building

Requires MinGW-w64 (g++ with C++17 support):

```bash
C:/mingw64/bin/g++.exe -g -std=c++17 -mwindows -municode calculator.cpp -o calculator_v8.exe
```

---

## Running

Double-click `calculator_v8.exe` from File Explorer. Must be run from the Windows desktop environment for full GUI/graphics support.

---

## Interface Overview

The calculator window is divided into two panels:

- **Left panel** — expression input, control buttons, and the full function button grid
- **Right panel** — the graphing area (280×630 pixels) with Plot / Clear / Zoom+ / Zoom- controls

### Top Bar Controls

| Button | Function |
|--------|----------|
| RAD/DEG | Toggle between Radians and Degrees mode |
| MS | Memory Store — saves current answer |
| MR | Memory Recall — inserts stored value |
| MC | Memory Clear — resets memory to 0 |
| M+ | Adds current answer to memory |
| M- | Subtracts current answer from memory |
| Back | Deletes last character |
| Clear | Clears the input field |
| = | Evaluates the expression |

**Enter key** also evaluates from the keyboard.

### Status Bar

The bar at the bottom shows:
- Current angle mode (`Mode: RAD` / `Mode: DEG`)
- Help text with an example when a function button is clicked (e.g. `EX: pvi(12,2) = 24W`)
- **Click the status bar** to copy the shown example directly into the input field

---

## Input Behaviour

- **After `=`**: typing a digit or function starts a new expression; typing an operator (`+`, `-`, `*`, `/`, `^`, `%`) chains with the previous result
- **Auto-close parentheses**: pressing `=` automatically closes any unmatched `(` — so `sin(45` evaluates as `sin(45)`
- **Implicit multiplication**: `2pi`, `2(3+1)`, `5sin(30)` automatically insert `*`
- **`+/-` button**: negates the current value or expression
- **`Ans`**: inserts the last calculated result into the expression

---

## Button Colour Guide

| Colour | Category |
|--------|----------|
| Blue | `=` Evaluate |
| Red | Clear / Back |
| Purple | RAD/DEG toggle |
| Teal | Memory functions (MS, MR, MC, M+, M-) |
| Orange | Arithmetic operators (+, -, *, /, ^, %, !) |
| Light Blue | Scientific functions (trig, sqrt, log, ln) |
| Green | Ohm's Law & DC power calculations |
| Cyan | AC power & impedance (Preal, Xc, Xl, fres) |
| Purple | Decibels & utilities (dB, Vdiv, abs, pow) |
| Amber | Calculus summations (Σ) |
| Red/Pink | Calculus integrals (∫) |
| Indigo | Calculus derivatives (d/dx) |
| Bright Green | Graphing presets — basic functions |
| Teal | Graphing presets — more functions |
| Magenta | Graphing presets — wave functions |

---

## Operators & Constants

| Symbol | Description |
|--------|-------------|
| `+` `-` `*` `/` | Basic arithmetic |
| `^` | Power / exponentiation |
| `%` | Modulo |
| `!` | Factorial (integer ≥ 0, max 170) |
| `pi` | π = 3.14159265358979… |
| `e` | Euler's number = 2.71828182845904… |
| `ans` | Last evaluated result |
| `mem` | Current memory value |

---

## Standard Scientific Functions

| Function | Args | Description | Example |
|----------|------|-------------|---------|
| `sin(x)` | 1 | Sine (respects RAD/DEG mode) | `sin(45)` → 0.7071 (DEG) |
| `cos(x)` | 1 | Cosine | `cos(60)` → 0.5 (DEG) |
| `tan(x)` | 1 | Tangent | `tan(45)` → 1 (DEG) |
| `asin(x)` | 1 | Inverse sine, domain [-1,1] | `asin(0.5)` → 30 (DEG) |
| `acos(x)` | 1 | Inverse cosine, domain [-1,1] | `acos(0.5)` → 60 (DEG) |
| `atan(x)` | 1 | Inverse tangent | `atan(1)` → 45 (DEG) |
| `sqrt(x)` | 1 | Square root, domain x≥0 | `sqrt(16)` → 4 |
| `ln(x)` | 1 | Natural logarithm, domain x>0 | `ln(e)` → 1 |
| `log(x)` | 1 | Base-10 logarithm, domain x>0 | `log(100)` → 2 |
| `abs(x)` | 1 | Absolute value | `abs(-5)` → 5 |
| `pow(x,y)` | 2 | x raised to power y | `pow(2,3)` → 8 |
| `min(a,b)` | 2 | Minimum of two values | `min(3,7)` → 3 |
| `max(a,b)` | 2 | Maximum of two values | `max(3,7)` → 7 |
| `x^2` | — | Appends `^2` to expression | `5^2` → 25 |
| `10^x` | — | Appends `10^(` | `10^(2)` → 100 |

---

## Electrical Engineering Functions

All EE functions take comma-separated arguments. Click any button to see an example in the status bar, then click the status bar to copy it into the input.

### DC Power & Ohm's Law (Green buttons)

| Button | Function | Args | Formula | Example |
|--------|----------|------|---------|---------|
| P=VI | `pvi(V,I)` | V, I | P = V × I | `pvi(12,2)` → 24 W |
| P=I²R | `pir(I,R)` | I, R | P = I² × R | `pir(2,10)` → 40 W |
| P=V²/R | `pvr(V,R)` | V, R | P = V² / R | `pvr(12,6)` → 24 W |
| V=IR | `vir(I,R)` | I, R | V = I × R | `vir(2,10)` → 20 V |
| I=V/R | `ivr(V,R)` | V, R | I = V / R | `ivr(12,4)` → 3 A |
| R=V/I | `rvi(V,I)` | V, I | R = V / I | `rvi(12,3)` → 4 Ω |

### Derived Power Calculations (Green buttons)

| Button | Function | Args | Formula | Example |
|--------|----------|------|---------|---------|
| V=P/I | `vpi(P,I)` | P, I | V = P / I | `vpi(24,2)` → 12 V |
| I=P/V | `ipv(P,V)` | P, V | I = P / V | `ipv(24,12)` → 2 A |
| V=√PR | `vpr(P,R)` | P, R | V = √(P × R) | `vpr(100,4)` → 20 V |
| I=√P/R | `ipr(P,R)` | P, R | I = √(P / R) | `ipr(100,4)` → 5 A |
| Z=√(R²+X²) | `zrx(R,X)` | R, X | Z = √(R² + X²) | `zrx(3,4)` → 5 Ω |
| PF | `pf(θ)` | θ | PF = cos(θ) | `pf(30)` → 0.866 (DEG) |

### AC Power (Cyan buttons)

| Button | Function | Args | Formula | Example |
|--------|----------|------|---------|---------|
| Preal | `preal(V,I,θ)` | V, I, θ | P = V × I × cos(θ) | `preal(120,5,30)` → 519.6 W (DEG) |
| Preact | `preact(V,I,θ)` | V, I, θ | Q = V × I × sin(θ) | `preact(120,5,30)` → 300 VAR (DEG) |
| Papp | `papp(V,I)` | V, I | S = V × I | `papp(120,5)` → 600 VA |

> The angle θ respects the current RAD/DEG mode.

### Reactance & Resonance (Cyan buttons)

| Button | Function | Args | Formula | Example |
|--------|----------|------|---------|---------|
| Xc | `xc(f,C)` | f (Hz), C (F) | Xc = 1 / (2πfC) | `xc(1000,0.000001)` → 159.15 Ω |
| Xl | `xl(f,L)` | f (Hz), L (H) | Xl = 2πfL | `xl(1000,0.001)` → 6.28 Ω |
| fres | `fres(L,C)` | L (H), C (F) | f₀ = 1 / (2π√(LC)) | `fres(0.001,0.000001)` → 5033 Hz |

### Decibels & Voltage Divider (Purple buttons)

| Button | Function | Args | Formula | Example |
|--------|----------|------|---------|---------|
| dB(V) | `dbv(V1,V2)` | V1, V2 | 20 × log₁₀(V1/V2) | `dbv(10,1)` → 20 dB |
| dB(P) | `dbp(P1,P2)` | P1, P2 | 10 × log₁₀(P1/P2) | `dbp(100,1)` → 20 dB |
| Vdiv | `vdiv(Vin,R1,R2)` | Vin, R1, R2 | Vout = Vin × R2 / (R1+R2) | `vdiv(12,1000,1000)` → 6 V |

---

## Calculus Functions

### Summations (Amber buttons)

| Button | Function | Args | Formula | Example |
|--------|----------|------|---------|---------|
| Σ(n) | `sum(n)` | n | 1+2+…+n = n(n+1)/2 | `sum(10)` → 55 |
| Σ(n²) | `sum2(n)` | n | 1²+2²+…+n² = n(n+1)(2n+1)/6 | `sum2(3)` → 14 |
| Σ(n³) | `sum3(n)` | n | 1³+2³+…+n³ = (n(n+1)/2)² | `sum3(3)` → 36 |
| geom | `geom(a,r,n)` | a, r, n | a(1−rⁿ⁺¹)/(1−r) | `geom(1,2,3)` → 15 |

### Definite Integrals (Red/Pink buttons)

All integrals use exact analytic formulas, not numerical approximation.

| Button | Function | Args | Formula | Example |
|--------|----------|------|---------|---------|
| ∫x^n | `intpow(a,b,k)` | a, b, k | ∫ₐᵇ xᵏ dx = (bᵏ⁺¹−aᵏ⁺¹)/(k+1) | `intpow(0,2,3)` → 4 |
| ∫e^x | `intexp(a,b)` | a, b | ∫ₐᵇ eˣ dx = eᵇ − eᵃ | `intexp(0,1)` → 1.718 |
| ∫sin | `intsin(a,b)` | a, b | ∫ₐᵇ sin x dx = −cos(b)+cos(a) | `intsin(0,pi)` → 2 |
| ∫cos | `intcos(a,b)` | a, b | ∫ₐᵇ cos x dx = sin(b)−sin(a) | `intcos(0,pi)` → 0 |
| ∫1/x | `intlog(a,b)` | a, b | ∫ₐᵇ (1/x) dx = ln(b)−ln(a) | `intlog(1,e)` → 1 |
| lim | `limpow(x0,n,dir)` | x0, n, dir | lim x→x0 of xⁿ (dir: +1 right, −1 left) | `limpow(0,2,1)` → 0 |

> For `intpow` with k=−1, the formula automatically switches to ln(x).

### Numerical Derivatives (Indigo buttons)

All derivatives use the **central difference method**: (f(x+h) − f(x−h)) / (2h). Use `h=1e-6` (0.000001) for best accuracy.

| Button | Function | Args | Approximates | Example |
|--------|----------|------|-------------|---------|
| d/dx x^n | `derivpow(x,n,h)` | x, n, h | d/dx(xⁿ) at x | `derivpow(2,3,0.000001)` → 12 |
| d/dx e^x | `derivexp(x,h)` | x, h | d/dx(eˣ) at x | `derivexp(1,0.000001)` → 2.718 |
| d/dx sin | `derivsin(x,h)` | x, h | d/dx(sin x) at x | `derivsin(0,0.000001)` → 1 |
| d/dx cos | `derivcos(x,h)` | x, h | d/dx(cos x) at x | `derivcos(0,0.000001)` → 0 |
| d/dx ln | `derivln(x,h)` | x, h | d/dx(ln x) at x | `derivln(2,0.000001)` → 0.5 |
| h=1e-6 | — | — | Inserts `0.000001` | Use as the h argument |

---

## Graphing

The graph panel on the right side plots any function of `x`.

### How to Plot

1. Type a function expression using `x` as the variable (e.g. `sin(x)`, `x^2`, `e^(-abs(x))*sin(x)`)
2. Click **Plot** — the Y axis auto-scales to fit the function
3. Use **Zoom+** / **Zoom-** to adjust the view
4. Click **Clear** to remove the graph

### Preset Graph Buttons

Click any preset to load the expression into the input field, then click **Plot**:

**Basic (Bright Green):** `y=sin(x)`, `y=cos(x)`, `y=tan(x)`, `y=x²`, `y=x³`, `y=√x`

**More Functions (Teal):** `y=e^x`, `y=ln(x)`, `y=1/x`, `y=|x|`, `y=sin(2x)`, `y=cos(2x)`

**Wave Functions (Magenta):** `y=e^(−|x|)sin(x)`, `y=sin+cos`, `y=sin²x`, `y=cos²x`, `y=sin(x)/x`, `y=x·sin(x)`

### Graph Features
- Dark background with a 10×10 grid
- X and Y axes drawn in lighter grey
- Function plotted in bright green
- Axis range labels shown at corners
- Current expression label shown top-left of graph panel
- Auto Y-scaling on Plot (10% padding added)
- Default X range: −10 to +10
- Zoom in/out adjusts both X and Y ranges by 20%/25% per click

---

## Error Handling

| Error | Cause |
|-------|-------|
| `asin domain [-1,1]` | Input to asin/acos outside valid range |
| `sqrt domain x>=0` | Negative input to sqrt |
| `ln domain x>0` / `log domain x>0` | Zero or negative input to logarithm |
| `division by zero` | Denominator evaluates to zero |
| `modulo by zero` | Modulo by zero |
| `factorial needs integer >= 0` | Non-integer or negative factorial |
| `factorial too large (>170)` | Factorial argument exceeds 170 |
| `fres/xc args must be > 0` | Invalid frequency/component values |
| `vdiv R1+R2 cannot be 0` | Both resistors zero in voltage divider |
| `invalid expression or domain` | General parse or evaluation error |

---

## Architecture

The calculator is a single self-contained C++ file (`calculator.cpp`) using the Windows Win32 API only — no external libraries required.

| Component | Description |
|-----------|-------------|
| `ExpressionEngine` | Core evaluator: tokeniser → implicit multiply insertion → Shunting-yard (RPN) → stack evaluator |
| `kButtons[]` | Button definitions (label + insert text) for all 96 function buttons |
| `getFunctionHelp()` | Returns status bar help text for each function |
| `getExampleExpression()` | Returns a ready-to-run example expression for each function |
| `buttonBgColor()` | Maps button ID to colour based on category |
| `WndProc` | Main Win32 window procedure handling all UI events |
| `EditSubclassProc` | Subclassed edit control for Enter key and post-equals behaviour |
| Graph panel | `SS_OWNERDRAW` static control — drawn via `WM_DRAWITEM` using GDI lines |

### Expression Engine Details
- **Tokeniser**: handles numbers, named identifiers, operators, parentheses, commas
- **Shunting-yard algorithm**: converts infix to Reverse Polish Notation (RPN), handles operator precedence, right-associativity (`^`), unary `+`/`−`, and functions
- **Implicit multiplication**: `2pi` → `2*pi`, `5sin(30)` → `5*sin(30)`
- **Variables**: `pi`, `e`, `ans`, `mem` resolved at evaluation time

---

## File Structure

```
cc++/
├── calculator.cpp              # Full source — single file application
├── calculator.h                # Header declarations
├── calculator_update_notes.md  # Development notes and function reference
├── calculator_v8.exe           # Latest build (with graphing + EE + calculus)
├── calculator_gui.exe          # Earlier GUI build
├── calculator_ee.exe           # Electrical engineering build
├── calculator_v2.exe           # ... (earlier versions)
├── calculator_v3.exe
├── calculator_v4.exe
├── calculator_v5.exe
├── calculator_v6.exe
├── calculator_v7.exe
├── calculator_backup.cpp       # Backup of earlier version
├── calculator_gui.cpp          # Earlier GUI source
├── calculator_gui_backup.cpp   # Backup
├── calculator_fixed.cpp        # Intermediate fix version
├── calculator_dev.c            # Early C development version
├── test_calculator.cpp         # Unit test file
├── test_all_functions.cpp      # Full function test suite
├── calculator_test_examples.txt # Manual test examples
├── gui_development_guide.txt   # GUI development notes
└── c_programming_guide.txt     # C programming reference notes
```
