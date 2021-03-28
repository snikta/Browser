var clamp = function (value, minLimit, maxLimit) {
    return min(max(value, minLimit), maxLimit)
}

var ColorPicker = function () {
	var hueBar = createElement('div')
	var saturationValueBoxContainer = createElement('div')
	var saturationValueBox = createElement('div')
	var satValInnerGrad = createElement('div')
	var hexCode = createElement('p')
	var hexCodeContainer = createElement('div')
	var satValVerticalLine = createElement('div')
	var satValHorizontalLine = createElement('div')
	var btnHueBarSlider = createElement('button')
	var colorPickerDiv = createElement('div')
	var that = this
	
	hexCode.id = 'hexCode'
	hexCodeContainer.id = 'hexCodeContainer'
	hueBar.id = 'hueBar'
	hueBar.style.background = 'linear-gradient(toright,rgb(255,0,0),rgb(255,255,0),rgb(0,255,0),rgb(0,255,255),rgb(0,0,255),rgb(255,0,255),rgb(255,0,0))'
	var moveHueBarSlider = function (e) {
		var hueBarWidth = getWidth(hueBar)
		if (hueBarWidth > 0) {
			var xDiff = clamp(pageX - getLeft(hueBar) - getWidth(btnHueBarSlider) / 2, 0, getWidth(hueBar) - getWidth(btnHueBarSlider))
			/*btnHueBarSlider.style.left = ((xDiff + getLeft(hueBar)) / innerWidth * 100) + '%'*/
			var hsl = hsvToRgb((xDiff / hueBarWidth * 360), 1.0, 1.0)
			saturationValueBox.style.background = 'linear-gradient(toright,rgb(255,255,255),' + hsl + ')'
			hueBar.hue = round(xDiff / getWidth(hueBar) * 360)
			var shapeCount = length(selectedShapes)
			var i = 0
			for ( i = 0; i <= shapeCount - 1; i = i + 1) {
				var shape = selectedShapes[i]
				shape.color = hsl
			}
			drawAllShapes()
		}
	}
	var dropHueBarSlider = function (e) {
		removeEventListener('mousemove', moveHueBarSlider)
		removeEventListener('mouseup', dropHueBarSlider)
	}
	var hueBarMouseDown = function (e) {
		if (pageX >= getLeft(hueBar)) {
			showSatValContainer()
			
			addEventListener('mousemove', moveHueBarSlider)
			addEventListener('mouseup', dropHueBarSlider)
		}
	}
	addEventListener('mousedown', hueBarMouseDown)
	/*saturationValueBoxContainer.onmousedown = function (e) { e.preventDefault(); e.stopPropagation(); }*/
	btnHueBarSlider.id = 'btnHueBarSlider'
	appendChild(hueBar, btnHueBarSlider)
	satValVerticalLine.id = 'satValVerticalLine'
	satValHorizontalLine.id = 'satValHorizontalLine'
	satValInnerGrad.id = 'satValInnerGrad'
	satValInnerGrad.style.background = 'linear-gradient(totop,rgba(0,0,0,1.0),rgba(0,0,0,0.0))'
	appendChild(satValInnerGrad, satValVerticalLine)
	appendChild(satValInnerGrad, satValHorizontalLine)
	appendChild(saturationValueBox, satValInnerGrad)
	saturationValueBox.id = 'saturationValueBox'
	saturationValueBox.style.background = 'linear-gradient(toright,rgba(255,255,255,1.0),rgba(255,0,0,0.0))'
	saturationValueBoxContainer.id = 'saturationValueBoxContainer'
	/*saturationValueBoxContainer["class"] = 'invisible'*/
	appendChild(saturationValueBoxContainer, saturationValueBox)
	hexCode.textContent = 'rgba(129,198,255,1) #81c6ff'
	appendChild(hexCodeContainer, hexCode)
	appendChild(saturationValueBoxContainer, hexCodeContainer)
	
	colorPickerDiv.id = 'colorPickerDiv'
	appendChild(colorPickerDiv, hueBar)
	appendChild(colorPickerDiv, saturationValueBoxContainer)
	/*this.colorPickerDiv = colorPickerDiv*/
	colorPickerDiv.style.left = (innerWidth - 320) + 'px'
	
	hueBar.hue = 0
	satValInnerGrad.x = 320
	satValInnerGrad.y = 0
	satValVerticalLine.style.left = (satValInnerGrad.x / innerWidth * 100) + '%'
	satValHorizontalLine.style.top = '0%'
	
	var renderPreviewColor = function () {
		var hue = hueBar.hue
		var rgbColor = hsvToRgb(hue, clamp((satValInnerGrad.x) / getWidth(satValInnerGrad), 0, 1), clamp(1 - (satValInnerGrad.y) / getHeight(satValInnerGrad), 0, 1))
		/*var rgbCode = substr(rgbColor, 4, length(rgbColor) - 1).split(',')
		var hexified
		hexified = hexifyRGB(rgbCode)
		rgbColor = 'rgba(' + rgbCode.join(',').replace(/\s/g, '') + ')'
		
		hexCode.rgbColor = rgbColor
		hexCode.textContent = rgbColor + ' ' + hexified
		hexCodeContainer.style.backgroundColor = rgbColor*/
	}
	
	var moveColorPickerShadeHandle = function (e) {
		satValInnerGrad.x = clamp(pageX - getLeft(satValInnerGrad), 0, getWidth(satValInnerGrad))
		satValInnerGrad.y = clamp(pageY - getTop(satValInnerGrad), 0, getHeight(satValInnerGrad))
		satValVerticalLine.style.left = (satValInnerGrad.x / innerWidth * 100) + '%'
		satValHorizontalLine.style.top = (satValInnerGrad.y / innerHeight * 100) + '%'
		
		renderPreviewColor()
		/*e.preventDefault()*/
	}
	var dropColorPickerShadeHandle = function (e) {
		/*changeColor('Fill', hexCode.rgbColor)*/
		removeEventListener('mousemove', moveColorPickerShadeHandle)
		removeEventListener('mouseup', dropColorPickerShadeHandle)
	}
	var satValInnerGradMouseDown = function (e) {
		addEventListener('mousemove', moveColorPickerShadeHandle)
		addEventListener('mouseup', dropColorPickerShadeHandle)
	}
	addEventListener(satValInnerGrad, 'mousedown', satValInnerGradMouseDown)
	
	var showSatValContainer = function () {
		saturationValueBoxContainer["class"] = ''
	}
	this.showSatValContainer = showSatValContainer
}

var myColorPicker = new ColorPicker()
myColorPicker.showSatValContainer()
