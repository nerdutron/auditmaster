function createRequestObject()
{
	var ro;
	var browser = navigator.appName;
	if(browser == "Microsoft Internet Explorer")
	{
		ro = new ActiveXObject("Microsoft.XMLHTTP");
	}
	else
	{
		ro = new XMLHttpRequest();
	}
	return ro;
}

var http = createRequestObject();

function GetAJAX(url)
{
	http.open('get', url);
	http.onreadystatechange = handleResponse;
	http.send(null);
}

function handleResponse()
{
	if(http.readyState == 4)
	{
		var response = http.responseText;
		var update = new Array();
	
		if(response.indexOf('|' != -1))
		{
			update = response.split('|');
			done = 0;
			i = 0;
			while (done == 0)
			{
				j = i ;
				k = i + 1;
				if (update[j])
				{
					document.getElementById(update[j]).innerHTML = update[k];
					i = i + 2;
				}
				else
				{
					done = 1;
				}
			}
		}
	}
}
