Log("jSignature.js")

var testPoints = [{x:12,y:13},{x:15,y:12},{x:17,y:10},{x:20,y:15},{x:23,y:23},{x:34,y:25},{x:38,y:34},{x:45,y:40}]

function Vector(x,y){
	var that = this
	that.x = x
	that.y = y
}
function vecReverse() {
	var that = this
	var vec = new Vector( that.x * -1, that.y * -1)
	return vec
}
Vector.prototype.reverse = vecReverse
function vecGetLength() {
	var that = this
	if ("_getLength" in that) {
		return that._getLength
	}
	var nlen = sqrt(pow(that.x, 2) + pow(that.y, 2))
	that._getLength = nlen
	return nlen
}
Vector.prototype.getLength = vecGetLength
function vecResizeTo(len){
	var that = this
	if (that.x == 0 && that.y == 0){
		that._getLength = 0
	} else if (that.x == 0){
		that._getLength = len
		that.y = len * polarity(that.y)
	} else if(that.y == 0){
		that._getLength = len
		that.x = len * polarity(that.x)
	} else {
		var proportion = abs(that.y / that.x)
		var x = sqrt(pow(len, 2) / (1 + pow(proportion, 2)))
		var y = proportion * x
		that._getLength = len
		that.x = x * polarity(that.x)
		that.y = y * polarity(that.y)
	}
	return that
}
Vector.prototype.resizeTo = vecResizeTo
function vecAngleTo(vec) {
	var that = this
	var divisor = that.getLength() * vec.getLength()
	if (divisor == 0) {
		return 0
	} else {
		return acos(min(max(( that.x * vec.x + that.y * vec.y ) / divisor, -1.0), 1.0)) / 3.14159265
	}
}
Vector.prototype.angleTo = vecAngleTo
function polarity(num){
	return round(num / abs(num))
}
Vector.prototype.polarity = polarity

function Point(x,y){
	var that = this
	that.x = x
	that.y = y
}
var _getVectorToCoordinates = function (x, y) {
	var that = this
	var retval = new Vector(x - that.x, y - that.y)
	return retval
}
Point.prototype.getVectorToCoordinates = _getVectorToCoordinates
var getVectorFromCoordinates = function (x, y) {
	var that = this
	var retval = that.getVectorToCoordinates(x, y).reverse()
	return retval
}
Point.prototype.getVectorFromCoordinates = getVectorFromCoordinates
var _getVectorToPoint = function (point) {
	var that = this
	var retval = new Vector(point.x - that.x, point.y - that.y)
	return retval
}
Point.prototype.getVectorToPoint = _getVectorToPoint
var _getVectorFromPoint = function (point) {
	var that = this
	var retval = that.getVectorToPoint(point).reverse()
	return retval
}
Point.prototype.getVectorFromPoint = _getVectorFromPoint

function roundDP(number, position){
	var tmp = pow(10, position)
	var retval = round(number * tmp) / tmp
	return retval
}

function segmentToCurve(stroke, positionInStroke, lineCurveThreshold){
	positionInStroke = positionInStroke + 1
	
	var Cpoint = new Point(stroke.x[positionInStroke-1], stroke.y[positionInStroke-1])
	var Dpoint = new Point(stroke.x[positionInStroke], stroke.y[positionInStroke])
	var CDvector = Cpoint.getVectorToPoint(Dpoint)
	
	var Bpoint = new Point(stroke.x[positionInStroke-2], stroke.y[positionInStroke-2])
	var BCvector = Bpoint.getVectorToPoint(Cpoint)
	var rounding = 2
	
	if ( BCvector.getLength() > lineCurveThreshold ) {
		if(positionInStroke > 2) {
			var ABvector = new Point(stroke.x[positionInStroke-3], stroke.y[positionInStroke-3])
			ABvector = ABvector.getVectorToPoint(Bpoint)
		} else {
			var ABvector = new Vector(0,0)
		}
		var minlenfraction = 0.05
		var maxlen = BCvector.getLength() * 0.35
		var ABCangle = BCvector.angleTo(ABvector.reverse())
		var BCDangle = CDvector.angleTo(BCvector.reverse())
		var BtoCP1vector = new Vector(ABvector.x + BCvector.x, ABvector.y + BCvector.y)
		BtoCP1vector = BtoCP1vector.resizeTo(max(minlenfraction, ABCangle) * maxlen)
		var CtoCP2vector = new Vector(BCvector.x + CDvector.x, BCvector.y + CDvector.y)
		CtoCP2vector = CtoCP2vector.reverse()
		CtoCP2vector = CtoCP2vector.resizeTo(max(minlenfraction, BCDangle) * maxlen)
		var BtoCP2vector = new Vector(BCvector.x + CtoCP2vector.x, BCvector.y + CtoCP2vector.y)
		
		var points = []
			points[0]={x: Bpoint.x, y: Bpoint.y}
			points[1]={x: roundDP( Bpoint.x + BtoCP1vector.x, rounding ), y: roundDP( Bpoint.y + BtoCP1vector.y, rounding )}
			points[2]={x: roundDP( Bpoint.x + BtoCP2vector.x, rounding ), y: roundDP( Bpoint.y + BtoCP2vector.y, rounding )}
			points[3]={x: roundDP( Bpoint.x + BCvector.x, rounding ), y: roundDP( Bpoint.y + BCvector.y, rounding )}
		var retval = {type: 'BezierCurve', points: points}
		return retval
	} else {
		var points = []
		points[0] = {
			x: roundDP( Bpoint.x + BCvector.x, rounding ),
			y: roundDP( Bpoint.y + BCvector.y, rounding )
		}
		var retval = {type:'Lineto', points: points}
		return retval
	}
}

