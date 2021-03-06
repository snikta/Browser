var canvasEl = createElement("canvas")
canvasEl.width = "400px"
canvasEl.height = "400px"
canvasEl.style.display = "block"
canvasEl.style.position = "absolute"

var coords = createElement("h1")
var prevX = 0
var prevY = 0
var shapes = []
var knots = []
var shapeType = "Rectangle"

var ChooseRectangle = function (e) {
	Log("ChooseRectangle(e)")
	shapeType = "Rectangle"
}

var ChooseEllipse = function (e) {
	Log("ChooseEllipse(e)")
	shapeType = "Ellipse"
}

var ChooseLine = function (e) {
	Log("ChooseLine(e)")
	shapeType = "Line"
}

var rectButton = createElement("div")
rectButton.id = "rectButton"
rectButton.style.left = "0px"
rectButton.textContent = "Rectangle"

var ellipseButton = createElement("div")
ellipseButton.id = "ellipseButton"
ellipseButton.style.left = "50px"
ellipseButton.textContent = "Ellipse"

var lineButton = createElement("div")
lineButton.id = "ellipseButton"
lineButton.style.left = "100px"
lineButton.textContent = "Line"

addEventListener(rectButton, "mousedown", ChooseRectangle)
addEventListener(ellipseButton, "mousedown", ChooseEllipse)
addEventListener(lineButton, "mousedown", ChooseLine)

var drawAllShapes = function () {
	ClearCanvas(canvasEl)
	var shapeCount = length(shapes)
	for ( i = 0; i <= shapeCount - 1; i = i + 1) {
		var shape = shapes[i]
		if (shape.type == "Rectangle") {
			DrawRectangle(canvasEl, shape.x, shape.y, shape.x + shape.width, shape.y + shape.height)
		} else if (shape.type == "Ellipse") {
			DrawEllipse(canvasEl, shape.x, shape.y, shape.x + shape.width, shape.y + shape.height)
		} else if (shape.type == "Line") {
			var lineKnots = shape.knots
			var knotCount = length(lineKnots)
			var i = 0
			if (knotCount > 1) {
				DrawLine(canvasEl, lineKnots)
			}
		}
	}
}

var canvasMouseUp = function (e) {
	var shapeCount = length(shapes)
	if (shapeType == "Line") {
		knots = simplify(knots, 10, false)
		shapes[shapeCount] = {type:"Line",knots:knots}
	} else {
		shapes[shapeCount] = {type:shapeType,x:min(pageX,prevX),y:min(pageY,prevY),width:abs(pageX-prevX),height:abs(pageY-prevY)}
	}
	drawAllShapes()
	removeEventListener("mousemove", canvasMouseMove)
	removeEventListener("mouseup", canvasMouseUp)
}

var canvasMouseMove = function (e) {
	coords.textContent = "X: " + pageX + ", Y: " + pageY
	if (shapeType == "Rectangle") {
		drawAllShapes()
		DrawRectangle(canvasEl, prevX, prevY, pageX, pageY)
	} else if (shapeType == "Ellipse") {
		drawAllShapes()
		DrawEllipse(canvasEl, prevX, prevY, pageX, pageY)
	} else if (shapeType == "Line") {
		DrawLine(canvasEl, [{x:prevX,y:prevY},{x:pageX,y:pageY}])
		var plen = length(knots)
		knots[plen] = {x:pageX,y:pageY}
		var p = knots[plen]
		prevX = pageX
		prevY = pageY
	}
}

var canvasMouseDown = function (e) {
	prevX = pageX
	prevY = pageY
	if (shapeType == "Line") {
		knots = []
	}
	addEventListener("mousemove", canvasMouseMove)
	addEventListener("mouseup", canvasMouseUp)
}
addEventListener("mousedown", canvasMouseDown)
