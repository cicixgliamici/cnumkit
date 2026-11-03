# Thermal Network: Principal Scientific Case Study

This case follows one physical problem from conservation equations to a checked
numerical result. Start here when reviewing how `cnumkit` supports a small
scientific calculation: owned matrices and vectors hold the system, Gaussian
elimination solves it, and borrowed kernels reconstruct the residual. The
application then checks heat conservation independently of that reconstruction.

The inputs are synthetic teaching data, not measured material properties.
The topology deliberately has only two unknown temperatures so every result
can be checked by hand. No new thermal API is added to the library.

## Physical Model And Units

```text
fixed TL -- gL -- [T1, source S1] -- gM -- [T2, source S2] -- gR -- fixed TR
                  qL ->                 qM ->                 qR ->
```

Each connection is a linear thermal conductance `g` in W/K. Temperatures are
reported in degrees Celsius; differences in Celsius and kelvin have the same
numerical value. Each source `S` injects heat in watts into an interior node.
Negative sources represent prescribed heat removal. Conductances must be
finite and strictly positive; boundary temperatures and sources must be finite.

The model assumes steady state, fixed boundary temperatures, constant
conductances, and no heat exchange except through the three connections and
two specified sources. It omits heat capacity, transients, radiation, and
temperature-dependent material properties. A conductance may represent `k*A/L`
for a uniform conductive connection, but geometry is not separately modeled.

Define each heat flow as positive from left to right:

```text
qL = gL * (TL - T1)
qM = gM * (T1 - T2)
qR = gR * (T2 - TR)
```

These are heat rates in watts, not heat flux densities in W/m^2. A negative
value means that heat travels in the opposite direction. Steady-state energy
conservation requires:

```text
qL + S1 - qM = 0
qM + S2 - qR = 0
```

Collecting the unknown temperatures gives `A*T = b`:

```text
A = [ gL+gM    -gM   ]    T = [ T1 ]    b = [ gL*TL + S1 ]
    [  -gM    gM+gR  ]        [ T2 ]        [ gR*TR + S2 ]
```

Coefficients have units W/K and both sides have units W, with a consistent
temperature origin. The determinant is `gL*gM + gL*gR + gM*gR`, which is
positive for admissible conductances. The physical system has a unique solution;
an extremely disparate set of conductances can still cause the numerical
solver's small-pivot check to reject it.

## Three Executable Cases

The executable runs these cases in order:

| Case | TL, TR (deg C) | gL, gM, gR (W/K) | S1, S2 (W) |
| --- | --- | --- | --- |
| Uniform reference | 30, 0 | 1, 1, 1 | 0, 0 |
| Unequal conductances with internal heating | 30, 0 | 2, 1, 3 | 4, 2 |
| Left boundary raised by 10 K | 40, 0 | 2, 1, 3 | 4, 2 |

The reference preserves the original example: three equal series resistances
split the 30 K drop equally, giving `T1=20`, `T2=10`, and all three flows `10 W`.

For the heated case, assembly gives:

```text
3*T1 - T2 = 64
-T1 + 4*T2 = 2
```

Substituting `T2=3*T1-64` into the second equation gives `11*T1=258`.
Thus `T1=258/11` and `T2=70/11`. The independently calculated flows are
`qL=144/11`, `qM=188/11`, and `qR=210/11 W`. They are not equal: the sources
add `4 W` and `2 W` along the path. The total outgoing heat exceeds incoming
heat by exactly `6 W`.

With the left boundary raised by 10 K, only `b1` changes, from 64 to 84 W.
The temperature changes satisfy `A*dT=[20,0]`, giving
`dT1=80/11 K` and `dT2=20/11 K`. The farther node responds less strongly.

| Case | T1, T2 (deg C) | qL, qM, qR (W) |
| --- | --- | --- |
| Uniform reference | 20.000000, 10.000000 | 10.000000, 10.000000, 10.000000 |
| Internal heating | 23.454545, 6.363636 | 13.090909, 17.090909, 19.090909 |
| Warmer left boundary | 30.727273, 8.181818 | 18.545455, 22.545455, 24.545455 |

## Reproduce The Calculation

From the repository root:

```bash
cmake -S . -B build/thermal -DCMAKE_BUILD_TYPE=Debug \
  -DCNUMKIT_WARNINGS_AS_ERRORS=ON
cmake --build build/thermal --config Debug --parallel
ctest --test-dir build/thermal -C Debug --output-on-failure -R 'thermal'
./build/thermal/thermal_network
```

For Visual Studio, run `build/thermal/Debug/thermal_network.exe` instead; the
CTest command selects the executable path automatically. Tests and examples
must be enabled, as they are by default. With examples disabled, `test_thermal`
still runs. With tests disabled, the standalone example can still be built.

Each case prints the temperatures and flows above, followed by the relative
residual, maximum node imbalance in watts, and signed global imbalance in
watts. Exact diagnostic digits depend on the compiler and floating-point
arithmetic; they are not fixed golden output. Any failed solve or diagnostic
calculation prints an error and exits unsuccessfully.

## What The Checks Establish

The algebraic residual is recomputed from the original, unmodified system:

```text
r = A*T - b
relative_residual = ||r||2 / (||A||F * ||T||2 + ||b||2)
```

`||A||F` is the Frobenius norm of the four coefficients. The zero-load,
zero-temperature solution is assigned a zero relative residual when the
normalizing denominator is zero. Non-finite arithmetic in assembly or
diagnostics is an error rather than an accepted residual.

Separate physical checks use the constitutive heat flows:

```text
maximum_node_imbalance = max(|qL+S1-qM|, |qM+S2-qR|)
global_imbalance = qR-qL-S1-S2
```

`tests/test_thermal.c` exercises the same assembly code as the example and
compares its output with independent analytic references:

- Equal and unequal series resistances without sources.
- The heated case and its analytic boundary sensitivity.
- Reversed boundaries, which reverse the heat flow sign.
- Equal boundaries with zero sources, including the all-zero state.
- Uniform scaling of conductances and sources by `1e-6` and `1e6`: temperatures
  stay the same, while heat rates scale with power.
- Invalid conductances, non-finite data, and assembly overflow, with output
  unchanged on failure and error state preserved through cleanup.
- Small-pivot rejection when extremely disparate conductances make the
  floating-point system numerically singular despite the physical uniqueness.

Temperature and heat-flow comparisons use the test framework's combined
absolute/relative tolerance of `1e-9`. The residual must be at most `1e-14`;
physical imbalances must be at most `1e-12 * max(1 W, largest expected |q|)`.
These are regression tolerances for the listed small cases, not a general
accuracy guarantee or a convergence study.

Conservation and a small residual check the discrete calculation. They do not
validate material properties, omitted physics, a continuum discretization, or
temperature accuracy for an ill-conditioned system. There is no experimental
validation, mesh refinement study, or performance claim in this case study.

## Reading The Code

1. `examples/thermal_network.c` selects the three scenarios and reports results.
2. `examples/thermal_model.h` declares the example's inputs and diagnostics.
3. `examples/thermal_model.c` validates physical inputs, assembles owned objects,
   solves, uses adapters and borrowed kernels for the residual, and checks heat
   balance. It publishes the result only after all stages succeed.
4. `tests/test_thermal.c` supplies analytic and failure-path evidence.
5. `src/linalg.c` and `docs/NUMERICAL_NOTES.md` explain the solver and its limits.

The helper is compiled directly into the example and its tests. It is not
installed as a public header or linked into `cnumkit`, keeping the numerical
library independent of this application model.