function lastSegmentToCurve(stroke, lineCurveThreshold){
	var positionInStroke = length(stroke.x) - 1
	var Cpoint = new Point(stroke.x[positionInStroke], stroke.y[positionInStroke])
	var Bpoint = new Point(stroke.x[positionInStroke-1], stroke.y[positionInStroke-1])
	var BCvector = Bpoint.getVectorToPoint(Cpoint)
	var rounding = 2
	
	if (positionInStroke > 1) {
		if (BCvector.getLength() > lineCurveThreshold) {
			var ABvector = new Point(stroke.x[positionInStroke-2], stroke.y[positionInStroke-2])
			ABvector = ABvector.getVectorToPoint(Bpoint)
			var ABCangle = BCvector.angleTo(ABvector.reverse())
			var minlenfraction = 0.05
			var maxlen = BCvector.getLength() * 0.35
			var BtoCP1vector = new Vector(ABvector.x + BCvector.x, ABvector.y + BCvector.y)
			BtoCP1vector = BtoCP1vector.resizeTo(max(minlenfraction, ABCangle) * maxlen)
			
			var points = []
			points[0]={x: Bpoint.x, y: Bpoint.y}
			points[1]={x: roundDP( Bpoint.x + BtoCP1vector.x, rounding ), y: roundDP( Bpoint.y + BtoCP1vector.y, rounding )}
			points[2]={x: roundDP( Bpoint.x + BCvector.x, rounding ), y: roundDP( Bpoint.y + BCvector.y, rounding )}
			points[3]={x: roundDP( Bpoint.x + BCvector.x, rounding ), y: roundDP( Bpoint.y + BCvector.y, rounding )}
			return {type:'BezierCurve', points:points}
		}
	} else {
		var points=[]
		points[0] = {
				x: roundDP( Bpoint.x + BCvector.x, rounding ),
				y: roundDP( Bpoint.y + BCvector.y, rounding )
		}
		var retval = {type:'Lineto', points:[points]}
		return retval
	}
}

function addstroke(stroke, shiftx, shifty){
	var point = {x: roundDP(stroke.x[0] - shiftx, 2),y: roundDP(stroke.y[0] - shifty, 2)}
	var points = [point]
	var lineTo = {type:'Moveto', points:points}
	var lines = []
	var i = 1
	var l = length(stroke.x) - 1
	var lineCurveThreshold = 0.001
	
	for(i = 1; i < l; i = i + 1){
		var lineCount = length(lines)
		lines[lineCount] = segmentToCurve(stroke, i, lineCurveThreshold)
	}
	if (l > 0){
		var lineCount = length(lines)
		lines[lineCount] = lastSegmentToCurve(stroke, i, lineCurveThreshold)
	} else if (l == 0){
		var lineCount = length(lines)
		lines[lineCount] = {type:'Lineto',points:[{x: stroke.x, y: stroke.y}]}
	}
	return lines
}

var getSplinePrimitivesFromKnots = function (knots) {
	var simplifiedPointsForCurveFitting = {x:[], y: []}
	var simplifiedPoints = []
	var i = 0
	var len = length(knots)
	
	for (i = 0; i <= len - 1; i = i + 1) {
		var curValue = knots[i]
		simplifiedPointsForCurveFitting.x[i] = curValue.x
		simplifiedPointsForCurveFitting.y[i] = curValue.y
	}
	
	var retval = addstroke(simplifiedPointsForCurveFitting, 0, 0)
	return retval
}
