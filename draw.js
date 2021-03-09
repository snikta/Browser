var canvasEl = createElement("canvas")
canvasEl.width = "800px"
canvasEl.height = "400px"
canvasEl.style.display = "block"
canvasEl.style.position = "absolute"

var coords = createElement("h1")
var prevX = 0
var prevY = 0
var shapes = []
var knots = []
var shapeType = "Rectangle"

var ChooseCursor = function (e) {
	shapeType = "Cursor"
}

var ChooseRectangle = function (e) {
	shapeType = "Rectangle"
}

var ChooseEllipse = function (e) {
	shapeType = "Ellipse"
}

var ChooseLine = function (e) {
	shapeType = "Line"
}

var cursorButton = createElement("div")
cursorButton.id = "cursorButton"
cursorButton.style.left = "0px"
cursorButton.textContent = "Cursor"

var rectButton = createElement("div")
rectButton.id = "rectButton"
rectButton.style.left = "50px"
rectButton.textContent = "Rectangle"

var ellipseButton = createElement("div")
ellipseButton.id = "ellipseButton"
ellipseButton.style.left = "100px"
ellipseButton.textContent = "Ellipse"

var lineButton = createElement("div")
lineButton.id = "ellipseButton"
lineButton.style.left = "150px"
lineButton.textContent = "Line"

addEventListener(cursorButton, "mousedown", ChooseCursor)
addEventListener(rectButton, "mousedown", ChooseRectangle)
addEventListener(ellipseButton, "mousedown", ChooseEllipse)
addEventListener(lineButton, "mousedown", ChooseLine)

var drawAllShapes = function () {
	ClearCanvas(canvasEl)
	var shapeCount = length(shapes)
	for ( i = 0; i <= shapeCount - 1; i = i + 1) {
		var shape = shapes[i]
		if (shape.type == "Rectangle") {
			DrawRectangle(canvasEl, shape.x, shape.y, shape.x + shape.width, shape.y + shape.height, "red")
			DrawText(canvasEl, "hello world", shape.x, shape.y)
		} else if (shape.type == "Ellipse") {
			DrawEllipse(canvasEl, shape.x, shape.y, shape.x + shape.width, shape.y + shape.height, "yellow")
		} else if (shape.type == "Line") {
			DrawLine(canvasEl, shape.curves)
		}
	}
}

var canvasMouseUp = function (e) {
	var shapeCount = length(shapes)
	if (shapeType == "Line") {
		curves = getSplinePrimitivesFromKnots(simplify(knots, 3, false))
		shapes[shapeCount] = {type:"Line",curves:curves}
	} else {
		shapes[shapeCount] = {type:shapeType,x:min(pageX,prevX),y:min(pageY,prevY),width:abs(pageX-prevX),height:abs(pageY-prevY)}
	}
	drawAllShapes()
	if (shapeType == "Cursor") {
		var shapeCount = length(shapes)
		var minX = min(prevX, pageX)
		var maxX = max(prevX, pageX)
		var minY = min(prevY, pageY)
		var maxY = max(prevY, pageY)
		var bboxMinX = maxX
		var bboxMaxX = minX
		var bboxMinY = maxY
		var bboxMaxY = minY
		for ( i = 0; i <= shapeCount - 1; i = i + 1) {
			var shape = shapes[i]
			if (shape.type == "Rectangle" || shape.type == "Ellipse") {
				if (shape.x >= minX && (shape.x + shape.width) <= maxX && shape.y >= minY && (shape.y + shape.height) <= maxY) {
					bboxMinX = min(bboxMinX, shape.x)
					bboxMaxX = max(bboxMaxX, shape.x + shape.width)
					bboxMinY = min(bboxMinY, shape.y)
					bboxMaxY = max(bboxMaxY, shape.y + shape.height)
				}
			}
		}
		DrawRectangle(canvasEl, bboxMinX, bboxMinY, bboxMaxX, bboxMaxY, "transparent")
	}
	removeEventListener("mousemove", canvasMouseMove)
	removeEventListener("mouseup", canvasMouseUp)
}

var canvasMouseMove = function (e) {
	coords.textContent = "X: " + pageX + ", Y: " + pageY
	if (shapeType == "Cursor") {
		drawAllShapes()
		DrawRectangle(canvasEl, prevX, prevY, pageX, pageY, "transparent")
	} else if (shapeType == "Rectangle") {
		drawAllShapes()
		DrawRectangle(canvasEl, prevX, prevY, pageX, pageY, "red")
	} else if (shapeType == "Ellipse") {
		drawAllShapes()
		DrawEllipse(canvasEl, prevX, prevY, pageX, pageY, "yellow")
	} else if (shapeType == "Line") {
		drawAllShapes()
		var plen = length(knots)
		knots[plen] = {x:pageX,y:pageY}
		DrawPolyline(canvasEl, knots)
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
