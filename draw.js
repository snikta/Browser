var canvasEl = createElement("canvas")
canvasEl.width = "400px"
canvasEl.height = "400px"
canvasEl.style.display = "block"
canvasEl.style.position = "absolute"

var tempCanvasEl = createElement("canvas")
tempCanvasEl.width = "400px"
tempCanvasEl.height = "400px"
tempCanvasEl.style.display = "block"
tempCanvasEl.style.position = "absolute"

var coords = createElement("h1")
var prevX = 0
var prevY = 0
var shapes = []

function Shape(x1, y1, x2, y2) {
	var that = this
	that.x1 = x1
	that.y1 = y1
	that.x2 = x2
	that.y2 = y2
}

var canvasMouseUp = function (e) {
	removeEventListener("mousemove", canvasMouseMove)
	removeEventListener("mouseup", canvasMouseUp)
}

var canvasMouseMove = function (e) {
	coords.textContent = "X: " + pageX + ", Y: " + pageY
	DrawLine(tempCanvasEl, prevX, prevY, pageX, pageY, "true")
	prevX = pageX
	prevY = pageY
}

var canvasMouseDown = function (e) {
	prevX = pageX
	prevY = pageY
	addEventListener("mousemove", canvasMouseMove)
	addEventListener("mouseup", canvasMouseUp)
}
addEventListener("mousedown", canvasMouseDown)
