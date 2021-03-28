var canvasEl = createElement('canvas')
canvasEl.width = '800px'
canvasEl.height = '400px'
canvasEl.style.display = 'block'
canvasEl.style.position = 'absolute'

var coords = createElement('h1')
var prevX = 0
var prevY = 0
var shapes = []
var knots = []
var shapeType = 'Cursor'

var ChooseCursor = function (e) {
	shapeType = 'Cursor'
}

var ChooseRectangle = function (e) {
	shapeType = 'Rectangle'
}

var ChooseEllipse = function (e) {
	shapeType = 'Ellipse'
}

var ChooseLine = function (e) {
	shapeType = 'Line'
}

var cursorButton = createElement('div')
cursorButton.id = 'cursorButton'
cursorButton.style.left = '0px'
cursorButton.textContent = 'Cursor'

var rectButton = createElement('div')
rectButton.id = 'rectButton'
rectButton.style.left = '50px'
rectButton.textContent = 'Rectangle'

var ellipseButton = createElement('div')
ellipseButton.id = 'ellipseButton'
ellipseButton.style.left = '100px'
ellipseButton.textContent = 'Ellipse'

var lineButton = createElement('div')
lineButton.id = 'lineButton'
lineButton.style.left = '150px'
lineButton.textContent = 'Line'

addEventListener(cursorButton, 'mousedown', ChooseCursor)
addEventListener(rectButton, 'mousedown', ChooseRectangle)
addEventListener(ellipseButton, 'mousedown', ChooseEllipse)
addEventListener(lineButton, 'mousedown', ChooseLine)

var applyMatrix = function (matrix, point) {
    var x = point.x
    var y = point.y
    
	var retval = {
        x: matrix.a * x + matrix.c * y + matrix.e,
        y: matrix.b * x + matrix.d * y + matrix.f
    }
    return retval
}

var drawAllShapes = function () {
	ClearCanvas(canvasEl)
	var shapeCount = length(shapes)
	for ( i = 0; i <= shapeCount - 1; i = i + 1) {
		var shape = shapes[i]
		if (shape.type == 'Rectangle') {
			DrawRectangle(canvasEl, min(shape.x1, shape.x2), min(shape.y1, shape.y2), max(shape.x1, shape.x2), max(shape.y1, shape.y2), shape.color, shape.matrix)
			DrawText(canvasEl, 'hello world', min(shape.x1, shape.x2), min(shape.y1, shape.y2), max(shape.x1, shape.x2), max(shape.y1, shape.y2), shape.matrix)
		} else if (shape.type == 'Ellipse') {
			DrawEllipse(canvasEl, min(shape.x1, shape.x2), min(shape.y1, shape.y2), max(shape.x1, shape.x2), max(shape.y1, shape.y2), shape.color, shape.matrix)
		} else if (shape.type == 'Line') {
			DrawLine(canvasEl, shape.curves, shape.matrix)
		}
	}
}

var clickedHandle = ''
var anchorHandle = ''
var bboxMinX = 800
var bboxMaxX = 0
var bboxMinY = 432
var bboxMaxY = 0
var dragging = false
var resizing = false
var selectedShapes = []

