################################################################
#
# kppp ruleset for Italy
#
# by Giovanni Venturi (gventuri73@tiscali.it)
#
# Tele2_Alti_ISP.rst
#
# URBANE.
#
# Ultimo aggiornamento: 26 Giugno 2003.
#
################################################################
##                                                            ##
## Addebito alla risposta di 0,0619 EUR.                      ##
##                                                            ##
## Tutti i prezzi si intendono in EUR/min IVA COMPRESA.       ##
##                                                            ##
##                                                            ##
##              Tariffa RIDOTTA: 0,017 EUR                    ##
##              Tariffa INTERA : 0,026 EUR                    ##
##                                                            ##
##                  | Ridotta  dalle 18:30 alle 08:00         ##
## Giorni FERIALI : |                                         ##
##                  | Intera   dalle 08:00 alle 18:30         ##
##                                                            ##
##                                                            ##
## Giorni FESTIVI : | Ridotta   dalle 00:00 alle 24:00        ##
##       e SABATO : |                                         ##
##                                                            ##
################################################################


# Nome del ruleset
name=Tele2_Altri_ISP

# Impostazioni della valuta
currency_symbol=EUR
currency_position=right
currency_digits=2

# Addebito alla risposta
per_connection=0.0619

# Giorni feriali, sabato e domenica a tariffa ridotta
default=(0.017, 60)

# Giorni feriali a tariffa intera
on (monday..friday) between (08:00..18:30) use (0.026, 60)

# Giorni festivi a tariffa ridotta
on (01/01) between () use (0.017, 60)
on (01/06) between () use (0.017, 60)
on (04/25) between () use (0.017, 60)
on (05/01) between () use (0.017, 60)
on (08/15) between () use (0.017, 60)
on (11/01) between () use (0.017, 60)
on (12/08) between () use (0.017, 60)
on (12/25) between () use (0.017, 60)
on (12/26) between () use (0.017, 60)
on (easter) between () use (0.017, 60)
on (easter + 1) between () use (0.017, 60)

# Fine
