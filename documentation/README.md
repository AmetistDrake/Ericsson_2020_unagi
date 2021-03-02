# Projekt struktúra

**FONTOS!!!**
```
Mivel a projekt a 2020-as Ericcson Bajnokságra készült, ahol a szervezők biztosították a tcp servert és a szerver oldali programot, így a projekt használhatóságához szimulálni kell a server működését. Ehhez a `nodejs-server` szolgál eszközül, ami tcp hálózati csatlakozással működik. Ez azonban csak az első fordulóhoz készült el, így a második forduló megváltoztatott protokolljával sajnos már nem kompatibilis, és így ez a projekt jelenleg nem futtatható!
```

## Kliens-szerver kapcsolat:
- A versenyen lehetőség volt a kódot feltöltve az Ericsson szerverén futtatni, és hálózati csatlakozással tcp protokollon keresztül is. Mi a projektünket úgy szerveztük, hogy mindkét módot egyszerűen lehessen megvalósítani, és ehhez szolgál eszközül a `socket-connector` mappa tartalma, valamint a `Reader.h` és `Reader.cpp` fájlokban megvalósított `readDataConsole()` és `readDataInfo(const vector<string>& infos)` függvények. 

- **Futtatás a szerverre feltöltéssel:**
    - Ekkor mindössze a projekt főkönyvtárában lévő `.cpp` és `.h` fájlokat szükséges feltölteni a szerverre:
        - `Reader.cpp`
        - `Reader.h`
        - `Solver.cpp`
        - `Solver.h`
        - `solver1_move_vaccine.cpp`
        - `solver2_healing.cpp`
        - `solver3_cleaned_back.cpp`
        - `solver4_infection.cpp`
        - `solver5_vaccine_production.cpp`
        - `solver_export_json.cpp`
    - A futtatáskor a szerver a standard i/o-n keresztül kommunikál a programmal a Solver osztályon keresztül, és a `readDataConsole()` fv hívódik meg a `Solver::process(const vector<string> &infos)` fv elején.

- **Futtatás tcp csatlakozással hálózaton keresztül**:
    - Ekkor a saját gépünkön a `socket-connector` mappában a `main.cpp`-t kell futtatni, és így a `readDataInfo(const vector<string>& infos)` fv fog meghívódni, ami a `main.cpp`-ben egy bufferből feltöltött `infos` vektorból olvassa ki a feladat megoldásához szükséges adatokat. 

## `nodejs-server`
```
Megjegyzés: Ez jelenleg már nem releváns, mert a verseny második fordulója során a szerver funkciója is bővült, de a mi verziónk már nem, ezért a cpp kód sem futtatható már.
```
Mivel a szervert az Ericsson biztosította, és a szerver oldali kódot mi nem láttuk, csak a viselkedését ismertük, ahhoz, hogy az Ericsson szervere nélkül is futtatható legyen a kód, szimulálnunk kellett azt. Ezt az első fordulóban megtettük, mert szükségünk volt rá a teszteléshez, és az Ericsson csapata csak az első forduló vége felé indította be a szerverét. A saját szerverünkhöz írt kód a `nodejs-server` mappában található.

Ismertető a szerver működéséről:
- a server nodejs alapú, így szükség van npm-re
- a mappában cmd-ről futtassuk az `npm install` parancsot
- majd az npn `start app.js` paranccsal indítsuk el a servert
- A tcp csatlakozás kliens oldali megvalósítása a socket-connector mappában történik, és itt található a main.cpp is, ami az entry point. Ennek a kódnak az alján meg kell adni a server ip címét/domain nevét, és a portot. 

## `socket-connector`
- a `main` fv:
```
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {...}
```
Ez csak azért nem egyszerűen `int main() {...}`, mert kísérleteztem a `windows.h` könyvtárat használni display készítésre. Ezt végül elvetettem, mert szükségtelenül bonyolult volt, de magamnak emlékeztetőül a kódban hagytam (git historyban valahol megtalálható egy félkész c++ nyelven írt display).

- a `client` class:

A server timeout előre definiált értéke:
```
std::chrono::duration<double> process_timeout_s;
```    
 
 A tcp csatlakozásért felelős objektum pointere (ennek az implementációja a `socket-connector` mappában megtalálható többi fájlban van):
```
std::unique_ptr<connector> _connector;
```

A server válasza ebbe kerül bele feldolgozásra:
```
std::string remained_buffer;
```

A feladat megoldását megalkotó osztály (ennek a helyes megvalósítása a verseny célja): 
```
Solver your_solver;
```

- A `main` fv a `client` class konstruktorával létrehozza a csatlakozást, majd elküldi a servernek a kezdeti üzenetet. Ezt követően meghívható a `client.run()` fv, ami a server csatlakozás befejeztéig irányítja a kommunikációt.
```
void run()
```
Ez a fv paraméterként megkapja az üzenetet, és elküldi a servernek:
```
void send_messages(const std::vector<std::string>& messages)
```

Ez a fv beolvassa a server üzenetét, és egy `vektor<string>`-ként adja vissza:
```
std::vector<std::string> receive_message()
```

## `displays`

Ebben a mappában egy böngésző alapú display található, ami lehetővé tette, hogy vizualizáljuk az egyes futtatásokat. A Display.html fájl be tudja tallózni a `solver_export_json.cpp` által generált `.json` fájlokat, és megjeleníteni minden kör beli információt.

## `Főkönyvtár forrásfájljai`

Ezekben oldottuk meg a feladot:

- `Solver`: Ennek a `process` fv-e fut le minden körben, ahonnan meghívódik minden egyéb részprogram.

- `Reader`: Itt olvassuk be az adott körre az adatokat, és eltároljuk a célszerű adatszerkezetekben.

- `solver1-5`: A megoldás menetét 5 szakaszra bontottuk, és ezeket könnyebb áttekinthetőség végett külön fájlokba kiemeltük. 

- `solver_export_json.cpp`: Ez a program készíti elő a `.json` fájlt beolvasásra a display számára