/*var header = createElement ("div")
header.id = "header"
var theSpan = createElement("span")
theSpan.textContent = "the "
var bEl = createElement("b")
var quickSpan = createElement("span")
quickSpan.textContent = "quick "
var iEl = createElement("i")
iEl.textContent = "browny"
var foxSpan = createElement("span")
foxSpan.textContent = " fox"
var jumpsSpan = createElement("span")
jumpsSpan.textContent = " jumps over the "
var lazyEl = createElement("i")
lazyEl.textContent = "lazy"
var dogSpan = createElement("span")
var main = createElement("div")
var sidebar = createElement("div")
var content = createElement("div")
var tableContainer = createElement('div')
var tableEl = createElement('table')
var tr1 = createElement('tr')
var tr2 = createElement('tr')
var tr3 = createElement('tr')
var thID = createElement('th')
var td1 = createElement('td')
var td2 = createElement('td')
var thFirstName = createElement('th')
var tdFirstName1 = createElement('td')
var tdFirstName2 = createElement('td')
var thLastName = createElement('th')
var thDateOfBirth = createElement('th')
var thEmail = createElement('th')
var tdAtkins = createElement('td')
var tdAtkinsDOB = createElement('td')
var tdAtkinsEmail = createElement('td')
var tdMusk = createElement('td')
var tdMuskDOB = createElement('td')
var tdMuskEmail = createElement('td')
var p1 = createElement('p')
var p2 = createElement('p')
var p3 = createElement('p')
var p4 = createElement('p')
var p5 = createElement('p')
var p6 = createElement('p')
var contentFooter = createElement('div')
var footer = createElement('div')
var footerLeftColumn = createElement('div')
var footerRightColumn = createElement('div')

contentFooter.id = 'contentFooter'
footer.id = 'footer'
footerLeftColumn["class"] = 'halfWide'
footerRightColumn["class"] = 'halfWide hwRight'
contentFooter.textContent = 'contentFooter'
footerLeftColumn.textContent = 'left column'
footerRightColumn.textContent = 'right column'

dogSpan.textContent = " dog"
appendChild(header, theSpan)
appendChild(bEl, quickSpan)
appendChild(bEl, iEl)
appendChild(bEl, foxSpan)
appendChild(header, bEl)
appendChild(header, jumpsSpan)
appendChild(header, lazyEl)
appendChild(header, dogSpan)

main.id = "main"
sidebar.id = "sidebar"
appendChild(main, sidebar)

content.id = 'content'

p1.textContent = 'This is a paragraph. Some text goes here.'
p2.textContent = 'This is another paragraph. Some more text goes here.'
p3.textContent = "Lipsum could go here, but doesn't."
p4.textContent = 'Again the code is also really simple and pretty easy to understand.'
p5.textContent = 'In the void setup, we are saying that we say that pin 7 is going to act as an output as this will be our LED pin, then down in the void loop, we say that sensorValue is the analog value of pin A0 which is then used in an if statement.'
p6.textContent = 'This if statement states that if the sensorValue is above 50 to turn on pin 7 which is the LED pin and send back \"Rumble On\" to the serial monitor and if the sensorValue is below 50 to keep the LED off and send back \"Rumble Off\" to the serial monitor.'

appendChild(main, content)
appendChild(content, p1)
appendChild(content, p2)
appendChild(content, p3)
appendChild(content, p4)
appendChild(content, p5)
appendChild(content, p6)

tableContainer.id = 'tableContainer'

appendChild(tableContainer, tableEl)
appendChild(content, tableContainer)
appendChild(tableEl, tr1)
appendChild(tableEl, tr2)
appendChild(tableEl, tr3)
appendChild(tr1, thID)
appendChild(tr2, td1)
appendChild(tr3, td2)
appendChild(tr1, thFirstName)
appendChild(tr2, tdFirstName1)
appendChild(tr3, tdFirstName2)
appendChild(tr1, thLastName)
appendChild(tr2, tdAtkins)
appendChild(tr3, tdMusk)
appendChild(tr1, thDateOfBirth)
appendChild(tr2, tdAtkinsDOB)
appendChild(tr3, tdMuskDOB)
appendChild(tr1, thEmail)
appendChild(tr2, tdAtkinsEmail)
appendChild(tr3, tdMuskEmail)

thFirstName["class"] = "firstName"
tdFirstName1["class"] = "firstName"
tdFirstName2["class"] = "firstName"

thID.textContent = 'ID'
td1.textContent = '1'
td2.textContent = '2'
thFirstName.textContent = 'First Name'
tdFirstName1.textContent = 'Josh'
tdFirstName2.textContent = 'Elon'
thLastName.textContent = 'Last Name'
tdAtkins.textContent = 'Atkins'
tdMusk.textContent = 'Musk'
thDateOfBirth.textContent = 'Date of Birth'
tdAtkinsDOB.textContent = '1994-04-15'
tdMuskDOB.textContent = '1971-06-28'
thEmail.textContent = 'Email'
tdAtkinsEmail.textContent = 'joshatkins94@gmail.com'
tdMuskEmail.textContent = 'elon@musk.com'

appendChild(content, contentFooter)
appendChild(footer, footerLeftColumn)
appendChild(footer, footerRightColumn)*/

var header = createElement ("div")
header.id = "header"

/*var imgApollo = createElement("img")

imgApollo.src = "apollo.jpg"
imgApollo.width = "233"
imgApollo.height = "155"
imgApollo.style.display = 'block'
imgApollo.style.position = 'fixed'*/

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
