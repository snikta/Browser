var canvasEl = createElement('canvas')
canvasEl.style.background = 'rgb(200, 200, 255)'
canvasEl.width = '400px'
canvasEl.height = '400px'
canvasEl.style.display = 'block'
canvasEl.style.position = 'absolute'

var coords = createElement('h1')

var canvasMouseUp = function (e) {
	removeEventListener('mousemove', canvasMouseMove)
	removeEventListener('mouseup', canvasMouseUp)
}

var canvasMouseMove = function (e) {
	coords.textContent = 'X: ' + pageX + ', Y: ' + pageY
	SetPixel(canvasEl, pageX, pageY)
}

var canvasMouseDown = function (e) {
	addEventListener('mousemove', canvasMouseMove)
	addEventListener('mouseup', canvasMouseUp)
}
addEventListener('mousedown', canvasMouseDown)
