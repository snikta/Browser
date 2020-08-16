var header = createElement("div")
header.id = "header"

/*var colors = ["rgb(255,0,0)", "rgb(255,255,0)", "rgb(191,255,0)", "rgb(0,255,255)", "rgb(0,0,255)", "rgb(255,0,255)", "rgb(255,0,0)"]
var pickColor = function () {
	selectedElement.style.background = this.style.background
}

for (i = 0, len = length(colors); i <= len - 1; i = i + 1) {
	var colorButton = createElement('div')
	colorButton.style.display = 'block'
	colorButton.style.width = (32 / innerWidth * 100) + '%'
	colorButton.style.height = (32 / innerHeight * 100) + '%'
	colorButton.style.left = (i * 32 / innerWidth * 100) + '%'
	colorButton.style.top = '0%'
	colorButton.style.background = colors[i]
	addEventListener(colorButton, 'mousedown', pickColor)
}*/

Alert("Done!")

var startX = 0
var startY = 0
var selectedElement = header

var shapeMouseUp = function (e) {
	removeEventListener('mousemove', shapeMouseMove)
	removeEventListener('mouseup', shapeMouseUp)
}

var shapeMouseMove = function (e) {
	var oldLeft = getLeft(selectedElement) / innerWidth * 100
	var oldTop = getTop(selectedElement) / innerHeight * 100 - getTop(header) / innerHeight * 100
	
	var left = getLeft(selectedElement) / innerWidth * 100
	var top = getTop(selectedElement) / innerHeight * 100
	var width = getWidth(selectedElement) / innerWidth * 100
	var height = getHeight(selectedElement) / innerHeight * 100
	
	moveHandles(left, top, width, height)
	
	selectedElement.style.left = (oldLeft + ((pageX - startX) / innerWidth * 100)) + '%'
	selectedElement.style.top = (oldTop + ((pageY - startY) / innerHeight * 100)) + '%'
	
	startX = pageX
	startY = pageY
}

var nwHandle = createElement('div')
var neHandle = createElement('div')
var seHandle = createElement('div')
var swHandle = createElement('div')

nwHandle.id = 'nwHandle'
neHandle.id = 'neHandle'
seHandle.id = 'seHandle'
swHandle.id = 'swHandle'

nwHandle["class"] = 'handle'
neHandle["class"] = 'handle'
seHandle["class"] = 'handle'
swHandle["class"] = 'handle'

nwHandle.style.width = 8 / innerWidth * 100 + '%'
nwHandle.style.height = 8 / innerHeight * 100 + '%'
neHandle.style.width = 8 / innerWidth * 100 + '%'
neHandle.style.height = 8 / innerHeight * 100 + '%'
seHandle.style.width = 8 / innerWidth * 100 + '%'
seHandle.style.height = 8 / innerHeight * 100 + '%'
swHandle.style.width = 8 / innerWidth * 100 + '%'
swHandle.style.height = 8 / innerHeight * 100 + '%'

var anchorHandle = nwHandle
var clickedHandle = seHandle

var selectHandle = function (e) {
	clickedHandle = this
	
	if (clickedHandle.id == 'nwHandle') {
		anchorHandle = seHandle
	} else if (clickedHandle.id == 'neHandle') {
		anchorHandle = swHandle
	} else if (clickedHandle.id == 'seHandle') {
		anchorHandle = nwHandle
	} else if (clickedHandle.id == 'swHandle') {
		anchorHandle = neHandle
	}
	
	addEventListener('mousemove', dragHandle)
	addEventListener('mouseup', dropHandle)
}

