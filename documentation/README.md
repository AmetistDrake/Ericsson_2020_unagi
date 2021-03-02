# Projekt struktúra

- Mivel a projekt a 2020-as Ericcson Bajnokságra készült, ahol a szervezők biztosították a tcp servert és a szerver oldali programot, így a projekt használhatóságához szimulálni kell a server működését. Ehhez a `nodejs-server` mappa tartalma szolgál eszközül.

## `nodejs-server`
- a server nodejs alapú, így szükség van npm-re
- a mappában cmd-ről futtassuk az `npm install` parancsot
- majd az npn `start app.js` paranccsal indítsuk el a servert