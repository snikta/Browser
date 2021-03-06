Log("jSignature.js")

var testPoints = [{x:12,y:13},{x:15,y:12},{x:17,y:10},{x:20,y:15},{x:23,y:23},{x:34,y:25},{x:38,y:34},{x:45,y:40}]

function Vector(x,y){
	var that = this
	that.x = x
	that.y = y
	var reverse = function() {
		var that = this
		var vec = new Vector( that.x * -1, that.y * -1)
		return vec
	}
	that.reverse = reverse
	that._length = 0
	var getLength = function(){
		var that = this
		that._length = sqrt( pow(that.x, 2) + pow(that.y, 2) )
		return that._length
	}
	that.getLength = getLength
	
	var polarity = function (e){
		return round(e / abs(e))
	}
	that.polarity = polarity
	var resizeTo = function(length){
		var that = this
		if (that.x == 0 && that.y == 0){
			that._length = 0
		} else if (that.x == 0){
			that._length = length
			that.y = length * polarity(that.y)
		} else if(that.y == 0){
			that._length = length
			that.x = length * polarity(that.x)
		} else {
			var proportion = abs(that.y / that.x)
			var x = sqrt(pow(length, 2) / (1 + pow(proportion, 2)))
			var y = proportion * x
			that._length = length
			that.x = x * polarity(that.x)
			that.y = y * polarity(that.y)
		}
		return this
	}
	that.resizeTo = resizeTo
	
	var angleTo = function(vectorB) {
		var that = this
		var divisor = that.getLength() * vectorB.getLength()
		if (divisor == 0) {
			return 0
		} else {
			return acos(min(max(( that.x * vectorB.x + that.y * vectorB.y ) / divisor, -1.0), 1.0)) / PI
		}
	}
	that.angleTo = angleTo
}
Log("testPoints.length = " + length(testPoints))

function Point(x,y){
	that.x = x
	that.y = y
	
	var getVectorToCoordinates = function (x, y) {
		var that = this
		var retval = new Vector(x - that.x, y - that.y)
		return retval
	}
	that.getVectorToCoordinates = getVectorToCoordinates
	var getVectorFromCoordinates = function (x, y) {
		var that = this
		var retval = that.getVectorToCoordinates(x, y).reverse()
		return retval
	}
	that.getVectorFromCoordinates = getVectorFromCoordinates
	var getVectorToPoint = function (point) {
		var that = this
		var retval = new Vector(point.x - that.x, point.y - that.y)
		return retval
	}
	that.getVectorToPoint = getVectorToPoint
	var getVectorFromPoint = function (point) {
		var that = this
		var retval = that.getVectorToPoint(point).reverse()
		return retval
	}
	that.getVectorFromPoint = getVectorFromPoint
}

function roundDP(number, position){
	var tmp = pow(10, position)
	var retval = round(number * tmp) / tmp
	return retval
}

