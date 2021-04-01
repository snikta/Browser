var rotateHandle = {x: -9999, y: -9999}
var rotateConnectorFrom = {x: -9999, y: -9999}
var rotateConnectorTo = {x: -9999, y: -9999}
var bboxCentroid = {x: -9999, y: -9999}
var tight = 'false'

var getMidpointOfTwoPoints = function (point1, point2) {
    return {x: (point1.x + point2.x) / 2, y: (point1.y + point2.y) / 2}
}

var rotatePoint = function (point, cosAngle, sinAngle, offsetX, offsetY) {
    var withoutOffset = {x: point.x - offsetX, y: point.y - offsetY}
    return {x: withoutOffset.x * cosAngle - withoutOffset.y * sinAngle + offsetX, y: withoutOffset.x * sinAngle + withoutOffset.y * cosAngle + offsetY}
}

var clamp = function (value, minLimit, maxLimit) {
    return min(max(value, minLimit), maxLimit)
}

var pythagSquared = function (point1, point2) {
    return pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2)
}
var pythag = function (point1, point2) {
    return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2))
}

var getAngleBetweenTwoTouchingLineSegments = function (touchPoint, line1EndPoint, line2EndPoint) {
	var line2EndPointVector = {x: line2EndPoint.x - touchPoint.x, y: line2EndPoint.y - touchPoint.y}
    var line1EndPointVector = {x: line1EndPoint.x - touchPoint.x, y: line1EndPoint.y - touchPoint.y}
    var aSq = pythagSquared(line1EndPoint, line2EndPoint)
    var b = pythag(line2EndPoint, touchPoint)
    var c = pythag(line1EndPoint, touchPoint)
    var crossProduct = line2EndPointVector.x * line1EndPointVector.y - line1EndPointVector.x * line2EndPointVector.y
	
	var sign = 1
	if (crossProduct < 0) {
		sign = -1
	}
	
    var retval = sign * acos(clamp((b * b + c * c - aSq) / (2 * b * c), -1, 1))
	return retval
    
    // h/t https://www.youtube.com/watch?v=3YFUQDRL1s4 Computational Geometry: Line Segment Properties (Two lines Clockwise or Counterclockwise)
}

function getBBox(shape) {
	var retval = {
		nw: {x:shape.x1,y:shape.y1},
		ne: {x:shape.x2,y:shape.y1},
		se: {x:shape.x2,y:shape.y2},
		sw: {x:shape.x1,y:shape.y2}
	}
	return retval
}

