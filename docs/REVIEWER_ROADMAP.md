# Roadmap Operativa Per La Review

Questa roadmap descrive i prossimi passi pratici per portare `cnumkit` a una review tecnica forte. Non e una roadmap di prodotto ampia: il focus e rendere la repo verificabile, coerente e credibile come libreria C scientifica.

## Fase 1: Build E Test Riproducibili

Obiettivo: chiunque cloni la repo deve poter costruire e testare il progetto senza interpretazioni.

- Installare o rendere disponibile CMake nell'ambiente di sviluppo principale.
- Verificare `cmake -S . -B build`, `cmake --build build` e `ctest --test-dir build --output-on-failure`.
- Testare almeno build Debug, Release, static library e shared library.
- Verificare `CNUMKIT_ENABLE_CONTRACTS=ON` e `CNUMKIT_ENABLE_SANITIZERS=ON` quando il compilatore li supporta.

Criterio di completamento: CMake e CTest passano localmente e il risultato e riportato nella documentazione di stato.

## Fase 2: Policy API Ed Errori

Obiettivo: ogni funzione pubblica deve avere comportamento prevedibile su successo e fallimento.

- Documentare per ogni API ownership, parametri validi, return value ed error code.
- Rendere coerente il comportamento su `NULL`, dimensioni zero, mismatch, NaN/Inf e output pointer nullo.
- Evitare che funzioni di sola lettura sovrascrivano l'errore globale in modo sorprendente, se non falliscono.
- Decidere e documentare se i setter devono accettare o rifiutare valori non finiti.

Criterio di completamento: header e guide descrivono lo stesso contratto che i test verificano.

## Fase 3: Test Numerici E Negativi

Obiettivo: aumentare fiducia senza gonfiare inutilmente il framework.

- Aggiungere test per output pointer nulli in tutti i moduli.
- Aggiungere test NaN/Inf per setter, ottimizzazione e ML.
- Aggiungere casi near-singular e ill-conditioned per il solver.
- Aggiungere test su dimensioni non quadrate e mismatch in `linalg`.
- Dichiarare tolleranze per ogni suite numerica.

Criterio di completamento: i test coprono sia il percorso felice sia i principali failure mode documentati.

## Fase 4: Documentazione E Doxygen

Obiettivo: la documentazione deve spiegare la libreria senza promettere funzioni non presenti.

- Generare Doxygen e correggere eventuali warning.
- Tenere README breve e orientato a build, test, sicurezza e primo esempio.
- Tenere `docs/LIBRARY_ARCHITECTURE.md` come spiegazione interna della libreria.
- Mantenere `docs/PROJECT_STATUS.md` aggiornato quando cambia la base verificata.

Criterio di completamento: Doxygen genera output senza warning critici e i documenti principali sono coerenti fra loro.

## Fase 5: RISC-V Scalar Readiness

Obiettivo: dimostrare che il core scalare puo essere compilato e testato per RISC-V.

- Verificare la toolchain `riscv64-linux-gnu-gcc`.
- Verificare `qemu-riscv64` come emulatore per smoke test.
- Eseguire build cross con `cmake/toolchains/riscv64-linux-gnu.cmake`.
- Eseguire almeno i test core sotto QEMU prima di qualunque lavoro RVV.

Criterio di completamento: build scalare RISC-V e smoke test passano e sono documentati in `docs/RISCV.md`.

## Fase 6: Benchmark Scalare E Preparazione RVV

Obiettivo: preparare l'ottimizzazione misurando prima il comportamento scalare.

- Aggiungere benchmark minimi per dot product, norma, matrix multiply e solver.
- Stabilire input piccoli, medi e grandi per misure ripetibili.
- Definire un meccanismo futuro per backend opzionali senza cambiare API pubbliche.
- Tenere RVV dietro opzione CMake o feature detection, con fallback scalare.

Criterio di completamento: esiste una baseline scalare misurabile prima di introdurre codice RVV.

## Fase 7: Packaging E Release Readiness

Obiettivo: rendere la libreria consumabile da un progetto esterno.

- Verificare installazione CMake con `install()` ed export `cnumkit::cnumkit`.
- Creare un piccolo progetto consumer esterno che usa `find_package(cnumkit REQUIRED)`.
- Verificare header installati, libreria statica/shared e symbol export.
- Preparare changelog pubblico e criteri minimi per tag `v0.1.0`.

Criterio di completamento: un progetto esterno puo includere e linkare `cnumkit` usando solo l'installazione CMake.
