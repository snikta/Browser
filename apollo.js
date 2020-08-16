var header = createElement("div")
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
appendChild(footer, footerRightColumn)

var header = createElement ("div")
header.id = "header"

var imgApollo = createElement("img")

imgApollo.src = "apollo.jpg"
imgApollo.width = "233"
imgApollo.height = "155"
imgApollo.style.display = 'block'
imgApollo.style.position = 'fixed'
