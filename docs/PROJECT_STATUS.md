# Stato Attuale Del Progetto

Questo documento fotografa dove si trova oggi `cnumkit` e cosa e gia stato implementato nella preparazione alla review.

## Obiettivo

`cnumkit` vuole essere una libreria C11 per calcolo scientifico: piccola, leggibile, robusta, testata e pronta a crescere verso backend ottimizzati. La direzione tecnica attuale e costruire prima un core scalare affidabile, poi usare quel core come riferimento per ottimizzazioni future, inclusa RISC-V RVV.

## Struttura Della Repo

- `include/`: API pubbliche e header umbrella `cnumkit.h`.
- `src/`: implementazioni dei moduli.
- `tests/`: test unitari leggeri con framework interno.
- `examples/`: esempi d'uso della libreria.
- `docs/`: guide, note architetturali e documentazione per reviewer.
- `cmake/toolchains/`: toolchain file per cross-build, inclusa RISC-V.

## Funzionalita Implementate

Il core attuale include:

- vettori dinamici con allocazione, accesso, scrittura, dot product e norma L2;
- matrici dense row-major con allocazione, identita e moltiplicazione;
- solver lineare tramite eliminazione gaussiana con pivoting parziale;
- derivata numerica e gradient descent 1D;
- regressione lineare 1D e mean squared error;
- gestione errori thread-local tramite `cnk_get_last_error()` e `cnk_get_last_error_message()`.

## Hardening Gia Fatto

La preparazione alla review ha introdotto o consolidato questi punti:

- API pubbliche con validazione runtime degli input invece di affidarsi agli `assert`.
- Contratti `CNK_REQUIRES` e `CNK_ENSURES` resi opt-in tramite `CNUMKIT_ENABLE_CONTRACTS`.
- Errori piu coerenti per puntatori nulli, dimension mismatch, matrici singolari, allocazioni fallite e risultati numerici non finiti.
- Controlli contro overflow `size_t` nelle allocazioni di vettori e matrici.
- Test negativi per input invalidi, dimensioni errate e matrici singolari.
- CMake aggiornato con registrazione CTest e opzioni per contracts e sanitizer.
- Fallback sorgente `include/cnumkit_export.h`, utile anche fuori da build CMake generate.
- Prima traccia RISC-V: toolchain file `riscv64-linux-gnu` e guida `docs/RISCV.md`.

## Stato Delle Verifiche

Verifiche eseguite localmente:

- build diretta MinGW dei test con `-Wall -Wextra -Werror -pedantic`;
- `test_basic`: 38 assert passati;
- `test_optim`: 10 assert passati;
- `test_ml`: 17 assert passati;
- `git diff --check`: nessun problema di whitespace, solo warning CRLF normali su Windows.

Limite attuale dell'ambiente:

- `cmake` non e disponibile nel `PATH` della shell corrente, quindi la registrazione CTest e stata ispezionata ma non ancora verificata eseguendo davvero `cmake --build` e `ctest`.

## Cosa Manca Ancora

Prima di una review forte restano da completare:

- verifica CMake/CTest reale su una macchina con CMake disponibile;
- policy API completa e documentata per ogni funzione pubblica;
- test piu ampi su NaN/Inf, output pointer nulli, near-singular system e casi numerici di riferimento;
- generazione Doxygen verificata;
- smoke test RISC-V via cross-build e QEMU;
- benchmark scalari minimi prima di qualsiasi ottimizzazione RVV;
- controllo install/export da un progetto consumer esterno.
