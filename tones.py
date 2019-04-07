
'''

So in real instruments, not only does each note have a different frequency, but also a slightly different distribution of overtones.
From the graphs I can see, the overtones generally all fill the range from the base, to ~3000, becoming more pure as you get up.
Which makes sense - an 880 A can double and triple, but that's it, while a 220 A can have many multiples before it gets that high.
The bottom note is awlays the loudest, at ~95%. The other notes drop off logarithmically.
THere is also a notable amount of extra noise between each of the spikes. But, a uniform amount. Ish. Some parts have drops in them.
	https://music.stackexchange.com/questions/8214/how-can-i-replicate-the-sound-of-an-instrument

In a clarinet, actually, a graph with a log-y-axis looks like it drops off logarithmically too. So the drop is significant.
	https://newt.phys.unsw.edu.au/music/clarinet/Dsharp4.html

So..
Generate all multiples up to a certain threshhold - like 3000hz.
Have the amplitude of each subsequent spike drop logarithmically.




'''

import pyaudio
from numpy import sin, pi, arange, math, float32
import random

class Player:
	def __init__(self):
		self.volume = 0.3
		self.p = pyaudio.PyAudio()
		self.fs=44100
		self.duration = 0.1   # in seconds
		self.baseFreq = 55
		self.plucked = False
		self.tones = []

		self.stream = self.p.open(format=pyaudio.paFloat32,
						    channels=1,
						    rate=self.fs,
						    output=True)

	def waveCrossedAxis(self, wave, i):
		#return (wave[i+1] * wave[i] < 0.0) or (wave[i-1] * wave[i] < 0.0)
		return wave[i+1] > 0.00001 and wave[i] < 0.00001
		#return abs(wave[i-1]) > 0.00001 and abs(wave[i]) < 0.001

	def cleanWave(self, wave, truncate=False):
		i = len(wave) - 2
		while ( i > 1 and not self.waveCrossedAxis(wave, i)):
			#print(len(wave), i, wave[i], self.waveCrossedAxis(wave, i), wave[i+1])
			#if (abs(wave[i+1]) < -0.00001):
			#	i += 1
			#	break
			i -= 1
		if truncate:
			#print(wave[i-1], wave[i], wave[i+1])
			wave[i+1:] *=0
			return wave[1:i+2]
		else:
			wave[i:] *= 0
			return wave
		

	def shapeWaveAmplitude(self, amp, wave):
		if self.plucked:
			for i in range(0, len(wave)):
				wave[i] *= amp * (1-math.sqrt(i/len(wave)))
			
			return wave
		else:
			return amp * wave


	def getWave(self, amp, f, duration):
		amp = amp * self.volume
		basicWave = sin( 2*pi * arange(self.fs*duration) * f/self.fs)
		cleanedWave = self.cleanWave(basicWave)
		return self.shapeWaveAmplitude(amp, cleanedWave)


	def getHalfToneFreq(self, numHalfTones=0):
		return self.baseFreq * math.pow(math.pow(2, 1/12), numHalfTones) 


	def getFullWave(self, toneAttrs, mainFreq, duration):
		tones = []
		for toneAttr in toneAttrs:
			amp = toneAttr[0]
			multiplier = toneAttr[1]
			freq = mainFreq * multiplier
			tones.append( self.getWave(amp, freq, duration) )

		fullWave = self.combineOvertones(tones)
		#print("..", self.cleanWave(fullWave, True))
		cleanedWave = self.cleanWave(fullWave, True)
		#print(cleanedWave[:5], cleanedWave[-5:])
		#exit(0)
		return cleanedWave


	def combineOvertones(self, tones):
		result = tones[0] * 0
		for tone in tones:
			result += tone
		return result


	def getRandOvertones(self):
		attrs = [(0.6, 1)]
		for i in range(0, random.randint(10,12)):
			randAmp = int(random.uniform(0.01, 0.5)**2 * 100)/100.
			randFreq = random.randint(2, 12)
			if (randAmp < 0.01):
				continue
			attrs.append((randAmp, randFreq))
		print(attrs)
		return attrs


	def buildToneFromFreq(self, mainFreq, duration):
		
		attrs = [
		#	[amplitude, multiplier],
			[0.8, 1],
			[0.4, 2],
			[0.3, 3],
			[0.1, 6],
			[0.05, 8],
			[0.02, 9],
			[0.1, 2.05]
		]
		#attrs = [(0.5, 1), (0.17, 12), (0.09, 3), (0.07, 7), (0.06, 6), (0.16, 3), (0.03, 2), (0.13, 6), (0.05, 9), (0.15, 3), (0.15, 7)]
		#attrs = [(0.5, 1), (0.13, 4), (0.05, 2), (0.23, 4), (0.23, 12), (0.19, 6)]
		#attrs = [(0.6, 1), (0.02, 12), (0.21, 3), (0.16, 4), (0.16, 5), (0.01, 10), (0.02, 4), (0.02, 3)]
		
