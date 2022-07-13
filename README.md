# Oximeter for the Raspberry pi PICO

ETA-dagen projekt 2022. Mäter din puls och visar den på ett par 7segmentsdisplayer

## 7 Segment
Använder [PIO](https://www.raspberrypi.com/news/what-is-pio/) för att [multiplexa](https://embedded-lab.com/blog/lab-11-multiplexing-seven-segment-led-displays/) displayerna.
PIO matas kontinuerligt av DMA med minimal interaktion från processorn. 

## Hur mäter man puls
Kort svar. Du mäter hur blodet rör sig i dina ådror. Syrerikt och syrfattigt blod absorberar och reflekterar olika mycket ljus.
Vi mäter hur mycket ljus som absorberar av bloded och får reda på om det finns mycket eller lite syre i blodet. 
Ett hjärtslag består av att först trycka ut syrerikt blod och sedan suga tillbaka syrefattigt blod. Det innenbär att ett slag innebär en ökning och sedan en miskning av mängden syre i blodet. Koden filtrerar först "hur mycket syre finns det i blodet"-signalen för att minska bruset. Efter det mäter den tiden mellan maximum av signalen. Detta är periodtiden på dina hjärtslag 1/period ger frekvens.


## Filter?
Vi använder ett 2a ordningens digitalt butterwurth filter. Ni kommer lära er mer om dessa snart (tm) i tex Kretsanalys. 

## Hur hittar vi maximum?
Vi letar efter toppar i signalen som omringas av en dal före och en dal efter. Dessa dalar måste vara "djupa" i förhållande till signalen för att vi ska räkna toppen som en topp. 

## Hur kommer man igång?
https://github.com/ndabas/pico-setup-windows
Ladda ner och kör, detta installerar vscode och toolchain för att bygga projektet och programmera din PICO.

## Vilken sektion?
E!

## Vilken färg?
GUL!

## Vilket skyddshelgon?
KALLE ANKA!