var dragHandle = function (e) {
	clickedHandle.style.left = ((pageX - 4) / innerWidth * 100) + '%'
	clickedHandle.style.top = ((pageY - 4) / innerHeight * 100) + '%'
	
	var anchorLeft = getLeft(anchorHandle)
	var anchorTop = getTop(anchorHandle)
	var clickedLeft = getLeft(clickedHandle)
	var clickedTop = getTop(clickedHandle)
	
	var left = min(anchorLeft, clickedLeft)
	var top = min(anchorTop, clickedTop)
	var right = max(anchorLeft, clickedLeft)
	var bottom = max(anchorTop, clickedTop)
	var width = right - left - 16 * 2
	var height = bottom - top - 16 * 2
	
	selectedElement.style.left = ((left + 16) / innerWidth * 100 - getLeft(header) / innerWidth * 100) + '%'
	selectedElement.style.top = ((top + 16) / innerHeight * 100 - getTop(header) / innerHeight * 100) + '%'
	selectedElement.style.width = (width / innerWidth * 100) + '%'
	selectedElement.style.height = (height / innerHeight * 100) + '%'
}

var dropHandle = function (e) {
	var left = getLeft(selectedElement) / innerWidth * 100
	var top = getTop(selectedElement) / innerHeight * 100
	var width = getWidth(selectedElement) / innerWidth * 100
	var height = getHeight(selectedElement) / innerHeight * 100
	
	moveHandles(left, top, width, height)
	
	removeEventListener('mousemove', dragHandle)
	removeEventListener('mouseup', dropHandle)
}

addEventListener(nwHandle, 'mousedown', selectHandle)
addEventListener(neHandle, 'mousedown', selectHandle)
addEventListener(seHandle, 'mousedown', selectHandle)
addEventListener(swHandle, 'mousedown', selectHandle)

var moveHandles = function (left, top, width, height) {
	nwHandle.style.left = (left - 16 / innerWidth * 100) + '%'
	nwHandle.style.top = (top - 16 / innerHeight * 100) + '%'
	
	neHandle.style.left = (left + 16 / innerWidth * 100 + width) + '%'
	neHandle.style.top = (top - 16 / innerHeight * 100) + '%'
	
	seHandle.style.left = (left + 16 / innerWidth * 100 + width) + '%'
	seHandle.style.top = (top + 16 / innerHeight * 100 + height) + '%'
	
	swHandle.style.left = (left - 16 / innerWidth * 100) + '%'
	swHandle.style.top = (top + 16 / innerHeight * 100 + height) + '%'
}

var shapeMouseDown = function (e) {
	selectedElement = this
	
	var left = getLeft(selectedElement) / innerWidth * 100
	var top = getTop(selectedElement) / innerHeight * 100
	var width = getWidth(selectedElement) / innerWidth * 100
	var height = getHeight(selectedElement) / innerHeight * 100
	
	startX = pageX
	startY = pageY
	
	moveHandles(left, top, width, height)
	
	addEventListener('mousemove', shapeMouseMove)
	addEventListener('mouseup', shapeMouseUp)
}

var mouseMoveHandler = function (e) {
	var minX = min(pageX, startX)
	var minY = min(pageY, startY)
	var maxX = max(pageX, startX)
	var maxY = max(pageY, startY)
	var width = abs(maxX - minX)
	var height = abs(maxY - minY)
	
	selectedElement.style.left = (minX / innerWidth * 100 - getLeft(header) / innerWidth * 100) + '%'
	selectedElement.style.top = (minY / innerHeight * 100 - getTop(header) / innerHeight * 100) + '%'
	selectedElement.style.width = (width / innerWidth * 100) + '%'
	selectedElement.style.height = (height / innerHeight * 100) + '%'
}
var mouseUpHandler = function (e) {
	addEventListener(selectedElement, 'mousedown', shapeMouseDown)
	removeEventListener('mousemove', mouseMoveHandler)
	removeEventListener('mouseup', mouseUpHandler)
}

function mouseDownHandler(e) {
	selectedElement = createElement('div')
	selectedElement.style.background = 'rgb(0,0,0)'
	selectedElement.style.position = 'fixed'
	selectedElement.style.display = 'block'
	startX = pageX
	startY = pageY
	addEventListener('mousemove', mouseMoveHandler)
	addEventListener('mouseup', mouseUpHandler)
}

addEventListener('mousedown', mouseDownHandler)

var myColorPicker = new ColorPicker()
myColorPicker.showSatValContainer()