#attrs = [(0.6, 1), (0.21, 2), (0.15, 3), (0.024, 6), (0.02, 7), (0.01, 11), (0.012, 13)]
		#attrs = [(0.6, 1), (0.11, 2), (0.15, 3), (0.184, 6), (0.02, 7), (0.02, 11), (0.012, 13)]
		#attrs = [(0.9, 1), (0.11, 2), (0.55, 3), (0.3, 4), (0.45, 5), (0.1, 6), (0.35, 7), (0.1, 8)] #supposed clarinet
		#attrs = [(0.8, 1), (0.5, 3), (0.4, 5), (0.3, 7), (0.05, 9), (0.01, 11)] #supposed clarinet (better)
		#attrs = [(0.8, 1), (0.5, 3), (0.4, 5), (0.3, 7), (0.3, 8), (0.05, 9), (0.01, 11)] 
		#attrs = [(0.6, 1)]

		attrs = [
			[0.9, 1],
			[0.0, 2],
			[0.0, 3],
			[0.3, 4],
			[0.0, 5],
			[0.5, 6],
			[0.0, 7],
			[0.2, 8],
			[0.0, 9],
			[0.2, 10],
			[0.0, 11],
			[0.1, 12],
			[0.0, 13],
			[0.05, 14],
			[0.0, 15],
			[0.01, 16],

			[0.1, 2.5],
			[0.1, 3.25],
			[0.1, 3.75],
			[0.1, 5.5]
		]

		wave = self.getFullWave(attrs, mainFreq, duration)

		return wave

	def buildTone(self, halfStep, duration):
		return self.buildToneFromFreq( self.getHalfToneFreq(halfStep), duration)

	def buildTones(self, top = 24):
		tones = {}
		for i in range(0, top + 1):

			tones[i] = self.buildTone(i, 0.16)
		return tones


	def play(self):

		self.plucked = True
		self.volume = 1
		top = 32
		self.tones = self.buildTones(top)


		self.playSongs()
		#self.playTone(tones[12])


	def playSongs(self):
		notes = []
		for i in range(0, len(self.tones)):
			notes.append(i)
			notes.append(i)
			notes.append(i)
		notes.append(len(self.tones)-1)
		notes.append(len(self.tones)-1)
		notes = [0,3,2,0,2,3,3,3,3,2,2,2,2,3,7,7,7,3,2,0,2,3,3,3,3,2,2,3,2,0,0,0]
		notes = [0,2,3,0,7,5,3,0,0,3,2,-2,5,3,2,-2,-2,2,3,0,7,5,3,0,0,-4,-5,0,0,-2,0]
		#notes = [-5,0,2,3,0,-1,-1,-1,-5,0,2,3,5,7,7,7,-5,0,2,3,5,2,0,-1,2,0,3,2,-1,0,0,0]

		print(len(notes))
		print(notes)

		self.playTone(self.tones[notes[0] + 8])
		for n in notes:
			self.playTone(self.tones[n + 8])

		self.playTone(self.tones[notes[len(notes)-1] + 8])

		for n in notes:
			self.playTone(self.tones[n + 18])
		for i in range(0,3):
			self.playTone(self.tones[notes[len(notes)-1] + 18])



	def playTone(self, wave):
		samples = wave.astype(float32).tobytes()
		self.stream.write(samples)

	def destroy(self):
		self.stream.stop_stream()
		self.stream.close()
		self.p.terminate()


if __name__ == "__main__":
	p = Player()
	p.play()
	p.destroy()
	