var selectShapes = function (shapesToSelect) {
	var xValues = []
    var yValues = []
    var atLeastOneShapeHasDifferentRotateAngle = false
    var prevRotateAngle = 'undefined'
    var rerotatedBBox = []
	var i = 0
	var shapeCount = length(shapesToSelect)
	if (shapeCount > 0) {
		for (i = 0; i < shapeCount; i=i+1) {
			var shape = shapesToSelect[i]
			var mat = shape.matrix
			var bbox = getBBox(shape)
			var transformedBoundingBox = []
			transformedBoundingBox[0] = applyMatrix(mat, bbox.nw)
			transformedBoundingBox[1] = applyMatrix(mat, bbox.ne)
			transformedBoundingBox[2] = applyMatrix(mat, bbox.se)
			transformedBoundingBox[3] = applyMatrix(mat, bbox.sw)
			shape.transformedBoundingBox = transformedBoundingBox
			xValues[length(xValues)] = transformedBoundingBox[0].x
			xValues[length(xValues)] = transformedBoundingBox[1].x
			xValues[length(xValues)] = transformedBoundingBox[2].x
			xValues[length(xValues)] = transformedBoundingBox[3].x
			yValues[length(yValues)] = transformedBoundingBox[0].y
			yValues[length(yValues)] = transformedBoundingBox[1].y
			yValues[length(yValues)] = transformedBoundingBox[2].y
			yValues[length(yValues)] = transformedBoundingBox[3].y
			var line2EndPoint = {x: 0, y: 0}
			line2EndPoint.x = transformedBoundingBox[3].x
			line2EndPoint.y = transformedBoundingBox[3].y - 50
			var rotateAngle = getAngleBetweenTwoTouchingLineSegments(transformedBoundingBox[3], transformedBoundingBox[0], line2EndPoint)
			var endOfNormalToSide = rotatePoint(transformedBoundingBox[3], 0, 0-1, transformedBoundingBox[0].x, transformedBoundingBox[0].y)
			var rotateAngleDeg = rotateAngle / (3.14159265 / 180)
			shape.rotateAngleDeg = rotateAngleDeg
			if (prevRotateAngle == 'undefined') {
				var prev = 'undefined'
			} else {
				var prev = round(prevRotateAngle * 100) / 100
			}
			var cur = round(rotateAngleDeg * 100) / 100
			shape.cosAngle = cos(rotateAngle)
			shape.sinAngle = sin(rotateAngle)
			var angle = getAngleBetweenTwoTouchingLineSegments(transformedBoundingBox[0], getMidpointOfTwoPoints(transformedBoundingBox[0], transformedBoundingBox[1]), endOfNormalToSide)
			shape.tanShearAngle = tan(angle)

			if (prev == -360) { prev = 360 }
			if (cur == -360) { cur = 360 }
			if (prev == -180) { prev = 180 }
			if (cur == -180) { cur = 180 }
			if (prev != cur) {
				if (prevRotateAngle != 'undefined') {
					atLeastOneShapeHasDifferentRotateAngle = true
				}
				prevRotateAngle = cur
			}
		}
		
		var i = 0
		var len = length(xValues)
		var minX = xValues[0]
		var maxX = xValues[0]
		for (i = 1; i < len; i=i+1) {
			minX = min(minX, xValues[i])
			maxX = max(maxX, xValues[i])
		}
		
		var i = 0
		var len = length(yValues)
		var minY = yValues[0]
		var maxY = yValues[0]
		for (i = 1; i < len; i=i+1) {
			minY = min(minY, yValues[i])
			maxY = max(maxY, yValues[i])
		}
		
		if (shapeCount > 0) {
			if (atLeastOneShapeHasDifferentRotateAngle) {
				bboxCentroid = {
					x: (primMinX + primMaxX) / 2,
					y: (primMinY + primMaxY) / 2
				}
				rotateHandle = {
					x: (primMinX + primMaxX) / 2,
					y: primMinY - 50
				}
				rotateConnectorFrom = {
					x: (primMinX + primMaxX) / 2,
					y: primMinY - 50
				}
				rotateConnectorTo = {
					x: (primMinX + primMaxX) / 2,
					y: primMinY
				}
				selectBoxKnots = []
				selectBoxKnots[0] = {x: primMinX, y: primMinY}
				selectBoxKnots[1] = {x: primMaxX, y: primMinY}
				selectBoxKnots[2] = {x: primMaxX, y: primMaxY}
				selectBoxKnots[3] = {x: primMinX, y: primMaxY}
				selectBoxKnots[4] = {x: primMinX, y: primMinY}
			} else {
				var unrotatedXValues = []
				var unrotatedYValues = []
				var i = 0
				for (i = 0; i < shapeCount; i = i + 1) {
					var j = 0
					var shape = shapesToSelect[i]
					for (j = 0; j < 4; j = j + 1) {
						var point = shape.transformedBoundingBox[j]
						var unrotatedPoint = rotatePoint({x: point.x - minX, y: point.y - maxY}, shape.cosAngle, 0-shape.sinAngle, 0, 0)
						unrotatedPoint = {x: unrotatedPoint.x, y: (0-shape.tanShearAngle) * unrotatedPoint.x + unrotatedPoint.y}
						unrotatedXValues[length(unrotatedXValues)] = unrotatedPoint.x
						unrotatedYValues[length(unrotatedYValues)] = unrotatedPoint.y
					}
				}
				
				var i = 0
				var len = length(unrotatedXValues)
				var unrotatedMinX = unrotatedXValues[0]
				var unrotatedMaxX = unrotatedXValues[0]
				for (i = 1; i < len; i=i+1) {
					unrotatedMinX = min(unrotatedMinX, unrotatedXValues[i])
					unrotatedMaxX = max(unrotatedMaxX, unrotatedXValues[i])
				}
				
				var i = 0
				var len = length(unrotatedYValues)
				var unrotatedMinY = unrotatedYValues[0]
				var unrotatedMaxY = unrotatedYValues[0]
				for (i = 1; i < len; i=i+1) {
					unrotatedMinY = min(unrotatedMinY, unrotatedYValues[i])
					unrotatedMaxY = max(unrotatedMaxY, unrotatedYValues[i])
				}
				
				rerotatedBBox[0] = {x: unrotatedMinX, y: unrotatedMinY}
				rerotatedBBox[1] = {x: unrotatedMaxX, y: unrotatedMinY}
				rerotatedBBox[2] = {x: unrotatedMaxX, y: unrotatedMaxY}
				rerotatedBBox[3] = {x: unrotatedMinX, y: unrotatedMaxY}
				
				var i = 0
				var len = 4
				var centroidX = 0
				var centroidY = 0
				var finalRotated = []
				for (i = 0; i < len; i = i + 1) {
					Log(i)
					var point = rerotatedBBox[i]
					point = {x: point.x, y: (shapesToSelect[0].tanShearAngle * point.x) + point.y}
					point = rotatePoint({x: point.x, y: point.y}, shapesToSelect[0].cosAngle, shapesToSelect[0].sinAngle, 0, 0)
					finalRotated[length(finalRotated)] = {x: point.x + minX, y: point.y + maxY}
					centroidX = centroidX + finalRotated[i].x / 4
					centroidY = centroidY + finalRotated[i].y / 4
					var pt = finalRotated[i]
				}
				tight = 'true'
				
				selectBoxKnots = []
				for (i = 0; i < len; i = i + 1) {
					selectBoxKnots[i] = finalRotated[i]
				}
				selectBoxKnots[length(selectBoxKnots)] = finalRotated[0]
				
				var c = getMidpointOfTwoPoints(finalRotated[2], finalRotated[3])
				var d = getMidpointOfTwoPoints(finalRotated[0], finalRotated[1])
				var modDMinusC = sqrt((d.x - c.x) * (d.x - c.x) + (d.y - c.y) * (d.y - c.y))
				var tValue = 1 + 35 / modDMinusC
				var nPoint = {
					x: c.x + tValue * (d.x - c.x),
					y: c.y + tValue * (d.y - c.y)
				}
				
				bboxCentroid = {x: centroidX, y: centroidY}
				rotateHandle = {x: nPoint.x, y: nPoint.y}
				rotateConnectorFrom = {x: nPoint.x, y: nPoint.y}
				rotateConnectorTo = {x: d.x, y: d.y}
			}
		} else {
			rotateHandle = {x: -9999, y: -9999}
			rotateConnectorFrom = {x: -9999, y: -9999}
			rotateConnectorTo = {x: -9999, y: -9999}
			selectBoxKnots = []
		}
		
		selectedShapes = []
		var i = 0
		for (i = 0; i < shapeCount; i = i + 1) {
			selectedShapes[i] = shapesToSelect[i]
		}
		
		if (length(shapeCount) == 0) {
			minX = -9999
			minY = -9999
			maxX = -9999
			maxY = -9999
		}
	}
}