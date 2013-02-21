fadeUp=function(element,red,green,blue){
	if(element.fade){
		window.clearTimeout(element.fade);
	}
	var cssValue = "rgb("+red+","+green+","+blue+")";
	$(element).css("background-color",cssValue);
	if(red == 255 && green == 255 && blue == 255){
		$(element).css("background-color","");
		return;
	}
	var newRed = red + Math.ceil((255-red)/10);
	var newGreen = green + Math.ceil((255-green)/10);
	var newBlue = blue + Math.ceil((255-blue)/10);
	var repeat = function(){
		fadeUp(element,newRed,newGreen,newBlue);
	};
	element.fade=window.setTimeout(repeat,100);
}