var drawSelectBox = function () {
	DrawRectangle(canvasEl, bboxMinX, bboxMinY, bboxMaxX, bboxMaxY, 'transparent', {a:1,b:0,c:0,d:1,e:0,f:0})
	DrawRectangle(canvasEl, bboxMinX - 5, bboxMinY - 5, bboxMinX + 5, bboxMinY + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
	DrawRectangle(canvasEl, bboxMaxX - 5, bboxMinY - 5, bboxMaxX + 5, bboxMinY + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
	DrawRectangle(canvasEl, bboxMaxX - 5, bboxMaxY - 5, bboxMaxX + 5, bboxMaxY + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
	DrawRectangle(canvasEl, bboxMinX - 5, bboxMaxY - 5, bboxMinX + 5, bboxMaxY + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
}

var multiplyMatrices = function (mat1, mat2) {
	var A = mat2.a
	var B = mat2.b
	var C = mat2.c
	var D = mat2.d
	var E = mat2.e
	var F = mat2.f

	var a = mat1.a
	var b = mat1.b
	var c = mat1.c
	var d = mat1.d
	var e = mat1.e
	var f = mat1.f
		
	return {
		a: a * A + b * C,
		b: a * B + b * D,
		c: A * c + C * d,
		d: B * c + d * D,
		e: A * e + E + C * f,
		f: B * e + D * f + F
	}
}

var canvasMouseUp = function (e) {
	var shapeCount = length(shapes)
	if (resizing == false && dragging == false) {
		if (shapeType == 'Line') {
			curves = getSplinePrimitivesFromKnots(simplify(knots, 3, false))
			shapes[shapeCount] = {type:'Line',curves:curves,matrix:{a:1,b:0,c:0,d:1,e:0,f:0}}
			var knotCount = length(knots)
			var i = 0
			var minX = 800
			var maxX = 0
			var minY = 432
			var maxY = 0
			for (i = 0; i <= knotCount - 1; i=i+1) {
				var knot = knots[i]
				minX = min(minX, knot.x)
				maxX = max(maxX, knot.x)
				minY = min(minY, knot.y)
				maxY = max(maxY, knot.y)
			}
			Delete(knots)
			var shape = shapes[shapeCount]
			shape.x1 = minX
			shape.y1 = minY
			shape.x2 = maxX
			shape.y2 = maxY
		} else if (shapeType != 'Cursor') {
			shapes[shapeCount] = {
				type: shapeType,
				x1: min(pageX,prevX),
				y1: min(pageY,prevY),
				x2: max(pageX,prevX),
				y2: max(pageY,prevY),
				color: 'rgb(129, 198, 255)',
				matrix:{a:1,b:0,c:0,d:1,e:0,f:0}
			}
		}
	}
	drawAllShapes()
	if (shapeType == 'Cursor' && dragging == false && resizing == false) {
		var shapeCount = length(shapes)
		var minX = min(prevX, pageX)
		var maxX = max(prevX, pageX)
		var minY = min(prevY, pageY)
		var maxY = max(prevY, pageY)
		bboxMinX = maxX
		bboxMaxX = minX
		bboxMinY = maxY
		bboxMaxY = minY
		selectedShapes = []
		for ( i = 0; i <= shapeCount - 1; i = i + 1) {
			var shape = shapes[i]
			var startPoint = applyMatrix(shape.matrix, {x: shape.x1, y: shape.y1})
			var endPoint = applyMatrix(shape.matrix, {x: shape.x2, y: shape.y2})
			var x1 = min(startPoint.x, endPoint.x)
			var y1 = min(startPoint.y, endPoint.y)
			var x2 = max(startPoint.x, endPoint.x)
			var y2 = max(startPoint.y, endPoint.y)
			if (x1 >= minX && x2 <= maxX && y1 >= minY && y2 <= maxY) {
				bboxMinX = min(bboxMinX, x1)
				bboxMaxX = max(bboxMaxX, x2)
				bboxMinY = min(bboxMinY, y1)
				bboxMaxY = max(bboxMaxY, y2)
				selectedShapes[length(selectedShapes)] = shape
			}
			if (length(selectedShapes) == 0) {
				bboxMinX = 800
				bboxMaxX = 0
				bboxMinY = 432
				bboxMaxY = 0
			}
		}
	}
	drawSelectBox()
	dragging = false
	removeEventListener('mousemove', canvasMouseMove)
	removeEventListener('mouseup', canvasMouseUp)
}

var anchorX = 0
var anchorY = 0
var clickedX = 0
var clickedY = 0
var origBBoxMinX = 0
var origBBoxMaxX = 0
var origBBoxMinY = 0
var origBBoxMaxY = 0
var canvasMouseMove = function (e) {
	coords.textContent = 'X: ' + pageX + ', Y: ' + pageY
	if (resizing == true) {
		var scaleX = (pageX - anchorX) / (clickedX - anchorX)
		var scaleY = (pageY - anchorY) / (clickedY - anchorY)
		var shapeCount = length(selectedShapes)
		var scaleMatrix = multiplyMatrices({a:1,b:0,c:0,d:1,e:0,f:0}, {a:scaleX,b:0,c:0,d:scaleY,e:0,f:0})
		scaleMatrix = multiplyMatrices(scaleMatrix, {a:1,b:0,c:0,d:1,e:anchorX,f:anchorY})
		for ( i = 0; i <= shapeCount - 1; i = i + 1) {
			var shape = selectedShapes[i]
			shape.matrix = multiplyMatrices(shape.untransformedMatrix, scaleMatrix)
		}
		drawAllShapes()
		drawSelectBox()
		scaledBBoxMinX = (origBBoxMinX - anchorX) * scaleX + anchorX
		scaledBBoxMaxX = (origBBoxMaxX - anchorX) * scaleX + anchorX
		scaledBBoxMinY = (origBBoxMinY - anchorY) * scaleY + anchorY
		scaledBBoxMaxY = (origBBoxMaxY - anchorY) * scaleY + anchorY
		bboxMinX = min(scaledBBoxMinX, scaledBBoxMaxX)
		bboxMaxX = max(scaledBBoxMinX, scaledBBoxMaxX)
		bboxMinY = min(scaledBBoxMinY, scaledBBoxMaxY)
		bboxMaxY = max(scaledBBoxMinY, scaledBBoxMaxY)
		coords.textContent = 'X: ' + pageX + ', Y: ' + pageY + ' scale(' + scaleX + ', ' + scaleY + ')'
	}
	else if (dragging == true) {
		var deltaX = pageX - prevX
		var deltaY = pageY - prevY
		bboxMinX = bboxMinX + deltaX
		bboxMaxX = bboxMaxX + deltaX
		bboxMinY = bboxMinY + deltaY
		bboxMaxY = bboxMaxY + deltaY
		var shapeCount = length(selectedShapes)
		var translateMatrix = {a:1,b:0,c:0,d:1,e:deltaX,f:deltaY}
		for ( i = 0; i <= shapeCount - 1; i = i + 1) {
			var shape = selectedShapes[i]
			shape.matrix = multiplyMatrices(shape.matrix, translateMatrix)
		}
		drawAllShapes()
		drawSelectBox()
		prevX = pageX
		prevY = pageY
	}
	else if (shapeType == 'Cursor') {
		drawAllShapes()
		DrawRectangle(canvasEl, prevX, prevY, pageX, pageY, 'transparent', {a:1,b:0,c:0,d:1,e:0,f:0})
	} else if (shapeType == 'Rectangle') {
		drawAllShapes()
		DrawRectangle(canvasEl, prevX, prevY, pageX, pageY, 'transparent', {a:1,b:0,c:0,d:1,e:0,f:0})
	} else if (shapeType == 'Ellipse') {
		drawAllShapes()
		DrawEllipse(canvasEl, prevX, prevY, pageX, pageY, 'transparent', {a:1,b:0,c:0,d:1,e:0,f:0})
	} else if (shapeType == 'Line') {
		drawAllShapes()
		var plen = length(knots)
		knots[plen] = {x:pageX,y:pageY}
		DrawPolyline(canvasEl, knots, {a:1,b:0,c:0,d:1,e:0,f:0})
		prevX = pageX
		prevY = pageY
	}
}

var canvasMouseDown = function (e) {
	prevX = pageX
	prevY = pageY
	
	if (pageX >= 800 - 320 && pageX <= 800) {
		return
	} else {	
		resizing = true
		dragging = false
		if (pageX >= (bboxMinX - 5) && pageX <= (bboxMinX + 5) && pageY >= (bboxMinY - 5) && pageY <= (bboxMinY + 5)) {
			clickedHandle = 'nw'
			anchorHandle = 'se'
		}
		else if (pageX >= (bboxMaxX - 5) && pageX <= (bboxMaxX + 5) && pageY >= (bboxMinY - 5) && pageY <= (bboxMinY + 5)) {
			clickedHandle = 'ne'
			anchorHandle = 'sw'
		}
		else if (pageX >= (bboxMaxX - 5) && pageX <= (bboxMaxX + 5) && pageY >= (bboxMaxY - 5) && pageY <= (bboxMaxY + 5)) {
			clickedHandle = 'se'
			anchorHandle = 'nw'
		}
		else if (pageX >= (bboxMinX - 5) && pageX <= (bboxMinX + 5) && pageY >= (bboxMaxY - 5) && pageY <= (bboxMaxY + 5)) {
			clickedHandle = 'sw'
			anchorHandle = 'ne'
		} else {
			resizing = false
			if (shapeType == 'Line') {
				knots = []
			}
			else if (pageX >= bboxMinX && pageX <= bboxMaxX && pageY >= bboxMinY && pageY <= bboxMaxY) {
				dragging = true
			}
		}
		if (resizing == true) {
			if (clickedHandle == 'nw') {
				anchorX = bboxMaxX
				anchorY = bboxMaxY
				clickedX = bboxMinX
				clickedY = bboxMinY
			}
			else if (clickedHandle == 'ne') {
				anchorX = bboxMinX
				anchorY = bboxMaxY
				clickedX = bboxMaxX
				clickedY = bboxMinY
			}
			else if (clickedHandle == 'se') {
				anchorX = bboxMinX
				anchorY = bboxMinY
				clickedX = bboxMaxX
				clickedY = bboxMaxY
			}
			else if (clickedHandle == 'sw') {
				anchorX = bboxMaxX
				anchorY = bboxMinY
				clickedX = bboxMinX
				clickedY = bboxMaxY
			}
			
			origBBoxMinX = bboxMinX
			origBBoxMaxX = bboxMaxX
			origBBoxMinY = bboxMinY
			origBBoxMaxY = bboxMaxY
			
			var shapeCount = length(selectedShapes)
			for ( i = 0; i <= shapeCount - 1; i = i + 1) {
				var shape = selectedShapes[i]
				shape.untransformedMatrix = multiplyMatrices(shape.matrix, {a:1,b:0,c:0,d:1,e:0-anchorX,f:0-anchorY})
			}
		}
		addEventListener('mousemove', canvasMouseMove)
		addEventListener('mouseup', canvasMouseUp)
	}
}
addEventListener('mousedown', canvasMouseDown)
