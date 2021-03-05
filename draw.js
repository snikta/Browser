var canvasEl = createElement('canvas')
canvasEl.style.background = 'rgb(200, 200, 255)'
canvasEl.width = '400px'
canvasEl.height = '400px'
canvasEl.style.display = 'block'
canvasEl.style.position = 'absolute'

var coords = createElement('h1')
var prevX = 0
var prevY = 0

var canvasMouseUp = function (e) {
	removeEventListener('mousemove', canvasMouseMove)
	removeEventListener('mouseup', canvasMouseUp)
}

var canvasMouseMove = function (e) {
	coords.textContent = 'X: ' + pageX + ', Y: ' + pageY
	DrawLine(canvasEl, prevX, prevY, pageX, pageY)
	prevX = pageX
	prevY = pageY
}

var canvasMouseDown = function (e) {
	prevX = pageX
	prevY = pageY
	addEventListener('mousemove', canvasMouseMove)
	addEventListener('mouseup', canvasMouseUp)
}
addEventListener('mousedown', canvasMouseDown)
