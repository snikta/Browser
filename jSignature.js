/** @license
jSignature v2 SVG export plugin.

*/
/**
Copyright (c) 2012 Willow Systems Corp http://willow-systems.com
MIT License <http://www.opensource.org/licenses/mit-license.php>
*/

/**
Vector class. Allows us to simplify representation and manipulation of coordinate-pair
representing shift against (0, 0)

@public
@class
@param
@returns {Type}
*/
function Vector(x,y){
	this.x = x
	this.y = y
	this.reverse = function(){
		return new this.constructor( 
			this.x * -1
			, this.y * -1
		)
	}
	this._length = null
	this.getLength = function(){
		if (!this._length){
			this._length = Math.sqrt( Math.pow(this.x, 2) + Math.pow(this.y, 2) )
		}
		return this._length
	}
	
	var polarity = function (e){
		return Math.round(e / Math.abs(e))
	}
	this.resizeTo = function(length){
		// proportionally changes x,y such that the hypotenuse (vector length) is = new length
		if (this.x === 0 && this.y === 0){
			this._length = 0
		} else if (this.x === 0){
			this._length = length
			this.y = length * polarity(this.y)
		} else if(this.y === 0){
			this._length = length
			this.x = length * polarity(this.x)
		} else {
			var proportion = Math.abs(this.y / this.x)
				, x = Math.sqrt(Math.pow(length, 2) / (1 + Math.pow(proportion, 2)))
				, y = proportion * x
			this._length = length
			this.x = x * polarity(this.x)
			this.y = y * polarity(this.y)
		}
		return this
	}
	
	/**
	 * Calculates the angle between 'this' vector and another.
	 * @public
	 * @function
	 * @returns {Number} The angle between the two vectors as measured in PI. 
	 */
	this.angleTo = function(vectorB) {
		var divisor = this.getLength() * vectorB.getLength()
		if (divisor === 0) {
			return 0
		} else {
			// JavaScript floating point math is screwed up.
			// because of it, the core of the formula can, on occasion, have values
			// over 1.0 and below -1.0.
			return Math.acos(
				Math.min( 
					Math.max( 
						( this.x * vectorB.x + this.y * vectorB.y ) / divisor
						, -1.0
					)
					, 1.0
				)
			) / Math.PI
		}
	}
}

function Point(x,y){
	this.x = x
	this.y = y
	
	this.getVectorToCoordinates = function (x, y) {
		return new Vector(x - this.x, y - this.y)
	}
	this.getVectorFromCoordinates = function (x, y) {
		return this.getVectorToCoordinates(x, y).reverse()
	}
	this.getVectorToPoint = function (point) {
		return new Vector(point.x - this.x, point.y - this.y)
	}
	this.getVectorFromPoint = function (point) {
		return this.getVectorToPoint(point).reverse()
	}
}

/**
Allows one to round a number to arbitrary precision.
Math.round() rounds to whole only.
Number.toFixed(precision) returns a string.
I need float to float, but with arbitrary precision, hence:

@public
@function
@param number {Number}
@param position {Number} number of digits right of decimal point to keep. If negative, rounding to the left of decimal.
@returns {Type}
*/
function round (number, position){
	var tmp = Math.pow(10, position)
	return Math.round( number * tmp ) / tmp
}

function segmentToCurve(stroke, positionInStroke, lineCurveThreshold){
	'use strict'
	// long lines (ones with many pixels between them) do not look good when they are part of a large curvy stroke.
	// You know, the jaggedy crocodile spine instead of a pretty, smooth curve. Yuck!
	// We want to approximate pretty curves in-place of those ugly lines.
	// To approximate a very nice curve we need to know the direction of line before and after.
	// Hence, on long lines we actually wait for another point beyond it to come back from
	// mousemoved before we draw this curve.
	
	// So for "prior curve" to be calc'ed we need 4 points 
	// 	A, B, C, D (we are on D now, A is 3 points in the past.)
	// and 3 lines:
	//  pre-line (from points A to B), 
	//  this line (from points B to C), (we call it "this" because if it was not yet, it's the only one we can draw for sure.) 
	//  post-line (from points C to D) (even through D point is 'current' we don't know how we can draw it yet)
	//
	// Well, actually, we don't need to *know* the point A, just the vector A->B

	// Again, we can only derive curve between points positionInStroke-1 and positionInStroke
	// Thus, since we can only draw a line if we know one point ahead of it, we need to shift our focus one point ahead.
	positionInStroke += 1
	// Let's hope the code that calls us knows we do that and does not call us with positionInStroke = index of last point.
	
	var Cpoint = new Point(stroke.x[positionInStroke-1], stroke.y[positionInStroke-1])
		, Dpoint = new Point(stroke.x[positionInStroke], stroke.y[positionInStroke])
		, CDvector = Cpoint.getVectorToPoint(Dpoint)
	// Again, we have a chance here to draw only PREVIOUS line segment - BC
	
	// So, let's start with BC curve.
	// if there is only 2 points in stroke array (C, D), we don't have "history" long enough to have point B, let alone point A.
	// so positionInStroke should start with 2, ie
	// we are here when there are at least 3 points in stroke array.
	var Bpoint = new Point(stroke.x[positionInStroke-2], stroke.y[positionInStroke-2])
	, BCvector = Bpoint.getVectorToPoint(Cpoint)
	, ABvector
	, rounding = 2
	
	if ( BCvector.getLength() > lineCurveThreshold ){
		// Yey! Pretty curves, here we come!
		if(positionInStroke > 2) {
			ABvector = (new Point(stroke.x[positionInStroke-3], stroke.y[positionInStroke-3])).getVectorToPoint(Bpoint)
		} else {
			ABvector = new Vector(0,0)
		}
		var minlenfraction = 0.05
		, maxlen = BCvector.getLength() * 0.35
		, ABCangle = BCvector.angleTo(ABvector.reverse())
		, BCDangle = CDvector.angleTo(BCvector.reverse())
		, BtoCP1vector = new Vector(ABvector.x + BCvector.x, ABvector.y + BCvector.y).resizeTo(
			Math.max(minlenfraction, ABCangle) * maxlen
		)
		, CtoCP2vector = (new Vector(BCvector.x + CDvector.x, BCvector.y + CDvector.y)).reverse().resizeTo(
			Math.max(minlenfraction, BCDangle) * maxlen
		)
		, BtoCP2vector = new Vector(BCvector.x + CtoCP2vector.x, BCvector.y + CtoCP2vector.y)
		
		// returing curve for BC segment
		// all coords are vectors against Bpoint
		return new Presentation.classes.primitives.BezierCurve(
			{x: Bpoint.x, y: Bpoint.y},
			{x: round( Bpoint.x + BtoCP1vector.x, rounding ), y: round( Bpoint.y + BtoCP1vector.y, rounding )},
			{x: round( Bpoint.x + BtoCP2vector.x, rounding ), y: round( Bpoint.y + BtoCP2vector.y, rounding )},
			{x: round( Bpoint.x + BCvector.x, rounding ), y: round( Bpoint.y + BCvector.y, rounding )}
		);
	} else {
		return new Presentation.classes.primitives.Lineto({
			x: round( Bpoint.x + BCvector.x, rounding ),
			y: round( Bpoint.y + BCvector.y, rounding )
		});
	}
}

