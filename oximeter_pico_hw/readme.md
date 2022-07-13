# HW sidan av projektet

## Förklaring av schema

- U1 är våran mikrokontrollermodul, som sköter allting.
- U2 är själva sensor som mäter puls och blodsyrenivåer genom att lysa lite LEDs, som styrs med transistorer Q5-Q7, och mäta hur stark är ljuset efter den går igenom cellerna i din finger. Den innehåller en fotodiod, som släpper en viss mängd ström beroende på ljusstyrkan- tillsammans med R9 bildar den en sorts variabel spänningsdelare, och resulterande analogt värde läses av Pico:n.
- U3 och U4 är 7-segmentsdisplayer, och båda innehåller LED:s för 2 siffror. Segmenter tänds genom själva pinnarna av Pico:n, och transistorer Q1-Q4 väljer vilken segment ska lysa. Resistorer R1-R8 begränsar ström som segmenter ska ta.


