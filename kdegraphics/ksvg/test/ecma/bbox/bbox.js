function clear_test(evt, number)
{
	for(var i = 0; i < number; i++)
	{
		var element = evt.target.getOwnerDocument().getElementById("bbox" + (i + 1));
		element.getParentNode().removeChild(element);
	}
}

function bbox_loop(drawit, number)
{
	var doc = evt.target.getOwnerDocument();

	for(var i = 0; i < number; i++)
	{
		var shape = doc.getElementById("test-" + i);
		var bbox = shape.getBBox();
	
		if(drawit == "true")
		{
			draw_it(doc, bbox, shape, i + 1);
		}
		else
		{
			do_string(bbox, i + 1);
		}
	}
}	

function do_string(shape, number)
{
	var string = "\nShape " + number + "\nX = " + shape.x + "\nY = " + shape.y + "\nW = " + shape.width + "\nH = " + shape.height;
	alert(string);
}

function draw_it(doc, bbox, shape, number)
{
	var element = doc.createElement("rect");
	element.setAttribute("x", bbox.x);
	element.setAttribute("y", bbox.y);
	element.setAttribute("width", bbox.width);
	element.setAttribute("height", bbox.height);
	element.setAttribute("fill", "none");
	element.setAttribute("stroke", "red");
	element.setAttribute("stroke-width", "3");
	element.setAttribute("id", "bbox" + number);
	
	shape.getParentNode().appendChild(element);
}

function gen_buttons(evt, number)
{
	var doc = evt.target.getOwnerDocument();
	var main = doc.getElementById("svg-root");
	
	var aone = doc.createElement("a");
	var atwo = doc.createElement("a");
	var athree = doc.createElement("a");
	
	var rect = doc.createElement("rect");
	var text = doc.createElement("text");

	aone.setAttribute("onclick", "bbox_loop('false'," + number + ")");

	atwo.setAttribute("onclick", "bbox_loop('true'," + number + ")");
	atwo.setAttribute("transform", "translate(0,22)");
		
	athree.setAttribute("onclick", "clear_test(evt," + number + ")");
	athree.setAttribute("transform", "translate(0,44)");

	rect.setAttribute("x", "0");
	rect.setAttribute("y", "0");
	rect.setAttribute("width", "110");
	rect.setAttribute("height", "20");

	text.setAttribute("x", "0");
	text.setAttribute("y", "15");
	text.setAttribute("fill", "white");

	// First button
	aone.appendChild(rect.cloneNode(1));
	var aonetext = text.cloneNode(1);
	aonetext.appendChild(doc.createTextNode("Calculate bbox"));
	aone.appendChild(aonetext);

	main.appendChild(aone);

	// Second button
	atwo.appendChild(rect.cloneNode(1));
	var atwotext = text.cloneNode(1);
	atwotext.appendChild(doc.createTextNode("Draw bbox"));
	atwo.appendChild(atwotext);

	main.appendChild(atwo);

	// Third button
	athree.appendChild(rect.cloneNode(1));
	var athreetext = text.cloneNode(1);
	athreetext.appendChild(doc.createTextNode("Clear bbox drawing"));
	athree.appendChild(athreetext);

	main.appendChild(athree);
}