function lastSegmentToCurve(stroke, lineCurveThreshold){
	'use strict'
	// Here we tidy up things left unfinished
	
	// What's left unfinished there is the curve between the last points
	// in the stroke
	// We can also be called when there is only one point in the stroke (meaning, the 
	// stroke was just a dot), in which case there is nothing for us to do.

	// So for "this curve" to be calc'ed we need 3 points 
	// 	A, B, C
	// and 2 lines:
	//  pre-line (from points A to B), 
	//  this line (from points B to C) 
	// Well, actually, we don't need to *know* the point A, just the vector A->B
	// so, we really need points B, C and AB vector.
	var positionInStroke = stroke.x.length - 1
	
	// there must be at least 2 points in the stroke.for us to work. Hope calling code checks for that.
	var Cpoint = new Point(stroke.x[positionInStroke], stroke.y[positionInStroke])
	, Bpoint = new Point(stroke.x[positionInStroke-1], stroke.y[positionInStroke-1])
	, BCvector = Bpoint.getVectorToPoint(Cpoint)
	, rounding = 2
	
	if (positionInStroke > 1 && BCvector.getLength() > lineCurveThreshold){
		// we have at least 3 elems in stroke
		var ABvector = (new Point(stroke.x[positionInStroke-2], stroke.y[positionInStroke-2])).getVectorToPoint(Bpoint)
		, ABCangle = BCvector.angleTo(ABvector.reverse())
		, minlenfraction = 0.05
		, maxlen = BCvector.getLength() * 0.35
		, BtoCP1vector = new Vector(ABvector.x + BCvector.x, ABvector.y + BCvector.y).resizeTo(
			Math.max(minlenfraction, ABCangle) * maxlen
		)
		
		return new Presentation.classes.primitives.BezierCurve(
			{x: Bpoint.x, y: Bpoint.y},
			{x: round( Bpoint.x + BtoCP1vector.x, rounding ), y: round( Bpoint.y + BtoCP1vector.y, rounding )},
			{x: round( Bpoint.x + BCvector.x, rounding ), y: round( Bpoint.y + BCvector.y, rounding )}, // CP2 is same as Cpoint
			{x: round( Bpoint.x + BCvector.x, rounding ), y: round( Bpoint.y + BCvector.y, rounding )});
	} else {
		// Since there is no AB leg, there is no curve to Presentation. This is just line
		return new Presentation.classes.primitives.Lineto({
				x: round( Bpoint.x + BCvector.x, rounding ),
				y: round( Bpoint.y + BCvector.y, rounding )
			});
	}
}

function addstroke(stroke, shiftx, shifty){
	'use strict'
	// we combine strokes data into string like this:
	// 'M 53 7 l 1 2 c 3 4 -5 -6 5 -6'
	// see SVG documentation for Path element's 'd' argument.
	var lines = [
		new Presentation.classes.primitives.Moveto({
			x: round( (stroke.x[0] - shiftx), 2),
			y: round( (stroke.y[0] - shifty), 2)
		})
	]
	// processing all points but first and last. 
	, i = 1 // index zero item in there is STARTING point. we already extracted it.
	, l = stroke.x.length - 1 // this is a trick. We are leaving last point coordinates for separate processing.
	, lineCurveThreshold = 0.001
	
	for(; i < l; i++){
		lines.push(segmentToCurve(stroke, i, lineCurveThreshold));
	}
	if (l > 0 /* effectively more than 1, since we "-1" above */){
		lines.push(lastSegmentToCurve(stroke, i, lineCurveThreshold));
	} else if (l === 0){
		// meaning we only have ONE point in the stroke (and otherwise refer to the stroke as "dot")
		lines.push(new Presentation.classes.primitives.Lineto({x: stroke.x, y: stroke.y}));
	}
	return lines;
}

var sx=[12,15,17,20,23,34,38,45]
var sy=[13,12,10,15,23,25,34,40]

