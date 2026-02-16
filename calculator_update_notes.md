k all the functio# Scientific Calculator - Update Notes

## Current Status
Working build at `calculator_gui.exe`. Compiles with:
```
C:/mingw64/bin/g++.exe -g -std=c++17 -mwindows -municode calculator.cpp -o calculator_gui.exe
```

## What's Been Done

### Core Behaviour Fixes
- **New-input-after-equals**: After pressing `=`, typing a number/function starts fresh; typing an operator (+, -, *, /, ^, %) chains with the result
- **Auto-close parentheses**: Pressing `=` auto-closes any unmatched `(` — so `sin(45` evaluates as `sin(45)`. Works for ALL functions
- **Enter key**: Works from the edit box (subclassed)
- **Keyboard input**: Respects just-evaluated state (digits clear, operators chain)

### Inverse Trig Fix
- `asin`, `acos`, `atan` now return degrees when in DEG mode (e.g. `asin(0.5)` = 30° in DEG)

### Implicit Multiplication
- `2pi`, `2(3+1)`, `5sin(30)` etc. automatically insert `*`

### Button Layout (6 columns x 10 rows)

**Row 1 - Trig Functions (Light Blue):**
sin, cos, tan, asin, acos, atan

**Row 2 - Math Functions (Light Blue):**
sqrt, ln, log, x^2, 10^x, +/-

**Row 3-6 - Number Pad (Gray) & Operators (Orange):**
Standard calculator layout with 0-9, . , operators (+, -, *, /, ^, %, !), parentheses, pi, e, Ans

**Row 7 - Ohm's Law & Power (Green):**
- `P=VI` → pvi(V,I) - Power from voltage and current
- `P=I²R` → pir(I,R) - Power from current squared times resistance
- `P=V²/R` → pvr(V,R) - Power from voltage squared over resistance
- `V=IR` → vir(I,R) - Voltage from current times resistance
- `I=V/R` → ivr(V,R) - Current from voltage over resistance
- `R=V/I` → rvi(V,I) - Resistance from voltage over current

**Row 8 - Derived Power (Green):**
- `V=P/I` → vpi(P,I) - Voltage from power over current
- `I=P/V` → ipv(P,V) - Current from power over voltage
- `V=√PR` → vpr(P,R) - Voltage from sqrt(power * resistance)
- `I=√P/R` → ipr(P,R) - Current from sqrt(power / resistance)
- `Z=√(R²+X²)` → zrx(R,X) - Impedance magnitude
- `PF` → pf(θ) - Power factor (cos of phase angle)

**Row 9 - AC Power & Impedance (Cyan):**
- `Preal` → preal(V,I,θ) - Real power (W) - 3 args
- `Preact` → preact(V,I,θ) - Reactive power (VAR) - 3 args
- `Papp` → papp(V,I) - Apparent power (VA)
- `Xc` → xc(f,C) - Capacitive reactance
- `Xl` → xl(f,L) - Inductive reactance
- `fres` → fres(L,C) - Resonant frequency

**Row 10 - Decibels & Utilities (Purple):**
- `dB(V)` → dbv(V1,V2) - Voltage gain in dB (20*log10)
- `dB(P)` → dbp(P1,P2) - Power gain in dB (10*log10)
- `Vdiv` → vdiv(Vin,R1,R2) - Voltage divider output
- `abs` → abs(x) - Absolute value
- `pow` → pow(x,y) - x to the power of y
- `min` → min(a,b) - Minimum of two values

### Top Bar
RAD/DEG | MS | MR | MC | M+ | M- | Back | Clear | =

## Color Coding
- **Blue** = Evaluate button
- **Red** = Clear/Back
- **Purple** = RAD/DEG toggle
- **Teal** = Memory functions
- **Orange** = Operators
- **Light Blue** = Scientific functions
- **Green** = Ohm's Law & Power calculations
- **Cyan** = AC Power & Impedance
- **Purple** = Decibels & utilities

## FAQ

### Q: Do I have to close brackets for sin(, cos(, etc.?
**No.** The auto-close feature handles it. When you press `=`, any unmatched opening parentheses are automatically closed before evaluation.

### Q: How do I use the EE functions?
Simply click the button and enter values separated by commas:
- `pvi(12,2)` = 24 (12V × 2A = 24W)
- `vir(2,10)` = 20 (2A × 10Ω = 20V)
- `preal(120,5,30)` = 519.6W (in DEG mode)
- `xc(1000,0.000001)` = 159.15Ω (1kHz, 1µF)

### Q: What's the difference between Preal, Preact, and Papp?
- **Papp(V,I)** = Apparent power in VA (just V × I)
- **Preal(V,I,θ)** = Real power in Watts (V × I × cos(θ))
- **Preact(V,I,θ)** = Reactive power in VAR (V × I × sin(θ))

The angle θ respects the current RAD/DEG mode.

## All Available Functions

### Standard Math
`sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sqrt`, `ln`, `log`, `abs`, `pow`, `min`, `max`, `!` (factorial)

### Constants
`pi`, `e`, `ans` (last result), `mem` (memory value)

### Electrical Engineering (NEW!)
| Function | Args | Description |
|----------|------|-------------|
| pvi(V,I) | 2 | Power = V × I |
| pir(I,R) | 2 | Power = I² × R |
| pvr(V,R) | 2 | Power = V² / R |
| vir(I,R) | 2 | Voltage = I × R |
| ivr(V,R) | 2 | Current = V / R |
| rvi(V,I) | 2 | Resistance = V / I |
| vpi(P,I) | 2 | Voltage = P / I |
| ipv(P,V) | 2 | Current = P / V |
| vpr(P,R) | 2 | Voltage = √(P × R) |
| ipr(P,R) | 2 | Current = √(P / R) |
| zrx(R,X) | 2 | Impedance = √(R² + X²) |
| pf(θ) | 1 | Power factor = cos(θ) |
| preal(V,I,θ) | 3 | Real power (W) |
| preact(V,I,θ) | 3 | Reactive power (VAR) |
| papp(V,I) | 2 | Apparent power (VA) |
| xc(f,C) | 2 | Capacitive reactance |
| xl(f,L) | 2 | Inductive reactance |
| fres(L,C) | 2 | Resonant frequency |
| dbv(V1,V2) | 2 | Voltage gain (dB) |
| dbp(P1,P2) | 2 | Power gain (dB) |
| vdiv(Vin,R1,R2) | 3 | Voltage divider |

## Completed ✓
All electrical engineering functions have been implemented and tested!
