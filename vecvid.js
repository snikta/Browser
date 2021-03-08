var FRAME_WIDTH = 1920
var FRAME_HEIGHT = 800
var canvasEl = createElement("canvas")
var canvasWidth = FRAME_WIDTH
var canvasHeight = FRAME_HEIGHT
canvasEl.width = "800px"
canvasEl.height = "400px"
canvasEl.style.display = "block"
canvasEl.style.position = "absolute"

DrawRectangle(canvasEl, 0, 0, 1, 1)

function handleChroma() {
	Log("Chroma length: " + length(content) + " bytes")
	var i = 0
	for (i = 0; i <= 12; i=i+1) {
		Log("Byte #" + (i + 1) + ": " + getByteAtPos(content, i))
	}
	
	var byteLen = length(content)
	var bitIndex = 0
	var byteIndex = 0
	
	var pos = 0
	var frameSizes = []
	var frameCount = getByteAtPos(content, pos) << 8
	frameCount = frameCount | getByteAtPos(content, pos + 1) & 255
	pos = 2
	Log("frameCount: " + frameCount)
	
	function getBit() {
		if (bitIndex == 8) {
			bitIndex = 0
			byteIndex = byteIndex + 1
		}
		if (byteIndex < byteLen) {
			var bit = 0
			var curByte = getByteAtPos(content, byteIndex)
			if ((curByte & (1 << (7 - bitIndex))) != 0) {
				bit = 1
			}
			bitIndex = bitIndex + 1
			return bit
		}
	}
	
	function getByte() {
		return getBit() << 7 | getBit() << 6 | getBit() << 5 | getBit() << 4 | getBit() << 3 | getBit() << 2 | getBit() << 1 | getBit()
	}
	
	function FrameSize(bytes,bits) {
		var that = this
		that.bytes = bytes
		that.bits = bits
	}
	
	var byteIdx = 2
	var i = 0
	for (i = 0; i <= frameCount - 1; i=i+1) {
		var frameBytes = getByteAtPos(content, pos) << 16 | getByteAtPos(content, pos+1) << 8 | getByteAtPos(content, pos+2) & 255
		var frameBits = getByteAtPos(content, pos+3) & 255
		var frameSize = new FrameSize(byteIdx, frameBits)
		byteIdx = byteIdx + frameBytes
		frameSizes[i] = frameSize
		pos = pos + 4
	}
	
	Log("pos: " + pos)

	var byteIdx = 2
	var i = 0
	var frameIndex = 0
	
	var preBytes = pos
	var compSize = 0
	
	function playFrame() {
		if (frameIndex>0) {
			byteIndex = preBytes - 2 + frameSizes[frameIndex].bytes
			bitIndex = frameSizes[frameIndex].bits
		}
		else {
			byteIndex = pos
			bitIndex = 0
		}

		var colorCount = getByte() << 8
		colorCount = colorCount | getByte()
		
		Log("colorCount: " + colorCount)

		var mSmallDeltas = {
			0: {x: -1, y: -1},
			1: {x: -1, y: 0},
			2: {x: -1, y: 1},
			3: {x: 0, y: -1},
			4: {x: 0, y: 1},
			5: {x: 1, y: -1},
			6: {x: 1, y: 0},
			7: {x: 1, y: 1}
		}

		var shapes = []
		var colors = []

		var frameKnots = []
		var colorIndex = 0
		for (colorIndex = 0; colorIndex < 15; colorIndex=colorIndex+1) {
			var bit = getBit()
			if (bit == 0) {
				frameIndex = frameIndex + 1
				playFrame()
				return
			}
			var red = getByte()
			var green = getByte()
			var blue = getByte()

			var contourCount = (getBit() << 11 | getBit() << 10 | getBit() << 9 | getBit() << 8 | getBit() << 7 | getBit() << 6 | getBit() << 5 | getBit() << 4 | getBit() << 3 | getBit() << 2 | getBit() << 1 | getBit())						
			
			compSize = compSize + 6

			var knots = []
			var contourIndex = 0
			for (contourIndex = 0; contourIndex < contourCount; contourIndex=contourIndex+1) {
				var firstPointX = (getBit() << 11 | getBit() << 10 | getBit() << 9 | getBit() << 8 | getBit() << 7 | getBit() << 6 | getBit() << 5 | getBit() << 4 | getBit() << 3 | getBit() << 2 | getBit() << 1 | getBit())
				var firstPointY = (getBit() << 11 | getBit() << 10 | getBit() << 9 | getBit() << 8 | getBit() << 7 | getBit() << 6 | getBit() << 5 | getBit() << 4 | getBit() << 3 | getBit() << 2 | getBit() << 1 | getBit())
				var innerContourCount = 0
				
				compSize = compSize + 2
				
				if (getBit() == 1) {
					if (getBit() == 1) {
						innerContourCount = (getBit() << 11 | getBit() << 10 | getBit() << 9 | getBit() << 8 | getBit() << 7 | getBit() << 6 | getBit() << 5 | getBit() << 4 | getBit() << 3 | getBit() << 2 | getBit() << 1 | getBit())
					}
					else {
						innerContourCount = getByte()
					}
				} else {
					innerContourCount = 1
				}

				var curPoint = {x: firstPointX, y: firstPointY}
				var curDelta = {x: 0, y: 0}

				var knotLen = length(knots)
				knots[knotLen] = {x: curPoint.x / 2.5, y: curPoint.y / 2.5}

				var innerContourIndex = 1
				for (innerContourIndex = 1; innerContourIndex < innerContourCount; innerContourIndex=innerContourIndex+1) {
					if (getBit() == 1) {
						var x = 0
						var y = 0
						if (getBit() == 1) {
							var sign = 1
							if (getBit() == 1) {
								sign = -1
							}
							x = (sign) * (getBit() << 11 | getBit() << 10 | getBit() << 9 | getBit() << 8 | getBit() << 7 | getBit() << 6 | getBit() << 5 | getBit() << 4 | getBit() << 3 | getBit() << 2 | getBit() << 1 | getBit())
							sign = 1
							if (getBit() == 1) {
								sign = -1
							}
							y = (sign) * (getBit() << 11 | getBit() << 10 | getBit() << 9 | getBit() << 8 | getBit() << 7 | getBit() << 6 | getBit() << 5 | getBit() << 4 | getBit() << 3 | getBit() << 2 | getBit() << 1 | getBit())
						} else {
							var deltaIndex = getBit() << 2 | getBit() << 1 | getBit()
							var delta = mSmallDeltas[deltaIndex]
							x = delta.x
							y = delta.y
						}
						curDelta = {x: x, y: y}
					}
					
					curPoint.x = curPoint.x + curDelta.x
					curPoint.y = curPoint.y + curDelta.y
						
					var knotLen = length(knots)
					knots[knotLen] = {x: curPoint.x / 2.5, y: curPoint.y / 2.5}
				}
			}
			
			DrawPolyline(canvasEl, knots)
		}
		
		frameIndex = frameIndex + 1
	}
	playFrame()
}

fetch("http://localhost:8000/chroma.txt", handleChroma)
