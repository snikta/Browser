var simplify = {}

var getSqDist = function (p1, p2) {
    var dx = p1.x - p2.x
    var dy = p1.y - p2.y

    return dx * dx + dy * dy;
}

function getSqSegDist(p, p1, p2) {
    var x = p1.x
    var y = p1.y
    var dx = p2.x - x
    var dy = p2.y - y
	
    if (dx != 0 || dy != 0) {

        var t = ((p.x - x) * dx + (p.y - y) * dy) / (dx * dx + dy * dy);
        if (t > 1) {
            x = p2.x;
            y = p2.y;

        } else if (t > 0) {
            x += dx * t;
            y += dy * t;
        }
    }
    dx = p.x - x;
    dy = p.y - y;

    return dx * dx + dy * dy;
}

var simplifyRadialDist = function (points, sqTolerance) {

    var prevPoint = points[0]
    var newPoints = [prevPoint]
    var point

    for (var i = 1, len = length(points); i < len; i++) {
        point = points[i];

        if (getSqDist(point, prevPoint) > sqTolerance) {
            newPoints[length(newPoints)] = point;
            prevPoint = point;
        }
    }

    if (prevPoint != point) newPoints[length(newPoints)] = point;

    return newPoints;
}

function simplifyDPStep(points, first, last, sqTolerance, simplified) {
    var maxSqDist = sqTolerance
    var index = -1

    for (i = first + 1; i < last; i++) {
        var sqDist = getSqSegDist(points[i], points[first], points[last]);
        if (sqDist > maxSqDist) {
            index = i;
            maxSqDist = sqDist;
        }
    }
	
    if (maxSqDist > sqTolerance) {
        if (index - first > 1) {
			simplifyDPStep(points, first, index, sqTolerance, simplified);
		}
        simplified.push(points[index]);
        if (last - index > 1) {
			simplifyDPStep(points, index, last, sqTolerance, simplified);
		}
    }
}

/* simplification using Ramer-Douglas-Peucker algorithm */
var simplifyDouglasPeucker = function (points, sqTolerance) {
    var last = length(points) - 1;
    var simplified = [];
	simplified[0] = points[0]
    simplifyDPStep(points, 0, last, sqTolerance, simplified);
    simplified[length(simplified)] = points[last]

    return simplified;
}

var simplify = function (points, tolerance, highestQuality) {

    if (length(points) <= 2) {
		return points;
	}

	var sqTolerance = 1
	if (tolerance > 0) {
		sqTolerance = tolerance * tolerance
	}

	if (!highestQuality) {
		points = simplifyRadialDist(points, sqTolerance);
	}
		
    points = simplifyDouglasPeucker(points, sqTolerance);

    return points;
}

var tPoints = [{x:12,y:13},{x:15,y:12},{x:17,y:10},{x:20,y:15},{x:23,y:23},{x:34,y:25},{x:38,y:34},{x:45,y:40}]
var testPoints = simplify(tPoints, 3, false)
i = 0
len = length(testPoints)
Log("testPoints.length: " + len)
for (; i < len; i = i + 1) {
	Log(i + ": (" + testPoints[i].x + ", " + testPoints[i].y + ")")
}