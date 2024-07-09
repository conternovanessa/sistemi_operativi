# sistemi_operativi
## Installation instruction on UBUNTU

```
sudo apt update && sudo apt upgrade -y
sudo apt install gcc g++ cmake -y
```

## How to build it

Open a terminal on current folder with terminal and run:

```
mkdir build
cp variabili.txt build
cd build
cmake ..
make
```

## Checklist:

- [ ] processo master
    - [ ] terminatore
      - [ ] Timeout  --> raggiungimento di SIM_DURATION
      - [ ] Explode  --> FREE_ENERGY > ENERGU_EXPLODE_THRESHOLD
      - [ ] Blackout --> prelievo di quantità superiore di quella possibile da prelevare
      - [ ] meltdown  --> fallimento di una fork
    - [ ] far partire N_ATOM_INIT processi *atomo* in cui si scelga il numero atomico da 0 a MAX_N_ATOMICO
    - [ ] far partire *attivatore*
    - [ ] far partire *alimentatore*
    - [ ] STAMPA stato corrente ogni secondo
    - [ ] preleva una quantità di energia da ENERGY_DEMAND
  
- [ ] processo atomo       (mantenere privato il numero atomico di ciascun processo)
    - [ ] scissione, tenere conto di quante scissioni avvengono
    - [ ] calcolo dei nuovi numeri atomici n1 e n2
    - [ ] scorie, tenerne conto in base a quali numeri atomici sono minori o uguali a MIN_N_ATOMICO
    - [ ] aggiornamento della free_energy  N1*N2 -MAX(N1,N2)
    - [ ] la scissione viene comunicata dal processo attivatore e se ne tiene conto

- [ ] processo attivatore
    - [ ] timer di STEP_ATTIVATORE
    - [ ] comunicazione ad atomo di scindere

- [ ] processo alimentatore
    - [ ] timer di STEP_ALIMENTAZIONE nanosecondi 
    - [ ] creazione di N_NUOVI_ATOMI

