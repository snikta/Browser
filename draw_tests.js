var canvasEl = createElement("canvas")
canvasEl.width = "800px"
canvasEl.height = "400px"
canvasEl.style.display = "block"
canvasEl.style.position = "absolute"

var len = 10;
var matrix = "";
for (i = 0; i <= len; i = i + 1) {
	var j = 0
	for (j = 0; j <= len; j = j + 1) {
		if (i == j) {
			matrix = matrix + "0 "
		}
		else if (len - i == j) {
			matrix = matrix + "1 "
		}
		else if (i == 5) {
			matrix = matrix + "2 "
		}
		else if (j == 5) {
			matrix = matrix + "3 "
		}
		else {
			matrix = matrix + "  "
		}
	}
	matrix = matrix +"\n"
}

DrawText(canvasEl, (5) * (6 + ((2 + 1)-(3 / 4)) * 10), 0, 0)
DrawText(canvasEl, -15.2 + -3.2 / 2.4-18.7 * -6.45 + 3 / 45.286, 0, 10)
DrawText(canvasEl, 5 / (8 + 16.25 * (91.23 / (18.25 + -(14.9 / (-18 / 2.5)) + (8 / (-6 / -25.8 * (-1.5 + (-8 * -6 * (52.3 + 18.0)) / 2.5)))))), 0, 20)
DrawText(canvasEl, 3 + (8 / (9-4.5 * (2 + (10 * (6 / (5 + -1)-2))))), 0, 30)
DrawText(canvasEl, (((((((((2 * 0 + 1) * 2 + 0) * 2 + 1) * 2 + 1) * 2 + 1) * 2 + 0) * 2 + 1) * 2 + 0) * 2 + 1) * 2 + 1, 0, 40)
DrawText(canvasEl, Multiply(Add(5, (8 * (4 / 5)), "45", 2), Divide(Add(10, -1), Divide(2, 1.5))), 0, 50)
DrawText(canvasEl, Add(Divide(3, 5), (8 * (4 / 5)), 45, 2), 0, 60)
DrawText(canvasEl, "Hello " + me.firstName + " " + me.lastName + ". Your date of birth is: " + me.dateOfBirth.monthName + " " + me.dateOfBirth.date + ", " + me.dateOfBirth.year  +"! You are: " + (currentYear - yearOfBirth) + " years old.", 0, 70)
DrawText(canvasEl, "Your middle names are " + me.middleNames.first + " and " + me["middle" + "Names"].second + ". " + spacemission + " occurred " + (yearOfBirth - 1969) + " years before your birth.", 0, 80)
DrawText(canvasEl, "Second mission in 1961: " + missionsByYear[1970 - 9][2 - 1] + ". First mission in 1961: " + missionsByYear[1961][0] + ". Missions eight years later: first=" + missionsByYear[1969][0] + " second=" + missionsByYear[1969][1], 0, 90)

i = 15
j = 22
k = 37

DrawText(canvasEl, josh = benjamin = mark = atkins = 1.5 * 2 * 3 / 2 * 4, 0, 100)
DrawText(canvasEl, i/=j*=k-=5, 0, 110)
DrawText(canvasEl, josh=(hey=(hello="world")+"_earth")+"_atkins", 0, 120)

var fibCount = 0
function fib(x) {
var n = 0
	var xN1 = 0
	var xN2 = 0
	fibCount = fibCount + 1
	if (x <= 1) {
		fibCache[x] = x
			return x
	}
	else {
		if (isset("fibCache", x - 1)) {
			xN1= fibCache[x - 1]
		}
		else {
			xN1 = fib(x - 1)
		}
		if (isset("fibCache", x - 2)) {
			xN2 = fibCache[x - 2]
		}
		else {
			xN2 = fib(x - 2)
		}
		n = xN1 + xN2
	}
	fibCache[x] = n
	return n
}

var factCount = 0
	function fact(x) {
	factCount = factCount + 1
	if (x <= 1) {
		return 1
	}
	if (isset("factCache", x)) {
		return factCache[x]
	}
	factCache[x] = x * fact(x - 1)
	return factCache[x]
}

DrawText(canvasEl, "fib(13): " + fib(13), 0, 130)
DrawText(canvasEl, "fib(25): " + fib(25), 0, 140)
DrawText(canvasEl, "6!= " + fact(6), 0, 150)
DrawText(canvasEl, "100!= " + fact(100), 0, 160)

function Shape() {
}
function Polygon() {
}
function Rectangle(w, h) {
	var that = this
	that.width = w
	that.height = h
}
function Triangle() {
}
function IsoscelesTriangle() {
}
function () {
}
Polygon.prototype = createPrototype(Shape.prototype)
Rectangle.prototype = createPrototype(Polygon.prototype)
function getRectArea() {
	var that = this
		var retval = ""
		if ("width" in that) {
			retval = that.width * that.height
		}
		else if ("sideLength" in that) {
			retval = that.sideLength * that.sideLength
		}
	return retval
}
Rectangle.prototype.findArea = getRectArea
Rectangle.prototype.buzz = "Aldrin"
var myrect = new Rectangle(15, 33)
function Square(l) {
	var that = this
		that.sideLength = l
}
Square.prototype = createPrototype(Rectangle.prototype)
var mysqr = new Square(13.2)
Shape.prototype.startPoint = "SHAPE_START_POINT"
Shape.prototype.endPoint = "SHAPE_END_POINT"
Polygon.prototype.points = "POLYGON_POINTS"
Triangle.prototype = createPrototype(Polygon.prototype)
IsoscelesTriangle.prototype = createPrototype(Triangle.prototype)
IsoscelesTriangle.prototype.triangleType = "Isoceles"
RightTriangle.prototype = createPrototype(Triangle.prototype)
RightTriangle.prototype.triangleType = "RightAngled"
var myiso = new IsoscelesTriangle()
DrawText(canvasEl, mysqr.buzz + " " + mysqr.points + " " + myiso.points, 0, 170)
DrawText(canvasEl, mysqr.sideLength + " " + myiso.triangleType + " " + myrect.findArea() + " " + mysqr.findArea(), 0, 180)

function myConstructor(w, h) {
	this.test = "bart simpson"
	var that = this
	that.width = w
	that.height = h
	function getArea() {
	return that.width * that.height
}
that.findArea = getArea
//"Log("First try: " + that.findArea())
}
var myObj = new myConstructor(15, 23)
DrawText(canvasEl, "Area: " + myObj.width * myObj.height + " vs. " + myObj.findArea(), 0, 190)

myObj.width = 23.6
myObj.height = 13.287
Log(myObj.findArea())
DrawText(canvasEl, "myObj.findArea() = " + myObj.findArea(), 0, 200)

function outerFunc(x, level) {
	function innerFunc(y, lvl) {
		function innermostFunc(z) {
			return (x + y) * z
		}
		DrawText(canvasEl, "innermostFunc(7): "+innermostFunc(7), 0, 210 + lvl * 10)
		return innermostFunc(3)
	}
	DrawText(canvasEl, "innerFunc(4): "+innerFunc(4, level + 1), 0, 210 + level * 10)
	return innerFunc(10, level + 2)
}
DrawText(canvasEl, "outerFunc(3): "+outerFunc(3, 1), 0, 210)
