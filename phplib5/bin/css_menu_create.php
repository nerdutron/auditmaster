<?php

/* create definitions */
$CSSMenu = array();

/* width: full width of a button */
$CSSMenu["width_full"] = "120px";
/* width: full width minus some room for the link/arrow graphic */
$CSSMenu["width_short"] = "114px";

/* font: link text size and font */
$CSSMenu["font"] = "12px sans-serif";
/* font-weight: link text weight (bold) */
$CSSMenu["weight"] = "normal";

/* border: outer border for each item */
$CSSMenu["border"] = "solid 1px #FFFFFF";

/* background-color: normal */
$CSSMenu["background"] = "#3E5C97";
/* background-color: hover */
$CSSMenu["background_hover"] = "#657DAC";

/* color: link text */
$CSSMenu["link"] = "#FFFFFF";
/* color: link text hover */
$CSSMenu["link_hover"] = "#FFFFFF";

/* create files */

file_put_contents("css_menu.css", CSS($CSSMenu));
file_put_contents("css_menu_ie.htc", JS($CSSMenu));
file_put_contents("pixel.gif", Pixel());

/* instructions */
echo "\n\n";
echo "\n\n";
echo "Don't forget to add 'css_sub_menu.gif' to this directory.\n";
echo "\n\n";
echo "Insert the following code above your menu:\n";
echo "
<link rel=\"stylesheet\" type=\"text/css\" media=\"screen\" href=\"[insert_menu_name_here]/css_menu.css\" />
<?php

echo \"<!--[if gte IE 5]>\\n\";
echo \"	<style type=\\\"text/css\\\">\\n\";
echo \"		ul.css_menu ul { \\n\";
echo \"			display: none; \\n\";
echo \"			position: absolute; \\n\";
echo \"			top: 4px; \\n\";
echo \"			left: ".$CSSMenu["width_full"]."; }\\n\"; /* width (full) */
echo \"	ul.css_menu li { \\n\";
echo \"		behavior: url( [insert_menu_name_here]/css_menu_ie.htc );}\\n\";
echo \"	</style>\\n\";
echo \"<![endif]-->\\n\";

";

echo "


Your menu should look like this:

<ul class=\"css_menu\">
<li class=\"sub\"><a href=\"[menu link]\">[menu text]</a></li> // indicates an attached sub menu
<ul class=\"css_menu\">
<li><a href=\"[menu link]\">[menu text]</a></li>
</ul
<li><a href=\"[menu link]\">[menu text]</a></li>
</ul>


";


function CSS($CSSMenu)
{
	$File = "";

	$File .= "/* css_menu css\n";
	$File .= "---------------------------------*/\n\n";

	$File .= "ul.css_menu, ul.css_menu ul\n";
	$File .= "{\n";
	$File .= "	width: ".$CSSMenu["width_full"]."; /* width (full) */\n";
	$File .= "	height: ".$CSSMenu["width_full"]."; /* width (full) */\n";
	$File .= "	padding: 0px;\n";
	$File .= "	margin: 0px;\n";
	$File .= "}\n\n";

	$File .= "ul.css_menu li\n";
	$File .= "{\n";
	$File .= "	list-style-type: none;\n";
	$File .= "	list-style-image: url(pixel.gif); /* single transparent pixel gif */\n";
	$File .= "	list-style-position: outside;\n";
	$File .= "	width: ".$CSSMenu["width_short"]."; /* width (short) */\n";
	$File .= "	margin: -1px 0px 0px 0px;\n";
	$File .= "	padding: 3px 3px 3px 3px;\n";
	$File .= "	position: relative;\n";
	$File .= "	border: ".$CSSMenu["border"]."; /* border outer */\n";
	$File .= "	font: ".$CSSMenu["font"]."; /* font */\n";
	$File .= "	background-color: ".$CSSMenu["background"]."; /* background */\n";
	$File .= "}\n\n";

	$File .= "ul.css_menu li>ul\n";
	$File .= "{\n";
	$File .= "	display: none;\n";
	$File .= "	position: absolute;\n";
	$File .= "	top: 4px;\n";
	$File .= "	left: ".$CSSMenu["width_full"]."; /* width (full) */\n";
	$File .= "}\n\n";

	$File .= "li.sub\n";
	$File .= "{\n";
	$File .= "	background: url(\"css_menu_sub.gif\") no-repeat right top;  /* arrow image */\n";
	$File .= "}\n\n";

	$File .= "ul.css_menu li:hover\n";
	$File .= "{\n";
	$File .= "	background: ".$CSSMenu["background_hover"]."; /* background hover  */\n";
	$File .= "}\n\n";

	$File .= "ul.css_menu li:hover>ul\n";
	$File .= "{\n";
	$File .= "	display: block;\n";
	$File .= "}\n\n";

	$File .= "ul.css_menu li a\n";
	$File .= "{\n";
	$File .= "	text-decoration: none;\n";
	$File .= "	display: block;\n";
	$File .= "	width: ".$CSSMenu["width_short"]."; /* width (short) */\n";
	$File .= "	font-weight: ".$CSSMenu["weight"]."; /* bold */\n";
	$File .= "}\n\n";

	$File .= "ul.css_menu li  a:link,\n";
	$File .= "ul.css_menu li  a:visited\n";
	$File .= "{\n";
	$File .= "	color: ".$CSSMenu["link"]."; /* link  (visited) */\n";
	$File .= "}\n\n";

	$File .= "ul.css_menu li  a:hover\n";
	$File .= "{\n";
	$File .= "	color: ".$CSSMenu["link_hover"]."; /* hover link */\n";
	$File .= "}\n\n";

	return ($File);
}

function JS($CSSMenu)
{
	$File = "";

	$File .= "<attach event=\"onmouseover\" handler=\"rollOver\" />\n";
	$File .= "<attach event=\"onmouseout\" handler=\"rollOff\" />\n";

	$File .= "<script type=\"text/javascript\">\n";

	$File .= "//in IE 5.5-, when you apply a width to UL, list-style-position: inside; is forced with its LIs\n";
	$File .= "if( !document.compatMode && navigator.userAgent.indexOf( 'Win' ) + 1 ) { element.style.marginLeft = -16; }\n";

	$File .= "function rollOver() {\n";
	$File .= "//change the color\n";
	$File .= "element.style.backgroundColor = '".$CSSMenu["background_hover"]."'; /* background hover */\n";
	$File .= "element.style.color = '".$CSSMenu["link_hover"]."'; /* link hover */\n";
	$File .= "//change display of child\n";
	$File .= "for( var x = 0; element.childNodes[x]; x++ ){\n";
	$File .= "if( element.childNodes[x].tagName == 'UL' ) { element.childNodes[x].style.display = 'block'; }\n";
	$File .= "}\n";
	$File .= "}\n";

	$File .= "function rollOff() {\n";
	$File .= "//change the color\n";
	$File .= "element.style.backgroundColor = '".$CSSMenu["background"]."'; /* background */\n";
	$File .= "element.style.color = '".$CSSMenu["link"]."'; /* link */\n";
	$File .= "//change display of child\n";
	$File .= "for( var x = 0; element.childNodes[x]; x++ ){\n";
	$File .= "if( element.childNodes[x].tagName == 'UL' ) { element.childNodes[x].style.display = 'none'; }\n";
	$File .= "}\n";
	$File .= "}\n";

	$File .= "</script>\n";

	return ($File);
}

function Pixel()
{
	return (base64_decode("R0lGODlhAQABAIAAAP///////yH5BAUUAAAALAAAAAABAAEAAAICRAEAOw=="));
}


?>
