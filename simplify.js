testPoints = {0:{x:12,y:13},1:{x:15,y:12},2:{x:17,y:10},3:{x:20,y:15},4:{x:23,y:23},5:{x:34,y:25},6:{x:38,y:34},7:{x:45,y:40}}
i = 0
len = length(testPoints)
for (; i < len; i = i + 1) {
	Log("(" + testPoints[i].x + ", " + testPoints[i].y + ")")
}
