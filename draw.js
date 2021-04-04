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
var origRotateHandlePos = {x: -9999, y: -9999}
var preRotateBBoxCentroid = {x: -9999, y: -9999}
var selectBoxKnots = []
var origPointUnrotated = {x: -9999, y: -9999}

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
var rotating = false
var selectedShapes = []

var drawSelectBox = function () {
	if (length(selectBoxKnots) > 0) {
		DrawPolyline(canvasEl, selectBoxKnots, {a:1,b:0,c:0,d:1,e:0,f:0})
		DrawEllipse(canvasEl, selectBoxKnots[0].x - 5, selectBoxKnots[0].y - 5, selectBoxKnots[0].x + 5, selectBoxKnots[0].y + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
		DrawEllipse(canvasEl, selectBoxKnots[1].x - 5, selectBoxKnots[1].y - 5, selectBoxKnots[1].x + 5, selectBoxKnots[1].y + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
		DrawEllipse(canvasEl, selectBoxKnots[2].x - 5, selectBoxKnots[2].y - 5, selectBoxKnots[2].x + 5, selectBoxKnots[2].y + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
		DrawEllipse(canvasEl, selectBoxKnots[3].x - 5, selectBoxKnots[3].y - 5, selectBoxKnots[3].x + 5, selectBoxKnots[3].y + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
	} else {
		DrawEllipse(canvasEl, bboxMinX, bboxMinY, bboxMaxX, bboxMaxY, 'transparent', {a:1,b:0,c:0,d:1,e:0,f:0})
		DrawEllipse(canvasEl, bboxMinX - 5, bboxMinY - 5, bboxMinX + 5, bboxMinY + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
		DrawEllipse(canvasEl, bboxMaxX - 5, bboxMinY - 5, bboxMaxX + 5, bboxMinY + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
		DrawEllipse(canvasEl, bboxMaxX - 5, bboxMaxY - 5, bboxMaxX + 5, bboxMaxY + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
		DrawEllipse(canvasEl, bboxMinX - 5, bboxMaxY - 5, bboxMinX + 5, bboxMaxY + 5, 'black', {a:1,b:0,c:0,d:1,e:0,f:0})
	}
	var knots = []
	knots[0] = {x: rotateConnectorFrom.x, y: rotateConnectorFrom.y}
	knots[1] = {x: rotateConnectorTo.x, y: rotateConnectorTo.y}
	DrawPolyline(canvasEl, knots, {a:1,b:0,c:0,d:1,e:0,f:0})
	DrawEllipse(canvasEl, rotateHandle.x - 5, rotateHandle.y - 5, rotateHandle.x + 5, rotateHandle.y + 5, 'rgb(128,128,128)', {a:1,b:0,c:0,d:1,e:0,f:0})
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
	if (rotating == false && resizing == false && dragging == false) {
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
	if (shapeType == 'Cursor' && dragging == false && resizing == false && rotating == false) {
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
		selectShapes(selectedShapes)
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
	if (rotating == true) {
		var centroid = {x: preRotateBBoxCentroid.x, y: preRotateBBoxCentroid.y}
		var cursorPoint = {x: pageX, y: pageY}
		var rotateAngle = getAngleBetweenTwoTouchingLineSegments({x: centroid.x, y: centroid.y}, {x: pageX, y: pageY}, {x: origRotateHandlePos.x, y: origRotateHandlePos.y})
		var cosAngle = cos(rotateAngle)
		var sinAngle = sin(rotateAngle)
		var rotateMatrix = {a: cosAngle, b: sinAngle, c: 0-sinAngle, d: cosAngle, e: 0, f: 0}
		rotateMatrix = multiplyMatrices(rotateMatrix, {a: 1, b: 0, c: 0, d: 1, e: centroid.x, f: centroid.y})
		var shapeCount = length(selectedShapes)
		for ( i = 0; i <= shapeCount - 1; i = i + 1) {
			var shape = selectedShapes[i]
			shape.matrix = multiplyMatrices(shape.untransformedMatrix, rotateMatrix)
		}
		selectShapes(selectedShapes)
		drawAllShapes()
		drawSelectBox()
	}
	else if (resizing == true) {
		var cosAngle = selectedShapes[0].cosAngle
		var sinAngle = selectedShapes[0].sinAngle
		var tanShearAngle = selectedShapes[0].tanShearAngle
		var scaleX = 1
		var scaleY = 1
		if (tight == 'true') {
			var cursorPoint = {x: pageX, y: pageY}
			var cursorPointUnrotated = rotatePoint({x: cursorPoint.x - anchorX, y: cursorPoint.y - anchorY}, cosAngle, 0-sinAngle, 0, 0)
			cursorPointUnrotated.y = cursorPointUnrotated.y - (tanShearAngle * cursorPointUnrotated.x)
			cursorPointUnrotated.x = cursorPointUnrotated.x + anchorX
			cursorPointUnrotated.y = cursorPointUnrotated.y + anchorY
			scaleX = (cursorPointUnrotated.x - anchorX) / (origPointUnrotated.x - anchorX)
			scaleY = (cursorPointUnrotated.y - anchorY) / (origPointUnrotated.y - anchorY)
		} else {
			scaleX = (pageX - anchorX) / (clickedX - anchorX)
			scaleY = (pageY - anchorY) / (clickedY - anchorY)
		}
		var shapeCount = length(selectedShapes)
		var cosAngle = selectedShapes[0].cosAngle
		var sinAngle = selectedShapes[0].sinAngle
		var tanShearAngle = selectedShapes[0].tanShearAngle
		var scaleMatrix = multiplyMatrices({a:1,b:0,c:0,d:1,e:0,f:0}, {a:scaleX,b:0,c:0,d:scaleY,e:0,f:0})
		scaleMatrix = multiplyMatrices(scaleMatrix, {a: 1, b: tanShearAngle, c: 0, d: 1, e: 0, f: 0})
		scaleMatrix = multiplyMatrices(scaleMatrix, {a: cosAngle, b: sinAngle, c: 0-sinAngle, d: cosAngle, e: 0, f: 0})
		scaleMatrix = multiplyMatrices(scaleMatrix, {a:1,b:0,c:0,d:1,e:anchorX,f:anchorY})
		for ( i = 0; i <= shapeCount - 1; i = i + 1) {
			var shape = selectedShapes[i]
			shape.matrix = multiplyMatrices(shape.untransformedMatrix, scaleMatrix)
		}
		
		selectShapes(selectedShapes)
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
		selectShapes(selectedShapes)
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
		rotating = false
		if (pageX >= (rotateHandle.x - 5) && pageX <= (rotateHandle.x + 5) && pageY >= (rotateHandle.y - 5) && pageY <= (rotateHandle.y + 5)) {
			var shapeCount = length(selectedShapes)
			for ( i = 0; i <= shapeCount - 1; i = i + 1) {
				var shape = selectedShapes[i]
				shape.untransformedMatrix = multiplyMatrices(shape.matrix, {a:1,b:0,c:0,d:1,e:0-bboxCentroid.x,f:0-bboxCentroid.y})
			}
			origRotateHandlePos = {x: rotateHandle.x, y: rotateHandle.y}
			preRotateBBoxCentroid = {x: bboxCentroid.x, y: bboxCentroid.y}
			resizing = false
			rotating = true
		}
		else if (pageX >= (selectBoxKnots[0].x - 5) && pageX <= (selectBoxKnots[0].x + 5) && pageY >= (selectBoxKnots[0].y - 5) && pageY <= (selectBoxKnots[0].y + 5)) {
			clickedHandle = 'nw'
			anchorHandle = 'se'
		}
		else if (pageX >= (selectBoxKnots[1].x - 5) && pageX <= (selectBoxKnots[1].x + 5) && pageY >= (selectBoxKnots[1].y - 5) && pageY <= (selectBoxKnots[1].y + 5)) {
			clickedHandle = 'ne'
			anchorHandle = 'sw'
		}
		else if (pageX >= (selectBoxKnots[2].x - 5) && pageX <= (selectBoxKnots[2].x + 5) && pageY >= (selectBoxKnots[2].y - 5) && pageY <= (selectBoxKnots[2].y + 5)) {
			clickedHandle = 'se'
			anchorHandle = 'nw'
		}
		else if (pageX >= (selectBoxKnots[3].x - 5) && pageX <= (selectBoxKnots[3].x + 5) && pageY >= (selectBoxKnots[3].y - 5) && pageY <= (selectBoxKnots[3].y + 5)) {
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
				anchorX = selectBoxKnots[2].x
				anchorY = selectBoxKnots[2].y
				clickedX = selectBoxKnots[0].x
				clickedY = selectBoxKnots[0].y
			}
			else if (clickedHandle == 'ne') {
				anchorX = selectBoxKnots[3].x
				anchorY = selectBoxKnots[3].y
				clickedX = selectBoxKnots[1].x
				clickedY = selectBoxKnots[1].y
			}
			else if (clickedHandle == 'se') {
				anchorX = selectBoxKnots[0].x
				anchorY = selectBoxKnots[0].y
				clickedX = selectBoxKnots[2].x
				clickedY = selectBoxKnots[2].y
			}
			else if (clickedHandle == 'sw') {
				anchorX = selectBoxKnots[1].x
				anchorY = selectBoxKnots[1].y
				clickedX = selectBoxKnots[3].x
				clickedY = selectBoxKnots[3].y
			}
			
			origBBoxMinX = bboxMinX
			origBBoxMaxX = bboxMaxX
			origBBoxMinY = bboxMinY
			origBBoxMaxY = bboxMaxY
			
			var cosAngle = selectedShapes[0].cosAngle
			var sinAngle = selectedShapes[0].sinAngle
			var tanShearAngle = selectedShapes[0].tanShearAngle
			origPointUnrotated = rotatePoint({x: clickedX - anchorX, y: clickedY - anchorY}, cosAngle, 0-sinAngle, 0, 0)
			origPointUnrotated.y = (0-tanShearAngle * origPointUnrotated.x) + origPointUnrotated.y
			origPointUnrotated.x = origPointUnrotated.x + anchorX
			origPointUnrotated.y = origPointUnrotated.y + anchorY
			
			var shapeCount = length(selectedShapes)
			for ( i = 0; i <= shapeCount - 1; i = i + 1) {
				var shape = selectedShapes[i]
				shape.untransformedMatrix = multiplyMatrices(shape.matrix, {a:1,b:0,c:0,d:1,e:0-anchorX,f:0-anchorY})
				if (tight == 'true') {
					shape.untransformedMatrix = multiplyMatrices(shape.untransformedMatrix, {a: shape.cosAngle, b: 0-shape.sinAngle, c: shape.sinAngle, d: shape.cosAngle, e: 0, f: 0})
					shape.untransformedMatrix = multiplyMatrices(shape.untransformedMatrix, {a: 1, b: 0-shape.tanShearAngle, c: 0, d: 1, e: 0, f: 0})
				}
			}			
		}
		addEventListener('mousemove', canvasMouseMove)
		addEventListener('mouseup', canvasMouseUp)
	}
}
addEventListener('mousedown', canvasMouseDown)
