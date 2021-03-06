var canvasEl = createElement("canvas")
canvasEl.width = "400px"
canvasEl.height = "400px"
canvasEl.style.display = "block"
canvasEl.style.position = "absolute"
canvasEl.style.left = "400px"

var tempCanvasEl = createElement("canvas")
tempCanvasEl.width = "400px"
tempCanvasEl.height = "400px"
tempCanvasEl.style.display = "block"
tempCanvasEl.style.position = "absolute"

var coords = createElement("h1")
var prevX = 0
var prevY = 0
var shapes = []
var knots = []

function Shape(x1, y1, x2, y2) {
	var that = this
	that.x1 = x1
	that.y1 = y1
	that.x2 = x2
	that.y2 = y2
}

function Point(x, y) {
	var that = this
	that.x = x
	that.y = y
}

var canvasMouseUp = function (e) {
	var simplePoints = simplify(knots, 10, false)
	var i = 0
	var plen = length(simplePoints)
	ClearCanvas(tempCanvasEl)
	for ( i = 1; i <= plen - 1; i = i + 1) {
		var prevPoint = simplePoints[i-1]
		var point = simplePoints[i]
		DrawLine(tempCanvasEl, prevPoint.x, prevPoint.y, point.x, point.y, "false")
	}
	removeEventListener("mousemove", canvasMouseMove)
	removeEventListener("mouseup", canvasMouseUp)
}

var canvasMouseMove = function (e) {
	coords.textContent = "X: " + pageX + ", Y: " + pageY
	DrawLine(tempCanvasEl, prevX, prevY, pageX, pageY, "true")
	var plen = length(knots)
	knots[plen] = {x:pageX,y:pageY}
	var p = knots[plen]
	prevX = pageX
	prevY = pageY
}

var canvasMouseDown = function (e) {
	prevX = pageX
	prevY = pageY
	knots = [{x:pageX,y:pageY}]
	addEventListener("mousemove", canvasMouseMove)
	addEventListener("mouseup", canvasMouseUp)
}
addEventListener("mousedown", canvasMouseDown)
