################################################################
#
# kppp ruleset for Italy
#
# by Luca Boni (me9139@mclink.it)
#
# Internet_InWind.rst
#
# Chiamate dirette ad un POP InWind. 
#
# Ultimo aggiornamento: 7 Marzo 2002.
# 
################################################################
##							      ##
## Nessun addebito alla risposta.		  	      ##
##							      ##
## Tutti i prezzi si intendono in EUR/min IVA COMPRESA.       ##
##							      ##
## Tariffa RIDOTTA:  fino a 3 min = 0,0106   oltre = 0,0085   ##
## Tariffa INTERA:   fino a 3 min = 0,0205   oltre = 0,0164   ##
##							      ##
##			| intera  dalle 09:00 alle 19:00      ##
## Giorni FERIALI:	| ridotta dalle 19:00 alle 09:00      ##
##							      ##
## Giorni FESTIVI:	|  ridotta tutto il giorno	      ##
## 	e  SABATO:	|				      ##
##							      ##
################################################################

# Nome del ruleset
name=Wind_Family_Internet_InWind

# Impostazioni della valuta
currency_symbol=EUR
currency_position=right 
currency_digits=2

# Nessun addebito alla risposta
per_connection=0

# Tariffa ridotta fino a 3 min
default=(0.0106, 60)

# Giorni feriali
on (monday..friday) between (19:00..9:00) use (0.0085, 60, 180)
on (monday..friday) between (9:00..19:00) use (0.0205, 60)
on (monday..friday) between (9:00..19:00) use (0.0164, 60, 180)

# Giorni festivi, sabato e domenica oltre 3 minuti
on (saturday) between () use (0.0085, 60, 180)
on (sunday) between () use (0.0085, 60, 180)
on (01/01) between () use (0.0085, 60, 180)
on (01/06) between () use (0.0085, 60, 180)
on (04/25) between () use (0.0085, 60, 180)
on (05/01) between () use (0.0085, 60, 180)
on (08/15) between () use (0.0085, 60, 180)
on (11/01) between () use (0.0085, 60, 180)
on (12/08) between () use (0.0085, 60, 180)
on (12/25) between () use (0.0085, 60, 180)
on (12/26) between () use (0.0085, 60, 180)
on (easter) between () use (0.0085, 60, 180)
on (easter + 1) between () use (0.0085, 60, 180)

# Fine
