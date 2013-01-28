#!/usr/bin/env python
import string, urllib, sys

if len(sys.argv) > 1:
	sport = sys.argv[1]
else:
	sport = "NHL" # Must be one of NHL, NBA, MLB

class SportsParser:
	def __init__(self):
		print "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>"
		print "<rdf:RDF"
		print "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\""
		print "xmlns=\"http://my.netscape.com/rdf/simple/0.9/\">"
		print "<channel>"
		print "<title>%s</title>" %self.title()
		print "<description>%s</description>" %self.description()
		print "<link>%s</link>" %self.link()
		print "</channel>"
		self.processScores(self.inputFile())
		print "</rdf:RDF>"
	
	def processScores(self, inputFile):
		try:
			input = urllib.urlopen(self.inputFile())
			lines = input.readlines()
		except:
			sys.exit(5)

		for line in lines:
			if self.isScoreLine(line):
				self.handleScoreLine(line)
	
	def handleGame(self, headline, link):
		print "<item>"
		print "<title>%s</title>" %headline
		print "<link>%s</link>" %link
		print "</item>"
	
class NHLParser(SportsParser):
	def isScoreLine(self, line):
		if line.startswith("nhl_scores_current[") or line.startswith("nhl_scores_previous["):
			return 1
		else:
			return 0
	
	def handleScoreLine(self, line):
		__line = string.strip(line)
		__line = string.split(__line, "|")
		__date = __line[1]
		__dayOfWeek = __line[3]
		__teamA = __line[4]
		__teamB = __line[5]
		__teamAAbbr = __line[7]
		__teamBAbbr = __line[8]
		__gameState = __line[9]
		__hour = int(__line[10])
		__minutes = int(__line[11])
		__timeZone = __line[12]
		__scoreA = __line[13]
		__scoreB = __line[15]
		if __gameState == "pre-game":
			self.handleGame("%s at %s (%i:%02i %s)" %(__teamAAbbr, __teamBAbbr, __hour, __minutes, __timeZone), self.link())
		elif __gameState == "Final":
			self.handleGame("%s %s %s %s (Final)" %(__teamBAbbr, __scoreB, __teamAAbbr, __scoreA), self.link())
		# I'm tired of wget'ing the HTML file just to wait for an in-progress
		# entry. Moo.
	
	def inputFile(self):
		return "http://sports.excite.com/nhl/index.html"
	
	def title(self):
		return "NHL Scores"
	
	def description(self):
		return "Official scores from the National Hockey League"
	
	def link(self):
		return "http://sports.excite.com/nhl"

class NBAParser(SportsParser):
	def isScoreLine(self, line):
		if line.startswith("nba_scores_current[") or line.startswith("nba_scores_previous["):
			return 1
		else:
			return 0

	def period(self, period):
		if period > 5:
			return "%sOT" %(period - 4)
		elif period == 5:
			return "OT"
		elif period == 4:
			return "4th"
		elif period == 3:
			return "3rd"
		elif period == 2:
			return "2nd"
		elif period == 1:
			return "1st"

	def handleScoreLine(self, line):
		__line = string.strip(line)
		__line = string.split(__line, "|")

		date = __line[1]
		dayOfWeek = __line[3]
		homeTeam = __line[4]
		awayTeam = __line[5]
		homeTeamAbbrev = __line[7]
		awayTeamAbbrev = __line[8]
		hourUpdated = int(__line[10])
		minuteUpdated = int(__line[11])
		timeZoneUpdated = __line[12]
		homeTeamScore = __line[13]
		try:
			period = int(__line[14])
		except:
			period = 0
		awayTeamScore = __line[15]
		minutesRemaining = __line[16]
		secondsRemaining = __line[17]

		headline = ""
		if __line[9].count("pre") > 0:
			# game hasn't started yet, print time it starts
			headline = "%s at %s (%s:%02i %s)" %(awayTeamAbbrev, homeTeamAbbrev, hourUpdated, minuteUpdated, timeZoneUpdated)
		elif __line[9].count("inal") > 0:
			# game over
			headline = "%s %s %s %s" %(awayTeamAbbrev, awayTeamScore, homeTeamAbbrev, homeTeamScore)
			if period > 4:
				headline += " (%s)" %self.period(period)
			else:
				headline += " (Final)"
		else:
			# game in progress
			minutesRemaining = int(minutesRemaining)
			secondsRemaining = int(secondsRemaining)
			headline += " %s %s %s %s" %(awayTeamAbbrev, awayTeamScore, homeTeamAbbrev, homeTeamScore)
			headline += " (%i:%02i %s)" %(minutesRemaining, secondsRemaining, self.period(period))

		self.handleGame(headline, self.link())
	
	def inputFile(self):
		return "http://sports.excite.com/nba/index.html"
	
	def title(self):
		return "NBA Scores"
	
	def description(self):
		return "Official scores from the National Basketball Association"
	
	def link(self):
		return "http://sports.excite.com/nba"

class MLBParser(SportsParser):
	def isScoreLine(self, line):
		if line.startswith("mlb_scores_current[") or line.startswith("mlb_scores_previous["):
			return 1
		else:
			return 0

	def inning(self, inning):
		# I wonder if there's some nice i18n-friendly way to do this
		if inning > 3:
			return "%sth" %(inning)
		elif inning == 3:
			return "3rd"
		elif inning == 2:
			return "2nd"
		elif inning == 1:
			return "1st"

	def handleScoreLine(self, line):
		__line = string.strip(line)
		__line = string.split(__line, "|")

		date = __line[1]
		dayOfWeek = __line[3]
		homeTeam = __line[4]
		awayTeam = __line[5]
		homeTeamAbbrev = __line[7]
		awayTeamAbbrev = __line[8]
		try:
			hourUpdated = int(__line[10])
		except:
			hourUpdated = 0
		try:
			minuteUpdated = int(__line[11])
		except:
			minuteUpdated = 0
		timeZoneUpdated = __line[12]
		homeTeamScore = __line[13]
		try:
			inning = int(__line[14])
		except:
			inning = 0
		awayTeamScore = __line[15]
		minutesRemaining = __line[16]
		secondsRemaining = __line[17]

		headline = ""
		if __line[9].count("pre") > 0:
			# game hasn't started yet, print time it starts
			headline = "%s at %s (%s:%02i %s)" %(awayTeamAbbrev, homeTeamAbbrev, hourUpdated, minuteUpdated, timeZoneUpdated)
		elif __line[9].count("inal") > 0:
			# game over
			headline = "%s %s %s %s" %(awayTeamAbbrev, awayTeamScore, homeTeamAbbrev, homeTeamScore)
			if inning > 4:
				headline += " (%s)" %self.inning(inning)
			else:
				headline += " (Final)"
		else:
			# Game in progress
			# TODO: find out if it's the top or bottom of the inning
			# Need to wget while a game's in progress to see.
			headline += " %s %s %s %s" %(awayTeamAbbrev, awayTeamScore, homeTeamAbbrev, homeTeamScore)
			headline += " (%s)" %(self.inning(inning))

		self.handleGame(headline, self.link())
	
	def inputFile(self):
		return "http://sports.excite.com/mlb/index.html"
	
	def title(self):
		return "MLB Scores"
	
	def description(self):
		return "Official scores from Major League Baseball"
	
	def link(self):
		return "http://sports.excite.com/mlb"

try:
	parser = globals()["%sParser" %(sport)]()

except:
	print "Invalid sport type '%s' selected." %sport
	sys.exit(2)

sys.exit(0)