function segmentToCurve(stroke, positionInStroke, lineCurveThreshold){
	positionInStroke += 1
	
	var Cpoint = new Point(stroke.x[positionInStroke-1], stroke.y[positionInStroke-1])
	var Dpoint = new Point(stroke.x[positionInStroke], stroke.y[positionInStroke])
	var CDvector = Cpoint.getVectorToPoint(Dpoint)
	
	var Bpoint = new Point(stroke.x[positionInStroke-2], stroke.y[positionInStroke-2])
	var BCvector = Bpoint.getVectorToPoint(Cpoint)
	var ABvector = 0
	var rounding = 2
	
	if ( BCvector.getLength() > lineCurveThreshold ){
		if(positionInStroke > 2) {
			ABvector = (new Point(stroke.x[positionInStroke-3], stroke.y[positionInStroke-3])).getVectorToPoint(Bpoint)
		} else {
			ABvector = new Vector(0,0)
		}
		var minlenfraction = 0.05
		var maxlen = BCvector.getLength() * 0.35
		var ABCangle = BCvector.angleTo(ABvector.reverse())
		var BCDangle = CDvector.angleTo(BCvector.reverse())
		var BtoCP1vector = new Vector(ABvector.x + BCvector.x, ABvector.y + BCvector.y).resizeTo(max(minlenfraction, ABCangle) * maxlen)
		var CtoCP2vector = (new Vector(BCvector.x + CDvector.x, BCvector.y + CDvector.y)).reverse().resizeTo(max(minlenfraction, BCDangle) * maxlen)
		var BtoCP2vector = new Vector(BCvector.x + CtoCP2vector.x, BCvector.y + CtoCP2vector.y)
		
		return {type: 'BezierCurve', points: [
			{x: Bpoint.x, y: Bpoint.y},
			{x: roundDP( Bpoint.x + BtoCP1vector.x, rounding ), y: roundDP( Bpoint.y + BtoCP1vector.y, rounding )},
			{x: roundDP( Bpoint.x + BtoCP2vector.x, rounding ), y: roundDP( Bpoint.y + BtoCP2vector.y, rounding )},
			{x: roundDP( Bpoint.x + BCvector.x, rounding ), y: roundDP( Bpoint.y + BCvector.y, rounding )}
		]}
	} else {
		return {type:'Lineto', points: [{
			x: roundDP( Bpoint.x + BCvector.x, rounding ),
			y: roundDP( Bpoint.y + BCvector.y, rounding )
		}]}
	}
}

function lastSegmentToCurve(stroke, lineCurveThreshold){
	var positionInStroke = stroke.x.length - 1
	var Cpoint = new Point(stroke.x[positionInStroke], stroke.y[positionInStroke])
	var Bpoint = new Point(stroke.x[positionInStroke-1], stroke.y[positionInStroke-1])
	var BCvector = Bpoint.getVectorToPoint(Cpoint)
	var rounding = 2
	
	if (positionInStroke > 1 && BCvector.getLength() > lineCurveThreshold){
		var ABvector = (new Point(stroke.x[positionInStroke-2], stroke.y[positionInStroke-2])).getVectorToPoint(Bpoint)
		var ABCangle = BCvector.angleTo(ABvector.reverse())
		var minlenfraction = 0.05
		var maxlen = BCvector.getLength() * 0.35
		var BtoCP1vector = new Vector(ABvector.x + BCvector.x, ABvector.y + BCvector.y).resizeTo(
			max(minlenfraction, ABCangle) * maxlen
		)
		
		return {type:'BezierCurve', points:[
			{x: Bpoint.x, y: Bpoint.y},
			{x: roundDP( Bpoint.x + BtoCP1vector.x, rounding ), y: roundDP( Bpoint.y + BtoCP1vector.y, rounding )},
			{x: roundDP( Bpoint.x + BCvector.x, rounding ), y: roundDP( Bpoint.y + BCvector.y, rounding )},
			{x: roundDP( Bpoint.x + BCvector.x, rounding ), y: roundDP( Bpoint.y + BCvector.y, rounding )}]}
	} else {
		return {type:'Lineto', points:[{
				x: roundDP( Bpoint.x + BCvector.x, rounding ),
				y: roundDP( Bpoint.y + BCvector.y, rounding )
		}]}
	}
}

function addstroke(stroke, shiftx, shifty){
	var lines = [
		{type:'Moveto', points:[{x: roundDP(stroke.x[0] - shiftx, 2),y: roundDP(stroke.y[0] - shifty, 2)}]}
	]
	Log("lines[0]: (" + lines[0].points[0].x + ", " + lines[0].points[0].y + ")")
	var i = 1
	var l = length(stroke.x) - 1
	var lineCurveThreshold = 0.001
	
	for(; i < l; i++){
		var lineCount = length(lines)
		lines[lineCount] = segmentToCurve(stroke, i, lineCurveThreshold)
	}
	if (l > 0){
		var lineCount = length(lines)
		lines[lineCount] = lastSegmentToCurve(stroke, i, lineCurveThreshold)
	} else if (l == 0){
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

Log("testPoints.length: " + length(testPoints))
var knots = getSplinePrimitivesFromKnots(testPoints)
Log(length(knots))
