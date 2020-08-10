for (var key in me.dateOfBirth) {
	Log("me[" + key + "]: " + me.dateOfBirth[key])
}
/*"function objcreate(proto) {
	return createPrototype(proto)
}\n"*/
//"Object.create = objcreate
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
Log(mysqr.buzz)
Log(mysqr.points)
Log(myiso.points)
Log(mysqr.sideLength)
Log(myiso.triangleType)
Log(myrect.findArea())
Log(mysqr.findArea())
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
Log("Area: " + myObj.width * myObj.height)
Log(myObj.findArea())

myObj.width = 23.6
myObj.height = 13.287
Log(myObj.findArea())
Log()
var i = 1
var len = 10
do {
	i = i + 1
		if (i >= 4) {
			if (i <= 6) {
				continue
			}
		}
	Log("Hello World " + (i * 10))
} while (i <= len)

function outerFunc(x) {
	function innerFunc(y) {
		function innermostFunc(z) {
			return (x + y) * z
		}
		Log(innermostFunc(7))
		return innermostFunc(3)
	}
	Log(innerFunc(4))
	return innerFunc(10)
}
Log("outerFunc(3): "+outerFunc(3))

/*Log((function (a,b) { return a+b })(11,16)*(function (a, b) { return a-b })(15,10))
Log((function(op1, op2) {
	return op1 + op2
	})(15, 34) * 2)
Log(14*(function (a, b) { return a-b })(11, 16))*/

Log("Hello " + me.firstName + " " + me.lastName + ". Your date of birth is: " + me.dateOfBirth.monthName + " " + me.dateOfBirth.date + ", " + me.dateOfBirth.year  +"! You are: " + (currentYear - yearOfBirth) + " years old.")
Log("Your middle names are " + me.middleNames.first + " and " + me["middle" + "Names"].second + ". " + spacemission + " occurred " + (yearOfBirth - 1969) + " years before your birth.")
Log("Second mission in 1961: " + missionsByYear[1970 - 9][2 - 1] + ". First mission in 1961: " + missionsByYear[1961][0] + ". Missions eight years later: first=" + missionsByYear[1969][0] + " second=" + missionsByYear[1969][1]);

i = 15
j = 22
k = 37

Log(josh = benjamin = mark = atkins = 1.5 * 2 * 3 / 2 * 4)
Log(i/=j*=k-=5)
Log(josh=(hey=(hello="world")+"_earth")+"_atkins")

Log((5) * (6 + ((2 + 1)-(3 / 4)) * 10))
Log(-15.2 + -3.2 / 2.4-18.7 * -6.45 + 3 / 45.286)
Log(5 / (8 + 16.25 * (91.23 / (18.25 + -(14.9 / (-18 / 2.5)) + (8 / (-6 / -25.8 * (-1.5 + (-8 * -6 * (52.3 + 18.0)) / 2.5)))))))
Log(3 + (8 / (9-4.5 * (2 + (10 * (6 / (5 + -1)-2))))))
Log((((((((((2 * 0 + 1) * 2 + 0) * 2 + 1) * 2 + 1) * 2 + 1) * 2 + 0) * 2 + 1) * 2 + 0) * 2 + 1) * 2 + 1)
Log(Multiply(Add(5, (8 * (4 / 5)), "45", 2), Divide(Add(10, -1), Divide(2, 1.5))))
Log(Add(Divide(3, 5), (8 * (4 / 5)), 45, 2))

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
Log(matrix)

function testFunc(var1, var2, var3) {
	Log("This is a test " + (var1 + var2 + var3))
}
testFunc(100, 200, 300);
testFunc(240, 480, 720);
testFunc(50, 60, 0);
Log(fib(13))
Log(fib(25))

Log("6!=" + fact(6))
Log("100!=" + fact(100))
