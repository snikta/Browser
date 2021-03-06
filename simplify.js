Log("simplify.js")

function getSqDist(p1, p2) {
    var dx = p1.x - p2.x
    var dy = p1.y - p2.y

    return dx * dx + dy * dy
}

function getSqSegDist(p, p1, p2) {
    var x = p1.x
    var y = p1.y
    var dx = p2.x - x
    var dy = p2.y - y
	
    if (dx != 0 || dy != 0) {
        var t = ((p.x - x) * dx + (p.y - y) * dy) / (dx * dx + dy * dy)
        if (t > 1) {
            x = p2.x
            y = p2.y

        } else if (t > 0) {
            x += dx * t
            y += dy * t
        }
    }
    dx = p.x - x
    dy = p.y - y

    return dx * dx + dy * dy
}

function simplifyRadialDist(points, sqTolerance) {

    var prevPoint = points[0]
    var nPoints = []
    var point
	var i = 0
	var len = length(points)
	nPoints[0] = prevPoint

    for (i = 1; i <= len - 1; i = i + 1) {
        point = points[i]

        if (getSqDist(point, prevPoint) > sqTolerance) {
			var nplen = length(nPoints)
            nPoints[nplen] = point
            prevPoint = point
        }
    }

    if (prevPoint != point) {
		var nplen = length(nPoints)
		nPoints[nplen] = point
	}

    return nPoints
}

function simplifyDPStep(points, first, last, sqTolerance, simplified) {
    var maxSqDist = sqTolerance
    var index = -1
	var i = 0

    for (i = first + 1; i <= last - 1; i = i + 1) {
        var sqDist = getSqSegDist(points[i], points[first], points[last])
        if (sqDist > maxSqDist) {
            index = i
            maxSqDist = sqDist
        }
    }
	
    if (maxSqDist > sqTolerance) {
        if (index - first > 1) {
			simplifyDPStep(points, first, index, sqTolerance, simplified)
		}
		var slen = length(simplified)
        simplified[slen] = points[index]
        if (last - index > 1) {
			simplifyDPStep(points, index, last, sqTolerance, simplified)
		}
    }
}

/* simplification using Ramer-Douglas-Peucker algorithm */
function simplifyDouglasPeucker(points, sqTolerance) {
    var last = length(points) - 1
    var simplified = []
	simplified[0] = points[0]
    simplifyDPStep(points, 0, last, sqTolerance, simplified)
    simplified[length(simplified)] = points[last]

    return simplified
}

function simplify(points, tolerance, highestQuality) {

	var sqTolerance = 1
	if (tolerance > 0) {
		sqTolerance = tolerance * tolerance
	}

	if (highestQuality != true) {
		points = simplifyRadialDist(points, sqTolerance)
	}
		
    points = simplifyDouglasPeucker(points, sqTolerance)

    return points
}
