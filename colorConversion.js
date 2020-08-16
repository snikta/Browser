function hsvToRgb(h, s, v) {
	h = round(h)
	var c = v * s
	var x = c * (1 - abs(((h/60) % 2) - 1))
	var m = v - c
	var r_ = 0
	var g_ = 0
	var b_ = 0
	
	if (h == 360) { h = 0 }
	
	if (h >= 0 && h < 60) {
		r_ = c
		g_ = x
		b_ = 0
	} else if (h >= 60 && h < 120) {
		r_ = x
		g_ = c
		b_ = 0
	} else if (h >= 120 && h < 180) {
		r_ = 0
		g_ = c
		b_ = x
	} else if (h >= 180 && h < 240) {
		r_ = 0
		g_ = x
		b_ = c
	} else if (h >= 240 && h < 300) {
		r_ = x
		g_ = 0
		b_ = c
	} else if (h >= 300 && h < 360) {
		r_ = c
		g_ = 0
		b_ = x
	}
	
	return 'rgb(' + round((r_ + m) * 255) + ',' + round((g_ + m) * 255) + ',' + round((b_ + m) * 255) + ')'
}
function rgbToHsv(r, g, b) {
	var r_ = r / 255
	var g_ = g / 255
	var b_ = b / 255
	var Cmax = max(r_, g_, b_)
	var Cmin = min(r_, g_, b_)
	var delta = Cmax - Cmin
	var H = 0
	var S = 0
	var V = 0
	
	if (delta === 0) {
		H = 0
	} else if (Cmax === r_) {
		H = 60 * (((g_ - b_) / delta) % 6)
	} else if (Cmax === g_) {
		H = 60 * (((b_ - r_) / delta) + 2)
	} else if (Cmax === b_) {
		H = 60 * (((r_ - g_) / delta) + 4)
	}
	
	if (H < 0) { H = 360 + H }
	
	if (Cmax == 0) {
		S = 0
	} else {
		S = delta / Cmax
	}
	V = Cmax
	
	return {h: H, s: S, v: V}
}
function hexifyRGB(rgb) {
	var hex, i
	hex = [toHex(rgb[0]), toHex(rgb[1]), toHex(rgb[2])]
	if (length(rgb) === 4) {
		hex[length(hex)] = toHex(rgb[3])
	}
	
	for (i = 0, len = length(rgb); i < len; i = i + 1) {
		if (length(hex[i]) == 0) {
			hex[i] = '00'
		} else if (length(hex[i]) == 1) {
			hex[i] = '0' + hex[i]
		}
	}
	
	var retval = '#'
	for (i = 0, len = length(hex); i < len; i = i + 1) {
		retval += hex[i]
	}
	
	return retval
}