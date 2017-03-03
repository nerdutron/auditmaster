<?php

// number of labels on a page
$LABEL_LAYOUT["Across"] = 3; // number of columns
$LABEL_LAYOUT["Down"] = 8; // number of rows

// paper size
$LABEL_LAYOUT["PageSize"] = "a4 portrait";
// or
$LABEL_LAYOUT["PageSize"] = "a4 landscape";
// or
$LABEL_LAYOUT["PageSize"] = "a3 landscape";
// etc

// margin on the outside of the page
$LABEL_LAYOUT["PageMargins"] = 5; // 5mm of blank space on all sides of the page
// or
$LABEL_LAYOUT["PageMargins"] = array(
	5, // 5mm border on top
	10, // 10mm border on bottom
	15, // 15mm border on left
	20 // 20mm border on right
	);

// spacing between the labels (horizontal, vertical)
// 5mm between sides of labels, 3mm separating the top of a label from the one above
// this is not added to the margins around the edge of the page!
$LABEL_LAYOUT["Spacing"] = array(5, 3);

// font name
$LABEL_LAYOUT["FontName"] = "Helvetica";

// font size
$LABEL_LAYOUT["FontSize"] = 10;


?>
