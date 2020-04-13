# Waschmaschinen-Display
## Vorgeschichte
Das Display unserer Bauknecht WAT UNIQ 65 AAA hat leider nach ca. 4 Jahren Betrieb angefangen zu flackern. Das Problem verschlimmerte sich soweit, dass man die eingestellten Programmdaten gar nicht mehr ablesen konnte.

## Erste Analyse
Nach dem Zerlegen der Maschine wurde klar, dass sich das Display zusammen mit 8 der Einstelltasten und ein paar LEDs auf einer einzelnen Platine befindet, die über vier Adern mit der Hauptsteuereinheit verbunden ist. Das Herz der Platine ist ein Atmel Atmega88-20AU. Das Display selbst ist wohl eine Eigenanfertigung des Herstellers und hat auf der Rückseite zwei PT6961 LED-Treiber eingegossen. Am Stecker des Displayboard sind die Kontakte VCC und GND beschriftet gewesen. Dazwischen liegen 5V Versorgungsspannung von der Steuerung an. Durch verfolgen der Leiterbahnen am Stecker bis zum Atmega stellten sich heraus, dass die Platine über I2C mit der Steuerung kommuniziert.

## I2C-Protokoll
Nach ein bisschen herumprobieren mit einem Logicanalyzer und einem Arduino zum testweisen Ansteuern des Displays hat sich das verwendete Protokoll so halbwegs offenbart. Der Atmega scheint allerdings einfach die Bits vom I2C zum Display weiterzuübertragen (und natürlich die LEDs anzusteuern und Taster abzufragen). Meine Erkenntnisse habe ich in einem PDF zusammengefasst.

## Nachbau der Platine
Anschließen habe ich vorerst auf einer Lochrasterplatine - entsprechend ihrer Position auf der Original-Leiterplatte - Taster, LEDs und ein Standart HD44780 1602 LCD aufgelötet. Angesteuert wird das ganze über einen Arduino-Nano-Klon. Der vorläufige Arduino-Sketch (Anzeige von Temperatur, Zeit und Schleuderdrehzahl) ist hier verfügbar.
